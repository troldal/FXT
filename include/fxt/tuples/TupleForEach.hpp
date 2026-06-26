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
 * @file TupleForEach.hpp
 * @brief Side-effect iteration over tuple elements: fxt::tuple_for_each and fxt::mtuple_for_each
 *
 * ## fxt::tuple_for_each
 * Applies a callable to every element of a tuple for its side-effects. Works with both
 * `fxt::tuple` (std::tuple) and `fxt::flat_tuple`. Elements are visited in order.
 * Unlike `fxt::tuple_transform`, no new tuple is produced.
 *
 * @code
 * auto t = fxt::make_tuple(1, 2.5, "hello");
 * fxt::tuple_for_each(t, [](const auto& x) { std::cout << x << '\n'; });
 *
 * // Curried / pipeline form
 * fxt::make_tuple(1, 2, 3)
 *     | fxt::tuple_for_each([](auto x) { std::cout << x << ' '; });
 * @endcode
 *
 * ## fxt::mtuple_for_each
 * Monadic lift of `tuple_for_each`: applies a callable to the elements of a tuple
 * held inside an `expected`- or `optional`-like container for its side-effects, then
 * passes the monad through unchanged so the pipeline can continue.
 * If the monad is empty / holds an error the callable is never invoked.
 *
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, double>, std::string>{
 *     fxt::make_tuple(42, 3.14)};
 *
 * exp | fxt::mtuple_for_each([](const auto& x) { std::cout << x << ' '; })
 *     | fxt::mtuple_apply_append([](int a, double b) { return a + b; });
 * @endcode
 *
 * @see fxt::tuple_transform
 * @see fxt::for_each        (ranges/ForEach.hpp — range-based counterpart)
 */

#pragma once

#include "Apply.hpp"

namespace fxt
{
    // ========================================================================
    // fxt::tuple_for_each — side-effect iteration
    // ========================================================================

    /**
     * @brief Apply @p fn to each element of @p tp for its side-effects.
     *
     * @tparam TupleT fxt::tuple or fxt::flat_tuple (deduced)
     * @tparam Fn     Callable accepting each element type (deduced)
     * @param  tp     Source tuple (any value category)
     * @param  fn     Callable to invoke on each element
     */
    template<typename TupleT, typename Fn>
    void tuple_for_each(TupleT&& tp, Fn&& fn)
    {
        fxt::apply(
            [&fn]<typename... T>(T&&... args) { (fn(std::forward<T>(args)), ...); },
            std::forward<TupleT>(tp));
    }

    /**
     * @brief Curried `tuple_for_each` for pipeline usage.
     *
     * Returns an adaptor that, when piped a tuple, applies @p fn to every element
     * for its side-effects. The adaptor captures @p fn by value and is reusable.
     *
     * @code
     * fxt::make_tuple(1, 2, 3) | fxt::tuple_for_each([](auto x) { std::cout << x; });
     * @endcode
     */
    template<typename Fn>
    constexpr auto tuple_for_each(Fn&& fn)
    {
        return [fn = std::forward<Fn>(fn)]<typename TupleT>(TupleT&& tp) {
            fxt::tuple_for_each(std::forward<TupleT>(tp), fn);
        };
    }

    // Deprecated aliases — spelling reconciled with fxt::for_each (ranges/ForEach.hpp).
    template<typename TupleT, typename Fn>
    [[deprecated("Use fxt::tuple_for_each")]]
    void tuple_foreach(TupleT&& tp, Fn&& fn)
    {
        fxt::tuple_for_each(std::forward<TupleT>(tp), std::forward<Fn>(fn));
    }

    template<typename Fn>
    [[deprecated("Use fxt::tuple_for_each")]]
    constexpr auto tuple_foreach(Fn&& fn)
    {
        return fxt::tuple_for_each(std::forward<Fn>(fn));
    }

    // ========================================================================
    // fxt::mtuple_for_each — monadic lift
    // ========================================================================

    /**
     * @brief Monadic lift of `tuple_for_each`.
     *
     * Returns an adaptor that, when piped a monad-of-tuple, applies @p fn to every
     * element of the contained tuple for its side-effects, then passes the monad
     * through unchanged so the pipeline can continue.
     *
     * - If the monad is empty / holds an error the callable is never invoked and
     *   the error / nullopt propagates.
     * - Elements are passed to @p fn as `const T&`; @p fn should not assume it can
     *   move elements out.
     *
     * @code
     * fxt::expected<fxt::tuple<int, double>, std::string>{fxt::make_tuple(1, 2.0)}
     *     | fxt::mtuple_for_each([](const auto& x) { std::cout << x << ' '; });
     * @endcode
     */
    template<typename Fn>
    constexpr auto mtuple_for_each(Fn&& fn)
    {
        return [fn = std::forward<Fn>(fn)]<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([&fn](const auto& t) {
                fxt::tuple_for_each(t, fn);
                return t;
            });
        };
    }

}    // namespace fxt
