/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#pragma once

#include "FlatTuple.hpp"
#include <tuple>
#include <utility>

namespace fxt
{
    // Re-export std::get so fxt::get works for fxt::tuple (= std::tuple) and any
    // other type that std::get already handles.
    using std::get;

    // Single constexpr noexcept forwarding-reference overload for fxt::flat_tuple.
    //
    // flat_tuple now stores each element in a flat_leaf<I,T> base class (see
    // FlatTuple.hpp). Access is a plain static_cast to the right base — no variant,
    // no tag check, no runtime branching.
    //
    // Friendship grants access to the private base chain
    // flat_tuple → flat_storage_base → flat_leaf<I,T>. std::forward_like
    // re-applies FlatTupleT's value category and const-ness to the member,
    // covering T& / const T& / T&& / const T&&.
    template<std::size_t I, typename FlatTupleT>
        requires requires { typename std::remove_cvref_t<FlatTupleT>::flat_tuple_tag; }
    constexpr decltype(auto) get(FlatTupleT&& tuple) noexcept
    {
        using CleanTuple = std::remove_cvref_t<FlatTupleT>;
        using T          = CleanTuple::template type_at<I>;
        // Propagate const-ness from FlatTupleT into the leaf cast: without this,
        // static_cast<flat_leaf<I,T>&> would always strip const and the compiler
        // would reject calls on const-qualified tuples with "drops const qualifier".
        // std::forward_like then applies the value category (lvalue vs rvalue).
        using Leaf = std::conditional_t<
            std::is_const_v<std::remove_reference_t<FlatTupleT>>,
            const impl::flat_leaf<I, T>,
            impl::flat_leaf<I, T>>;
        return std::forward_like<FlatTupleT>(static_cast<Leaf&>(tuple).value);
    }

    namespace impl
    {
        // Resolves a type T to its index within a flat_tuple's element list — the
        // machinery behind the type-based fxt::get<T>(flat_tuple), mirroring
        // std::get<T>(std::tuple). `count` lets the caller enforce "exactly one T"
        // as a constraint (so a wrong count removes the overload rather than hard-
        // erroring), and `index` is the position of the unique match.
        template<typename T, typename FlatTupleT>
        struct flat_type_lookup;

        template<typename T, typename... Ts>
        struct flat_type_lookup<T, flat_tuple<Ts...>>
        {
            static constexpr std::size_t count = (std::size_t{std::is_same_v<T, Ts>} + ... + std::size_t{0});

            static constexpr std::size_t index = []
            {
                std::size_t idx   = 0;
                std::size_t found = 0;
                // Walk the pack left to right; on the first match record the running
                // index and short-circuit. `idx` counts the non-matches seen so far,
                // which equals the position of the match.
                (void)((std::is_same_v<T, Ts> ? (found = idx, true) : (++idx, false)) || ...);
                return found;
            }();
        };
    }    // namespace impl

    // Type-based access for fxt::flat_tuple, mirroring std::get<T>(std::tuple).
    // Constrained so T must occur exactly once (count == 1); otherwise the overload
    // drops out (no viable get, same observable effect as std::get<T>'s ill-formed
    // case). Delegates to the index-based overload above, inheriting its value-
    // category propagation, noexcept guarantee, and constexpr-ness for free.
    template<typename T, typename FlatTupleT>
        requires requires { typename std::remove_cvref_t<FlatTupleT>::flat_tuple_tag; }
              && (impl::flat_type_lookup<T, std::remove_cvref_t<FlatTupleT>>::count == 1)
    constexpr decltype(auto) get(FlatTupleT&& tuple) noexcept
    {
        constexpr std::size_t idx = impl::flat_type_lookup<T, std::remove_cvref_t<FlatTupleT>>::index;
        return fxt::get<idx>(std::forward<FlatTupleT>(tuple));
    }

    // ========================================================================
    // fxt::mget — element extraction from a monad-of-tuple
    // ========================================================================

    /**
     * @brief Extract the element at index I from a tuple inside a monadic container.
     *
     * Returns a callable that, when applied to an `expected`- or `optional`-like
     * container holding a tuple, extracts element I and re-wraps it in a new
     * container of the same kind. Supports both lvalue and rvalue monads so the
     * element can be moved out of a temporary pipeline.
     *
     * @tparam I Zero-based index of the element to extract
     * @return A pipe-adaptor callable
     *
     * @code
     * auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>
     *                {std::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mget<0>();  // fxt::expected<int, Error>{1}
     *
     * auto opt = fxt::optional<std::tuple<int, double>>{std::make_tuple(42, 3.14)};
     * auto r2 = opt | fxt::mget<1>();  // fxt::optional<double>{3.14}
     * @endcode
     */
    template<std::size_t I>
    constexpr auto mget()
    {
        // The inner callback returns by value: fxt::get yields int& / int&& depending
        // on the monad's value category, and decaying to a prvalue both satisfies
        // expected/optional (whose value type must be a non-reference) and lets an
        // rvalue pipeline move the element out instead of copying it.
        return []<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([]<typename TValue>(TValue&& t) {
                return fxt::get<I>(std::forward<TValue>(t));
            });
        };
    }

    /**
     * @brief Extract the element of type T from a tuple inside a monadic container.
     *
     * Returns a callable that, when applied to an `expected`- or `optional`-like
     * container holding a tuple, extracts the unique element of type T and re-wraps
     * it. The tuple must contain exactly one element of type T. Supports both lvalue
     * and rvalue monads.
     *
     * @tparam T Type of the element to extract (must appear exactly once in the tuple)
     * @return A pipe-adaptor callable
     *
     * @code
     * auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>
     *                {std::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mget<std::string>();  // fxt::expected<std::string, Error>{"three"}
     * @endcode
     */
    template<typename T>
    constexpr auto mget()
    {
        return []<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([]<typename TValue>(TValue&& t) {
                // fxt::get<T> resolves to std::get<T> for std::tuple and to the
                // flat_tuple overload otherwise, so mget<T> works for both.
                return fxt::get<T>(std::forward<TValue>(t));
            });
        };
    }

}    // namespace fxt
