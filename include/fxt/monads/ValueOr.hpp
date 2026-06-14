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
     * @brief Monadic value_or operation for types that provide a value_or() member function
     *
     * This is a generic function object that works with any type that has a value_or() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided alternative value
     * to the container's value_or() member function, enabling a functional approach to providing
     * default values.
     *
     * @tparam TAlternative The type of the alternative value
     * @param val The alternative value to use if the container is empty or contains an error
     * @return A callable that accepts any container with a value_or() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named value_or() that accepts the provided alternative value
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage with expected
     * auto result1 = divide(10, 0) | fxt::value_or(42);
     *
     * // Pipe operator usage with optional
     * auto result2 = parse_int("abc") | fxt::value_or(0);
     *
     * // Function call syntax
     * auto result3 = fxt::value_or(100)(maybe_value);
     *
     * // Chaining operations
     * auto result4 = parse_int("123")
     *              | fxt::transform([](int x) { return x * 2; })
     *              | fxt::value_or(0);
     * @endcode
     */
    inline constexpr auto value_or = []<typename TFunction>(TFunction&& f) {
    return [f = std::forward<TFunction>(f)]<typename TContainer, typename TFn = TFunction>(TContainer&& container)
        requires requires(TContainer&& c, TFn fn) { c.value_or(fn); }
    { return std::forward<TContainer>(container).value_or(f); };
};

}    // namespace fxt