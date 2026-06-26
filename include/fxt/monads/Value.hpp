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

namespace fxt
{
    /**
     * @brief Nullary adaptor factory for value extraction from monadic types.
     *
     * `fxt::value` is a factory: calling it with no arguments returns an adaptor
     * that extracts the contained value from any type that has a `.value()` member
     * function (such as fxt::expected and fxt::optional). The extra `()` is required
     * to obtain the adaptor — write `fxt::value()`, not `fxt::value`, in expressions.
     *
     * Note: `fxt::value` follows the same nullary-factory convention as `fxt::join()`,
     * `fxt::to_optional()`, `fxt::take<N>()`, and `fxt::tuple_drop<N>()`. These all differ
     * from adaptors such as `fxt::and_then(f)` and `fxt::transform(f)`, which accept
     * a function argument and immediately return the adaptor without a trailing `()`.
     *
     * @return A callable (the adaptor) that accepts any type with a `.value()` member
     *         function and returns the contained value.
     *
     * @section Concepts
     * The returned adaptor accepts any type that satisfies:
     * - Has a member function named value() that returns the contained value
     * - Supports both lvalue and rvalue references (rvalue containers move the value out)
     *
     * @section Usage
     * @code
     * // Pipe operator usage — note the required ()
     * auto result1 = divide(10, 2) | fxt::value();
     *
     * // Pipe operator usage with optional
     * auto result2 = parse_int("42") | fxt::value();
     *
     * // Direct call syntax — value() returns the adaptor, which is then called
     * auto result3 = fxt::value()(maybe_value);
     *
     * // Chaining operations
     * auto result4 = parse_int("123")
     *              | fxt::transform([](int x) { return x * 2; })
     *              | fxt::value();
     * @endcode
     *
     * @note Throws if called on an expected that holds an error, or an optional that
     *       is empty — the same preconditions as calling .value() directly.
     * @note Only works with types that expose a .value() member function.
     */
    inline constexpr auto value = []() {
        return []<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.value(); }
        {
            // If TContainer is not an lvalue reference type, it's an rvalue
            if constexpr (!std::is_lvalue_reference_v<TContainer>) {
                return std::move(container).value();
            } else {
                return container.value();
            }
        };
    };
}    // namespace fxt