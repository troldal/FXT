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
 * @file MZip.hpp
 * @brief Monadic zip: combine N monadic values into a single monad holding a tuple.
 *
 * fxt::mtuple_zip takes N monadic values (all fxt::optional, or all fxt::expected with
 * the same error type) and returns a single monad containing an fxt::tuple of
 * their unwrapped values. Short-circuits on the first empty/error monad.
 *
 * This enables a LINQ-like query style for monadic computations:
 *
 * @code
 * // from f in firstName, from l in lastName, from t in title
 * // select std::format("{} {} {}", t, f, l)
 * auto fullName = fxt::mtuple_zip(firstName, lastName, title)
 *               | fxt::mtuple_apply([](auto f, auto l, auto t) {
 *                     return std::format("{} {} {}", t, f, l);
 *                 });
 * @endcode
 *
 * @see fxt::mtuple_append
 * @see fxt::mapply
 */

#pragma once

#include "../concepts/IsMonad.hpp"
#include "TupleAppend.hpp"

#include <type_traits>
#include <utility>

namespace fxt
{
    /**
     * @brief Combine N monadic values into a monad containing an fxt::tuple of their values.
     *
     * All inputs must be the same kind of monad (all fxt::optional, or all
     * fxt::expected with a compatible error type). The result short-circuits
     * on the first empty/error input.
     *
     * @tparam First    Type of the first monadic value (must satisfy fxt::monad_like).
     * @tparam Rest     Types of the remaining monadic values (must satisfy fxt::monad_like).
     * @param  first    The first monadic value.
     * @param  rest     Additional monadic values to zip with the first.
     * @return A monad containing fxt::tuple<T1, T2, ..., TN> on success, or the
     *         first encountered empty/error state.
     *
     * @code
     * auto a = fxt::expected<int,         std::string>{1};
     * auto b = fxt::expected<double,      std::string>{2.5};
     * auto c = fxt::expected<std::string, std::string>{"three"};
     *
     * auto zipped = fxt::mtuple_zip(a, b, c);
     * // zipped is fxt::expected<fxt::tuple<int, double, std::string>, std::string>
     * // containing (1, 2.5, "three")
     * @endcode
     */
    template<typename First, typename... Rest>
        requires fxt::monad_like<std::remove_cvref_t<First>>
              && (fxt::monad_like<std::remove_cvref_t<Rest>> && ...)
    constexpr auto mtuple_zip(First&& first, Rest&&... rest)
    {
        // Wrap the first monad's value into a single-element fxt::tuple.
        auto seed = std::forward<First>(first).transform(
            []<typename T>(T&& v) {
                return fxt::tuple<std::remove_cvref_t<T>>{ std::forward<T>(v) };
            });

        if constexpr (sizeof...(Rest) == 0) {
            return seed;
        } else {
            // Delegate the rest to mtuple_append, which already handles
            // monadic-value appending with proper short-circuit semantics.
            return fxt::mtuple_append(std::move(seed), std::forward<Rest>(rest)...);
        }
    }

    template<typename First, typename... Rest>
    [[deprecated("Use fxt::mtuple_zip")]]
    constexpr auto mzip(First&& first, Rest&&... rest)
        -> decltype(mtuple_zip(std::forward<First>(first), std::forward<Rest>(rest)...))
    {
        return mtuple_zip(std::forward<First>(first), std::forward<Rest>(rest)...);
    }

}    // namespace fxt

