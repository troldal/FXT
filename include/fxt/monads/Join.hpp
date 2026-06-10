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

#include <utility>
#include "../concepts/IsExpected.hpp"

namespace fxt
{
    /**
     * @brief Monadic join (flatten) operation for nested expected types
     *
     * This is the canonical monadic join: it collapses a doubly-wrapped
     * expected<expected<T, E>, E> into a single expected<T, E>. If the outer
     * expected holds an error, that error is propagated; otherwise the inner
     * expected is returned as-is, flattening the two layers into one.
     *
     * It is implemented as a nullary adaptor factory (consistent with fxt::value,
     * fxt::to_optional, etc.): calling fxt::join() yields the pipe adaptor, which
     * forwards to the container's and_then() member function with an identity
     * callable so the inner monad is returned without re-wrapping.
     *
     * @tparam TContainer The (deduced) doubly-wrapped expected type
     * @param container The nested expected to flatten
     * @return The flattened expected<T, E>
     *
     * @section Concepts
     * The callable is constrained via a requires clause so it only participates in
     * overload resolution when its argument is a doubly-wrapped expected:
     * - The argument itself must satisfy expected_like
     * - The argument's value_type must also satisfy expected_like
     *
     * Calling join on anything other than expected<expected<T, E>, E> produces a
     * clean compile error rather than a confusing template instantiation failure.
     *
     * @section Usage
     * @code
     * // Flatten the result of a curry | with() pipeline whose final callable
     * // itself returns fxt::expected<R, E>:
     * auto result = fxt::curry(compute)
     *             | fxt::with(validate_x(raw_x))   // expected<double, E>
     *             | fxt::with(validate_y(raw_y))   // expected<double, E>
     *             | fxt::join();                   // expected<expected<R, E>, E> -> expected<R, E>
     *
     * // Function call syntax
     * auto flat = fxt::join()(nested);   // nested : expected<expected<T, E>, E>
     * @endcode
     */
    // TODO: Currently restricted to nested expected<expected<T, E>, E> with a single
    //       shared error type E. Two possible generalizations:
    //         1. Accept any monad family by gating on monad_like (IsMonad.hpp) instead
    //            of expected_like, so optional<optional<T>> also flattens.
    //         2. Promote the same-error-type requirement into the requires clause
    //            (same_as<outer::error_type, value_type::error_type>) so a mismatch
    //            fails at the call site with a clear message rather than deep inside
    //            and_then().
    inline constexpr auto join = []() {
        return []<typename TContainer>(TContainer&& container)
            requires expected_like<std::remove_cvref_t<TContainer>> &&
                     expected_like<typename std::remove_cvref_t<TContainer>::value_type>
        {
            return std::forward<TContainer>(container).and_then(
                [](auto&& inner) -> decltype(auto) {
                    return std::forward<decltype(inner)>(inner);
                });
        };
    };
}    // namespace fxt
