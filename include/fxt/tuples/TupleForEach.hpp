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
 * @brief Iteration operations for tuple elements
 *
 * This file provides the tuple_foreach function for applying side-effects to each element
 * of a tuple. Unlike tuple_transform which returns a new tuple, tuple_foreach is used when
 * you want to perform operations for their side-effects (e.g., printing, logging, mutation).
 *
 * ## Main Function
 *
 * **tuple_foreach(tuple, callable)** - Applies a callable to each tuple element for side-effects
 * - Iterates through all elements in order from first to last
 * - Does not return a value (void return type)
 * - Perfect forwarding preserves value categories
 * - Works with both fxt::tuple and fxt::flat_tuple
 *
 * ## Use Cases
 * - Printing tuple elements
 * - Logging values
 * - Mutating external state
 * - Accumulating results into containers
 * - Performing I/O operations on each element
 *
 * @see fxt::tuple_transform
 */

#pragma once

#include <utility>
#include <functional>

namespace fxt
{
    /**
     * @brief Apply a callable to each element of a tuple for side-effects
     *
     * Iterates through each element in the tuple and applies the given callable to it.
     * This function is designed for performing side-effects and does not return a value.
     * Elements are processed in order from first to last. Perfect forwarding is used
     * to preserve the value category of the tuple and its elements.
     *
     * Unlike tuple_transform, which creates a new tuple with transformed values,
     * tuple_foreach is used when you want to perform operations for their side-effects
     * such as printing, logging, or modifying external state.
     *
     * @tparam TupleT The tuple type (deduced, can be fxt::tuple or fxt::flat_tuple)
     * @tparam Fn The callable type (deduced)
     * @param tp The input tuple (lvalue or rvalue reference)
     * @param fn The callable to apply to each element
     *
     * @note This function returns void. Use tuple_transform if you need to create
     *       a new tuple with transformed values.
     *
     * @section Usage
     * @code
     * // Print each element
     * auto t = fxt::make_tuple(1, 2.5, "hello");
     * fxt::tuple_foreach(t, [](const auto& x) {
     *     std::cout << x << std::endl;
     * });
     * // Output:
     * // 1
     * // 2.5
     * // hello
     *
     * // Accumulate into a vector
     * std::vector<int> vec;
     * auto t2 = fxt::make_tuple(1, 2, 3, 4, 5);
     * fxt::tuple_foreach(t2, [&vec](int x) {
     *     vec.push_back(x * 2);
     * });
     * // vec now contains: {2, 4, 6, 8, 10}
     *
     * // Modify tuple elements (requires lvalue reference)
     * auto t3 = fxt::make_tuple(1, 2, 3);
     * fxt::tuple_foreach(t3, [](auto& x) {
     *     x *= 2;
     * });
     * // t3 now contains: {2, 4, 6}
     *
     * // Use with side-effect operations
     * int sum = 0;
     * auto t4 = fxt::make_tuple(10, 20, 30);
     * fxt::tuple_foreach(t4, [&sum](int x) {
     *     sum += x;
     * });
     * // sum is now 60
     * @endcode
     */
    template<typename TupleT, typename Fn>
    void tuple_foreach(TupleT&& tp, Fn&& fn)
    {
        std::apply([&fn]<typename... T>(T&&... args) { (fn(std::forward<T>(args)), ...); }, std::forward<TupleT>(tp));
    }

    /**
     * @brief Apply a callable to each element of a tuple for side-effects (curried version for pipeline)
     *
     * Returns a lambda that applies the given callable to each element of a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * The returned lambda performs side-effects and does not return a value.
     *
     * @tparam Fn The callable type (deduced)
     * @param fn The callable to apply to each element
     * @return A lambda that takes a tuple and applies the callable to each element
     *
     * @section Usage
     * @code
     * // Pipeline usage for printing
     * auto t = fxt::make_tuple(1, 2, 3);
     * t | fxt::tuple_foreach([](const auto& x) {
     *     std::cout << x << " ";
     * });
     * // Output: 1 2 3
     *
     * // Pipeline with state capture
     * int sum = 0;
     * auto t2 = fxt::make_tuple(10, 20, 30);
     * t2 | fxt::tuple_foreach([&sum](int x) {
     *     sum += x;
     * });
     * // sum is now 60
     *
     * // Chaining with other operations
     * auto t3 = fxt::make_tuple(1, 2, 3, 4, 5)
     *     | fxt::drop<2>()
     *     | fxt::tuple_transform([](auto x) { return x * 2; });
     *
     * t3 | fxt::tuple_foreach([](auto x) {
     *     std::cout << x << std::endl;
     * });
     * // Output:
     * // 6
     * // 8
     * // 10
     * @endcode
     */
    template<typename Fn>
    constexpr auto tuple_foreach(Fn&& fn)
    {
        return [fn = std::forward<Fn>(fn)]<typename TupleT>(TupleT&& tp) {
            fxt::tuple_foreach(std::forward<TupleT>(tp), fn);
        };
    }

}    // namespace fxt