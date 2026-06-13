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
     * @brief Monadic or_else operation for types that provide an or_else() member function
     *
     * This is a generic function object that works with any type that has an or_else() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided function to
     * the container's or_else() member function, enabling monadic error handling composition.
     *
     * @tparam TFunction The type of the function to apply via or_else()
     * @param f The function to forward to the container's or_else() member function
     * @return A callable that accepts any container with an or_else() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named or_else() that accepts the provided function
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = divide(10, 0) | fxt::or_else(handle_error);
     * auto result2 = parse_int("abc") | fxt::or_else(return_default);
     *
     * // Function call syntax
     * auto result3 = fxt::or_else(handle_error)(divide(10, 0));
     *
     * // Chaining operations
     * auto result4 = parse_int("abc")
     *              | fxt::or_else(try_parse_hex)
     *              | fxt::or_else(return_zero);
     * @endcode
     */
    inline constexpr auto or_else = []<typename TFunction>(TFunction&& f) {
    return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
        requires requires(TContainer&& c, TFunction fn) { c.or_else(fn); }
    { return std::forward<TContainer>(container).or_else(f); };
};

}    // namespace fxt