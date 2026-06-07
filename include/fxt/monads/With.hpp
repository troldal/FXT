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
 * @file With.hpp
 * @brief Applicative with() adaptor for fxt::expected
 *
 * Provides the applicative pattern for fxt::expected, enabling expected-wrapped
 * arguments to be fed one at a time into a curried function via the pipe operator.
 *
 * ## Overview
 *
 * The with() adaptor implements the Apply operation from applicative functors.
 * It allows a multi-argument function to be applied to validated (expected-wrapped)
 * arguments while automatically propagating the first error encountered.
 *
 * ## Key Features
 *
 * - **Short-circuit semantics**: identical to and_then — the first error stops the chain
 * - **Error type safety**: all expected values in the chain must share the same error type E
 * - **Curry integration**: designed to compose with fxt::curry for the primary use case
 *
 * ## Usage Example
 *
 * @code{.cpp}
 * #include <fxt/monads/With.hpp>
 * #include <fxt/utils/Curry.hpp>
 *
 * auto compute = [](double p, double t, double q) { return p * t * q; };
 *
 * auto result = fxt::curry(compute)
 *             | fxt::with(validate_pressure(raw_p))     // expected<double, XLError>
 *             | fxt::with(validate_temperature(raw_t))  // expected<double, XLError>
 *             | fxt::with(validate_quality(raw_q));     // expected<double, XLError>
 * // result : expected<double, XLError>
 * @endcode
 */

#pragma once

#include <type_traits>
#include <utility>
#include "../concepts/IsExpected.hpp"
#include "Expected.hpp"

namespace fxt
{
    /**
     * @brief Applicative adaptor returned by fxt::with().
     *
     * Holds one expected-wrapped argument and, when invoked via the pipe operator,
     * applies it to either a plain callable (pipeline entry) or an
     * expected-wrapped partial application (subsequent steps).
     *
     * Not intended for direct construction — use fxt::with() instead.
     *
     * @tparam T The value type of the wrapped argument
     * @tparam E The shared error type for the entire pipeline
     */
    template<typename T, typename E>
    class with_adaptor
    {
        fxt::expected<T, E> arg_;

    public:
        explicit with_adaptor(fxt::expected<T, E> ex) : arg_(std::move(ex)) {}

        /**
         * @brief Invoke the adaptor with the left-hand side of a pipe expression.
         *
         * Two cases are handled via if constexpr:
         *
         * **Case 2** — Left is expected_like (expected<Func, E>): unwrap both sides, invoke
         * Func on the argument, wrap the result. Propagates whichever error appears first.
         *
         * **Case 1** — Left is a plain callable: apply it directly to the stored argument,
         * wrapping the result in expected<R, E>. This is the pipeline entry point.
         *
         * @tparam Left Type of the left-hand side (expected<Func,E> or plain callable)
         * @param left  The left-hand side value
         * @return expected<invoke_result_t<Func, T>, E>
         */
        template<typename Left>
        auto operator()(Left&& left) const
        {
            if constexpr (expected_like<std::remove_cvref_t<Left>>) {
                static_assert(std::same_as<typename std::remove_cvref_t<Left>::error_type, E>,
                              "fxt::with(): all expected values in the pipeline must share the same error type E");
                return std::forward<Left>(left).and_then([this](auto&& f) {
                    return arg_.transform([&f](auto&& t) {
                        return f(std::forward<decltype(t)>(t));
                    });
                });
            }
            else {
                return arg_.transform([&left](auto&& t) {
                    return left(std::forward<decltype(t)>(t));
                });
            }
        }
    };

    /**
     * @brief Applicative with() — wraps an argument for use in a curried expected pipeline.
     *
     * Returns a pipe adaptor that feeds the given expected-wrapped argument into the
     * function on the left-hand side of |, applying short-circuit error propagation.
     *
     * All expected values in the chain must share the same error type E.
     *
     * @code
     * auto result = fxt::curry(f)
     *             | fxt::with(validate_x(raw_x))
     *             | fxt::with(validate_y(raw_y));
     * @endcode
     *
     * @tparam T Value type of the argument
     * @tparam E Shared error type for the pipeline
     * @param ex The expected-wrapped argument to feed into the pipeline
     * @return A with_adaptor<T,E> pipe adaptor
     */
    template<typename T, typename E>
    auto with(fxt::expected<T, E> ex) -> with_adaptor<T, E>
    {
        return with_adaptor<T, E>{std::move(ex)};
    }

    /**
     * @brief Pipe operator for the pipeline entry point: plain callable | with(ex).
     *
     * Handles Case 1 — the left-hand side is a plain callable (e.g., the result of
     * fxt::curry()) rather than an expected. Lifts the callable implicitly and applies
     * it to the argument, returning expected<invoke_result_t<F,T>, E>.
     *
     * Case 2 (expected<Func,E> | with(ex)) is handled by the pipe operator in
     * Expected.hpp, which dispatches to with_adaptor::operator().
     *
     * @requires F must not satisfy expected_like
     */
    template<typename F, typename T, typename E>
        requires (!expected_like<std::remove_cvref_t<F>>)
    auto operator|(F&& f, with_adaptor<T, E> adaptor)
    {
        return adaptor(std::forward<F>(f));
    }

}    // namespace fxt
