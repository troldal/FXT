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
#include "../concepts/IsExpected.hpp"
#include <concepts>
#include <functional>

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
    /**
     * @brief Pipe-adaptor form of fxt::attempt for use mid-pipeline.
     *
     * Returns a callable that, when applied to an
     * @c expected<T, fxt::failure> value via @c |, invokes @p fn with the
     * contained value inside a try-catch block:
     *
     * - If the input already holds an error, it is propagated unchanged and
     *   @p fn is never called.
     * - If @p fn returns successfully, the result is wrapped in
     *   @c expected<result_type, fxt::failure>.  Void-returning functions
     *   produce @c fxt::unit as the value.
     * - If @p fn throws, the exception is caught and returned as
     *   @c unexpected<fxt::failure>, exactly as the free-function form does.
     *
     * This overload is selected only when @p fn is **not** callable with zero
     * arguments, which eliminates ambiguity with the immediate-invocation form
     * @c attempt(fn) for zero-argument callables.
     *
     * @note Unlike the free-function form, the returned closure is not
     *       @c noexcept: propagating an existing @c fxt::failure through the
     *       pipeline may copy the failure object, which can allocate.
     *
     * @tparam Fn Callable type accepting exactly the value type of the piped expected
     * @param fn  The potentially-throwing function to wrap mid-pipeline
     * @return    A pipe-adaptor closure compatible with @c operator|
     *
     * @code
     * // Instead of the verbose form:
     * fxt::attempt(parse, input)
     *     | fxt::and_then([](auto x) { return fxt::attempt(validate, x); });
     *
     * // Write:
     * fxt::attempt(parse, input)
     *     | fxt::attempt(validate);
     * @endcode
     */
    template<typename Fn>
        requires (!std::invocable<std::decay_t<Fn>>)
    auto attempt(Fn&& fn)
    {
        return [fn = std::forward<Fn>(fn)]<typename TContainer>(TContainer&& container)
            requires expected_like<std::remove_cvref_t<TContainer>>
                  && std::same_as<typename std::remove_cvref_t<TContainer>::error_type, fxt::failure>
                  && std::invocable<std::decay_t<Fn>,
                                    typename std::remove_cvref_t<TContainer>::value_type>
        {
            using TValue     = typename std::remove_cvref_t<TContainer>::value_type;
            using Ret        = std::invoke_result_t<std::decay_t<Fn>, TValue>;
            using ValueType  = std::conditional_t<std::is_void_v<Ret>, fxt::unit, Ret>;
            using ResultType = fxt::expected<ValueType, fxt::failure>;

            if (!container.has_value())
                return ResultType{ fxt::unexpected{ std::forward<TContainer>(container).error() } };

            try {
                if constexpr (std::is_void_v<Ret>) {
                    std::invoke(fn, std::forward<TContainer>(container).value());
                    return ResultType{ fxt::unit{} };
                } else {
                    return ResultType{ std::invoke(fn, std::forward<TContainer>(container).value()) };
                }
            } catch (...) {
                return ResultType{ fxt::unexpected<fxt::failure>(std::current_exception()) };
            }
        };
    }

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