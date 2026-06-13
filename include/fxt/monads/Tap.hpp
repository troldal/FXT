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
     * @brief Pass-through side-effect adaptor for monadic pipelines
     *
     * Executes a side-effect function on the contained value (if present) and
     * forwards the container unchanged. Suitable for logging, debugging, or
     * any observation that must not alter the pipeline value.
     *
     * Works with any type that exposes has_value() and value() — i.e. both
     * optional-like and expected-like containers. The side effect is skipped
     * when the container holds no value (empty optional or error state).
     *
     * @tparam TFunction The type of the side-effect callable
     * @param f The callable to invoke with the contained value
     * @return A pipe adaptor that returns the container unchanged
     *
     * @section Usage
     * @code
     * // Mid-pipeline logging
     * auto result = parse_int("42")
     *             | fxt::tap([](int x) { std::cout << "parsed: " << x << '\n'; })
     *             | fxt::transform([](int x) { return x * 2; });
     *
     * // Multiple taps for tracing
     * auto result = fxt::expected<int, std::string>{5}
     *             | fxt::tap([](int x) { log("before", x); })
     *             | fxt::transform([](int x) { return x * 3; })
     *             | fxt::tap([](int x) { log("after",  x); });
     * @endcode
     */
    inline constexpr auto tap = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires { container.has_value(); container.value(); }
        {
            if (container.has_value()) {
                f(container.value());
            }
            return std::forward<TContainer>(container);
        };
    };

    /**
     * @brief Pass-through side-effect adaptor for the error channel of expected-like types
     *
     * Executes a side-effect function on the contained error (if present) and
     * forwards the container unchanged. The callable is skipped when the container
     * holds a value (success state).
     *
     * @tparam TFunction The type of the side-effect callable
     * @param f The callable to invoke with the error value
     * @return A pipe adaptor that returns the container unchanged
     *
     * @section Usage
     * @code
     * auto result = some_operation()
     *             | fxt::tap_error([](const Error& e) { log_error(e); })
     *             | fxt::or_else([](const Error&) { return fallback(); });
     * @endcode
     */
    inline constexpr auto tap_error = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires { container.has_value(); container.error(); }
        {
            if (!container.has_value()) {
                f(container.error());
            }
            return std::forward<TContainer>(container);
        };
    };

    /**
     * @brief Pass-through side-effect adaptor for the empty state of optional-like types
     *
     * Executes a no-argument side-effect callable when the container holds no value,
     * then forwards the container unchanged. Complementary to fxt::tap.
     *
     * @tparam TFunction The type of the side-effect callable (takes no parameters)
     * @param f The callable to invoke when the container is empty
     * @return A pipe adaptor that returns the container unchanged
     *
     * @section Usage
     * @code
     * auto result = find_user(id)
     *             | fxt::tap     ([](const User& u) { log_found(u); })
     *             | fxt::tap_none([]                { log_missing(); });
     * @endcode
     */
    inline constexpr auto tap_none = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires { container.has_value(); }
        {
            if (!container.has_value()) {
                f();
            }
            return std::forward<TContainer>(container);
        };
    };

}    // namespace fxt
