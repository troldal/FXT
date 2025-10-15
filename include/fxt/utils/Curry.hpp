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
 * @file Curry.hpp
 * @brief Automatic function currying utilities for C++23
 *
 * This file provides utilities for automatic currying of functions, enabling
 * partial application and functional composition patterns. Currying transforms
 * a multi-argument function into a chain of single-argument functions, allowing
 * arguments to be supplied incrementally.
 *
 * ## Overview
 *
 * The primary facility is the `curry` function, which automatically transforms
 * any callable into a curried version. When called with fewer arguments than
 * required, it returns a new function awaiting the remaining arguments. When
 * all arguments are provided, the original function executes.
 *
 * ## Key Features
 *
 * - **Automatic Currying**: Transforms any function into a curried version
 * - **Partial Application**: Supply arguments one at a time or in groups
 * - **Perfect Forwarding**: Preserves value categories and const-correctness
 * - **Compile-Time Detection**: Uses C++20 concepts to determine when to execute
 * - **Zero Runtime Overhead**: All decisions made at compile time
 *
 * ## Functions
 *
 * - **curry**: Transforms a function into its curried equivalent, enabling
 *   partial application. Implementation credit: Björn Fahller (@rollbear on GitHub)
 *
 * ## Usage Example
 *
 * @code{.cpp}
 * #include <fxt/utils/Curry.hpp>
 *
 * // Define a multi-argument function
 * auto add3 = [](int a, int b, int c) { return a + b + c; };
 *
 * // Curry the function
 * auto curriedAdd = fxt::curry(add3);
 *
 * // Partial application
 * auto add10AndMore = curriedAdd(10);
 * auto add10And20AndMore = add10AndMore(20);
 * int result = add10And20AndMore(30); // 60
 *
 * // Or chain directly
 * int result2 = fxt::curry(add3)(5)(10)(15); // 30
 * @endcode
 *
 * @note This implementation uses C++23 features including deducing `this`,
 *       which allows the lambda to reference itself recursively.
 */

#pragma once

#include <functional>
#include <utility>

namespace fxt
{
    /**
     * @brief A recursive lambda that enables automatic currying of functions
     *
     * This lambda transforms any function into a curried version that can be
     * partially applied. When all required arguments are supplied, the function
     * executes. Otherwise, it returns a new function that accepts the remaining
     * arguments.
     *
     * @tparam F Type of the function to curry
     * @param curry Self-reference for recursive calls
     * @param f The function to curry
     * @return A curried version of the function
     *
     * The implementation uses deducing `this` and SFINAE via `requires` to determine
     * whether the function can be called with the provided arguments. If not enough
     * arguments are provided, it returns a new function using std::bind_front.
     *
     * @example
     *   auto add = [](int a, int b) { return a + b; };
     *   auto curriedAdd = fxt::curry(add);
     *   auto add5 = curriedAdd(5);
     *   int result = add5(10); // 15
     */
    inline auto curry = []<typename F>(this auto& curry, F&& f) {
        return [curry, f = std::forward<F>(f)]<typename Self, typename... Ts>(this Self&&, Ts&&... ts) -> decltype(auto) {
            if constexpr (requires { std::forward_like<Self>(f)(std::forward<Ts>(ts)...); }) {
                return std::forward_like<Self>(f)(std::forward<Ts>(ts)...);
            }
            else {
                return curry(std::bind_front(std::forward_like<Self>(f), std::forward<Ts>(ts)...));
            }
        };
    };

}    // namespace fxt