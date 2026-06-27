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
#include "../monads/Lifted.hpp"
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
    // Nullary get<I>() / get<T>() — tuple-element adaptors for use with lifted
    // ========================================================================

    /** @brief Returns an adaptor that extracts element I from any tuple-like. */
    template<std::size_t I>
    constexpr auto get()
    {
        return []<typename T>(T&& t) { return fxt::get<I>(std::forward<T>(t)); };
    }

    /** @brief Returns an adaptor that extracts the unique element of type T from any tuple-like. */
    template<typename T>
    constexpr auto get()
    {
        return []<typename Tuple>(Tuple&& t) { return fxt::get<T>(std::forward<Tuple>(t)); };
    }

    // ========================================================================
    // fxt::mget — element extraction from a monad-of-tuple
    // ========================================================================

    /**
     * @brief Returns a pipe adaptor that extracts element I from a tuple inside a monad.
     *
     * @code
     * auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>
     *                {std::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mget<0>();  // fxt::expected<int, Error>{1}
     * @endcode
     */
    template<std::size_t I>
    constexpr auto mget() { return lifted(fxt::get<I>()); }

    /**
     * @brief Returns a pipe adaptor that extracts the unique element of type T from a tuple inside a monad.
     *
     * @code
     * auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>
     *                {std::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mget<std::string>();  // fxt::expected<std::string, Error>{"three"}
     * @endcode
     */
    template<typename T>
    constexpr auto mget() { return lifted(fxt::get<T>()); }

}    // namespace fxt
