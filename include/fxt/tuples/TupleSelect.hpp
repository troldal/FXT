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

/**
 * @file Select.hpp
 * @brief Arbitrary-index / arbitrary-type element projection for tuples
 *
 * ## Plain-tuple operations
 *
 * **tuple_select<Is...>(tpl)** — build a new tuple from elements at indices `Is`
 * - Direct call: `fxt::tuple_select<0, 2>(tpl)`
 * - Curried:     `tpl | fxt::tuple_select<0, 2>()`
 *
 * **tuple_select<Ts...>(tpl)** — build a new fxt::tuple from elements of types `Ts`
 * (each type must appear exactly once; works with fxt::tuple only, not flat_tuple)
 * - Direct call: `fxt::tuple_select<int, std::string>(tpl)`
 * - Curried:     `tpl | fxt::tuple_select<int, std::string>()`
 *
 * ## Monadic lifts (fxt::expected / fxt::optional)
 *
 * **mtuple_select<Is...>()** / **mtuple_select<Ts...>()**
 * - Curried pipe form only: `container | fxt::mtuple_select<0, 2>()`
 * - Implemented via `.transform()` — works with any monad-like type, supports
 *   rvalue pipelines, and handles both fxt::tuple and fxt::flat_tuple.
 *
 * @see fxt::tuple_take, fxt::tuple_drop
 */

#pragma once

#include "../monads/Expected.hpp"
#include "../monads/Optional.hpp"
#include "FlatTuple.hpp"
#include "TupleAppend.hpp"
#include <tuple>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::tuple_select — index-based projection
    // ========================================================================

    /**
     * @brief Build a new tuple from elements at the specified indices.
     *
     * @tparam Is  Indices to select (must be valid for the input tuple size)
     * @tparam Tuple  fxt::tuple or fxt::flat_tuple (deduced)
     * @param  tpl    Source tuple (any value category)
     *
     * @code
     * auto t  = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto r  = fxt::tuple_select<0, 2>(t);   // fxt::tuple<int, const char*>{1, "three"}
     *
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto r2 = fxt::tuple_select<0, 3>(ft);  // fxt::flat_tuple<double, double>{1.0, 4.0}
     * @endcode
     */
    template<std::size_t... Is, typename Tuple>
        requires (impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>> ||
                  impl::is_flat_tuple_v<std::remove_cvref_t<Tuple>>)
    constexpr auto tuple_select(Tuple&& tpl)
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");
        return impl::make_tuple_like<Tuple>(fxt::get<Is>(std::forward<Tuple>(tpl))...);
    }

    /** @brief Curried `tuple_select<Is...>` for pipeline usage. */
    template<std::size_t... Is>
    constexpr auto tuple_select()
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::tuple_select<Is...>(std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::tuple_select — type-based projection
    // ========================================================================

    /**
     * @brief Build a new tuple from elements of the specified types.
     *
     * Each type must appear exactly once in the source tuple.
     * Works with both fxt::tuple and fxt::flat_tuple; the output kind matches the input.
     * Uniqueness is enforced by `fxt::get<T>` — a duplicate type is a compile error.
     *
     * @code
     * auto t  = fxt::make_tuple(42, 3.14, 'x');
     * auto r  = fxt::tuple_select<int, char>(t);    // fxt::tuple<int, char>{42, 'x'}
     *
     * auto ft = fxt::make_flat_tuple(1, 2.5, std::string("hi"));
     * auto r2 = fxt::tuple_select<double, int>(ft); // fxt::flat_tuple<double, int>{2.5, 1}
     * auto r3 = ft | fxt::tuple_select<std::string, int>();
     * @endcode
     */
    template<typename... Ts, typename TupleT>
        requires (impl::is_fxt_tuple_v<std::remove_cvref_t<TupleT>> ||
                  impl::is_flat_tuple_v<std::remove_cvref_t<TupleT>>)
              && (sizeof...(Ts) >= 1)
    constexpr auto tuple_select(TupleT&& tpl)
    {
        return impl::make_tuple_like<TupleT>(fxt::get<Ts>(std::forward<TupleT>(tpl))...);
    }

    /** @brief Curried type-based `tuple_select<Ts...>` for pipeline usage. */
    template<typename... Ts>
        requires (sizeof...(Ts) >= 1)
    constexpr auto tuple_select()
    {
        return []<typename TupleT>(TupleT&& tpl) {
            return impl::make_tuple_like<TupleT>(fxt::get<Ts>(std::forward<TupleT>(tpl))...);
        };
    }

    // ========================================================================
    // fxt::mtuple_select — monadic index-based projection
    // ========================================================================

    /**
     * @brief Monadic lift of `tuple_select<Is...>` — projects elements inside a monad.
     *
     * Uses `.transform()` so it works with any monad-like type and supports rvalue pipelines.
     * For plain-tuple projection use `fxt::tuple_select`.
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int, double, std::string>, Error>{
     *     fxt::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mtuple_select<0, 2>();
     * // fxt::expected<fxt::tuple<int, std::string>, Error>{1, "three"}
     * @endcode
     */
    template<std::size_t... Is>
    constexpr auto mtuple_select()
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");
        return []<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([](auto&& tpl) {
                return impl::make_tuple_like<decltype(tpl)>(
                    fxt::get<Is>(std::forward<decltype(tpl)>(tpl))...);
            });
        };
    }

    // ========================================================================
    // fxt::mtuple_select — monadic type-based projection
    // ========================================================================

    /**
     * @brief Monadic lift of `tuple_select<Ts...>` — projects elements by type inside a monad.
     *
     * Works with both fxt::tuple and fxt::flat_tuple inside the monad; the output kind matches
     * the inner tuple type. Each selected type must appear exactly once.
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int, double, std::string>, Error>{
     *     fxt::make_tuple(1, 2.0, "three")};
     * auto r = exp | fxt::mtuple_select<int, std::string>();
     * // fxt::expected<fxt::tuple<int, std::string>, Error>{1, "three"}
     *
     * auto opt = fxt::optional<fxt::flat_tuple<int, double, std::string>>{
     *     fxt::make_flat_tuple(7, 3.14, std::string("hi"))};
     * auto r2 = opt | fxt::mtuple_select<double, int>();
     * // fxt::optional<fxt::flat_tuple<double, int>>{3.14, 7}
     * @endcode
     */
    template<typename... Ts>
        requires (sizeof...(Ts) >= 1)
    constexpr auto mtuple_select()
    {
        return []<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([](auto&& tpl) {
                return impl::make_tuple_like<decltype(tpl)>(
                    fxt::get<Ts>(std::forward<decltype(tpl)>(tpl))...);
            });
        };
    }
}    // namespace fxt
