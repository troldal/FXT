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
 * @file Attempt.hpp
 * @brief Exception-safe function invocation utility
 *
 * This file provides the `attempt` function template, which wraps function invocation
 * in exception handling, converting exceptions into `fxt::expected` return values.
 * This enables functional-style error handling by transforming exceptions (side-channel
 * errors) into values (in-band errors) that can be composed with other monadic operations.
 *
 * Key features:
 * - Catches all exceptions and returns them as `fxt::failure` in an `fxt::expected`
 * - Handles both void and non-void return types seamlessly
 * - Uses `fxt::unit` as a placeholder for void returns
 * - Provides perfect forwarding of function and arguments
 * - Marked `noexcept` since all exceptions are caught internally
 * - Constrained with C++20 concepts for better error messages
 *
 * @example
 * @code
 * auto result = fxt::attempt([]{ return 42 / 0; });
 * if (result) {
 *     std::cout << "Success: " << *result << std::endl;
 * } else {
 *     std::cout << "Error: " << result.error().message() << std::endl;
 * }
 * @endcode
 *
 * @see fxt::expected
 * @see fxt::failure
 * @see fxt::unit
 */

#pragma once
#include "Failure.hpp"
#include "Unit.hpp"

namespace fxt
{
    /**
     * @brief Attempts to invoke a function and captures any exceptions as failure values.
     *
     * This function template provides exception-safe invocation by wrapping the function call
     * in a try-catch block. If the function executes successfully, its return value (or `fxt::unit`
     * for void functions) is wrapped in an `fxt::expected`. If an exception is thrown, it is caught
     * and returned as an `fxt::unexpected<fxt::failure>`.
     *
     * This design pattern enables:
     * - Functional error handling without try-catch blocks at call sites
     * - Composition with other monadic operations (and_then, or_else, transform)
     * - Converting exceptions into values for more predictable control flow
     * - Type-safe handling of both success and failure cases
     *
     * @tparam Fn The type of the callable object (function, lambda, functor)
     * @tparam Args The types of the arguments to be passed to the callable
     *
     * @param fn The callable to be invoked
     * @param args The arguments to be forwarded to the callable
     *
     * @return An `fxt::expected` containing:
     *         - On success: The return value of `fn` (or `fxt::unit` if void)
     *         - On failure: An `fxt::failure` containing the exception information
     *
     * @note This function is marked `noexcept` because all exceptions are caught internally
     *       and converted to failure values. It will never propagate exceptions.
     *
     * @note For void-returning functions, the success value is `fxt::unit{}`, which serves
     *       as a unit type similar to `std::monostate` but semantically clearer.
     *
     * @example
     * @code
     * // Non-void function that may throw
     * auto divide = [](int a, int b) -> int {
     *     if (b == 0) throw std::runtime_error("Division by zero");
     *     return a / b;
     * };
     *
     * auto result = fxt::attempt(divide, 10, 2);
     * if (result) {
     *     std::cout << "Result: " << *result << std::endl;  // Output: Result: 5
     * }
     *
     * auto error = fxt::attempt(divide, 10, 0);
     * if (!error) {
     *     std::cout << error.error().message() << std::endl;  // Output: Division by zero
     * }
     *
     * // Void function
     * auto print = [](const std::string& msg) {
     *     if (msg.empty()) throw std::invalid_argument("Empty message");
     *     std::cout << msg << std::endl;
     * };
     *
     * auto void_result = fxt::attempt(print, "Hello");
     * if (void_result) {
     *     // Successfully printed, void_result contains fxt::unit
     * }
     * @endcode
     *
     * @see fxt::expected
     * @see fxt::failure
     * @see fxt::unit
     * @see std::invoke
     */
    // TODO: SAFETY — attempt is declared noexcept, but the catch handler constructs a
    //       fxt::failure (which allocates a std::string for the message) and the success
    //       path copies/moves Ret into the expected; if either throws (e.g. bad_alloc, or a
    //       throwing move of Ret outside the try block is fine, but inside the catch it is
    //       not caught), std::terminate is called. Either drop noexcept or make the catch
    //       path non-allocating.
    template<typename Fn, typename... Args>
    requires std::invocable<Fn, Args...>
    auto attempt(Fn &&fn, Args &&...args) noexcept
            -> fxt::expected<std::conditional_t<std::is_void_v<std::invoke_result_t<Fn, Args...>>, fxt::unit,
                                                std::invoke_result_t<Fn, Args...>>,
                             fxt::failure> {
        using Ret = std::invoke_result_t<Fn, Args...>;
        using ValueType = std::conditional_t<std::is_void_v<Ret>, fxt::unit, Ret>;
        using ExpectedType = fxt::expected<ValueType, fxt::failure>;

        try {
            if constexpr (std::is_void_v<Ret>) {
                // Invoke the function purely for its side effects
                std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...);
                // Return an expected<unit, failure>
                return ExpectedType{fxt::unit{}};
            } else {
                // Return an expected<Ret, failure>
                return ExpectedType{std::invoke(std::forward<Fn>(fn), std::forward<Args>(args)...)};
            }
        } catch (...) {
            return fxt::unexpected<fxt::failure>(std::current_exception());
        }
    }
}