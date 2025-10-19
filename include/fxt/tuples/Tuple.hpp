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
 * @file Tuple.hpp
 * @brief Core tuple type and operations for the FXT library
 *
 * This file provides the foundational tuple type and operations for the FXT library.
 * It serves as a thin wrapper around std::tuple with additional FXT-specific functionality,
 * enabling functional-style operations and pipeline composition.
 *
 * ## Main Components
 *
 * **fxt::tuple<Ts...>** - Alias for std::tuple providing consistent FXT namespace interface
 * - Allows writing `fxt::tuple` instead of `std::tuple`
 * - Compatible with all standard library tuple operations
 *
 * **fxt::make_tuple(args...)** - Factory function to create tuples with type deduction
 * - Forwards to std::make_tuple with perfect forwarding
 * - Provides consistent FXT interface
 *
 * ## Pipe Operators
 *
 * Defines pipe operators (`operator|`) enabling functional-style composition. Supports all
 * value categories for maximum flexibility:
 * - `tuple& | callable` - Lvalue reference
 * - `const tuple& | callable` - Const lvalue reference
 * - `tuple&& | callable` - Rvalue reference (move semantics)
 * - `const tuple&& | callable` - Const rvalue reference
 *
 * ## Key Features
 * - Consistent namespace interface across FXT library
 * - Perfect forwarding support for all operations
 * - Pipeline-friendly design with pipe operator support
 * - Universal value category support (lvalue, rvalue, const)
 * - Foundation for higher-level tuple operations
 *
 * ## Examples
 *
 * @code
 * // Creating tuples
 * fxt::tuple<int, double, std::string> t1{42, 3.14, "hello"};
 * auto t2 = fxt::make_tuple(42, 3.14, "hello");
 *
 * // Using pipe operator
 * auto t = fxt::make_tuple(1, 2, 3);
 * auto result = t | fxt::tuple_reverse();
 * // result is fxt::tuple<int, int, int>{3, 2, 1}
 *
 * // Chaining operations
 * auto result2 = t
 *     | fxt::tuple_transform([](auto x) { return x * 2; })
 *     | fxt::take<2>();
 * // result2 is fxt::tuple<int, int>{2, 4}
 *
 * // Pipeline with rvalue
 * auto result3 = fxt::make_tuple(1, 2, 3, 4, 5)
 *     | fxt::drop<2>()
 *     | fxt::tuple_reverse()
 *     | fxt::take<2>();
 * // result3 is fxt::tuple<int, int>{5, 4}
 * @endcode
 *
 * @see fxt::flat_tuple
 * @see fxt::tuple_transform
 * @see fxt::tuple_reverse
 */

#pragma once

#include <concepts>
#include <functional>
#include <tuple>

namespace fxt
{
    /**
     * @brief Alias for std::tuple
     *
     * Provides a consistent fxt namespace interface for tuple types,
     * allowing users to write fxt::tuple instead of std::tuple.
     */
    template<typename... Ts>
    using tuple = std::tuple<Ts...>;

    /**
     * @brief Create a tuple, deducing the target type from the types of arguments
     *
     * Forwards to std::make_tuple. Provides a consistent fxt::make_tuple
     * interface that matches fxt::tuple.
     *
     * @tparam Ts Types of the elements
     * @param args Values to initialize the tuple with
     * @return A tuple containing the given values
     *
     * @code
     * auto t = fxt::make_tuple(42, 3.14, "hello");
     * // Creates fxt::tuple<int, double, const char*>
     * @endcode
     */
    template<typename... Ts>
    constexpr auto make_tuple(Ts&&... args)
    {
        return std::make_tuple(std::forward<Ts>(args)...);
    }

}    // namespace fxt

