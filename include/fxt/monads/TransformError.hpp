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
     * @brief Monadic transform_error operation for types that provide a transform_error() member function
     *
     * This is a generic function object that works with any type that has a transform_error() member
     * function, such as fxt::expected. It forwards the provided function to the container's
     * transform_error() member function, enabling monadic error transformation.
     *
     * @tparam TFunction The type of the function to apply via transform_error()
     * @param f The function to forward to the container's transform_error() member function
     * @return A callable that accepts any container with a transform_error() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named transform_error() that accepts the provided function
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = divide(10, 0) | fxt::transform_error(to_string);
     * auto result2 = parse_int("abc") | fxt::transform_error(wrap_error);
     *
     * // Function call syntax
     * auto result3 = fxt::transform_error(to_string)(divide(10, 0));
     *
     * // Chaining operations
     * auto result4 = parse_int("abc")
     *              | fxt::transform_error(add_context)
     *              | fxt::transform_error(log_error)
     *              | fxt::transform_error(to_user_message);
     * @endcode
     */
    inline constexpr auto transform_error = []<typename TFunction>(TFunction&& f) {
    return [f = std::forward<TFunction>(f)]<typename TContainer, typename TFn = TFunction>(TContainer&& container)
        requires requires(TContainer&& c, TFn fn) { c.transform_error(fn); }
    { return std::forward<TContainer>(container).transform_error(f); };
};
}    // namespace fxt