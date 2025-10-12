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