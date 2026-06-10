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

    /**
     * @brief Monadic applicative adaptor returned by fxt::mwith().
     *
     * Structurally identical to with_adaptor, but uses .and_then() instead of
     * .transform() in both pipeline branches. This handles the case where the
     * curried callable itself returns an expected-like type rather than a plain
     * value, flattening the result instead of producing a nested
     * expected<expected<R, E>, E>.
     *
     * Not intended for direct construction — use fxt::mwith() instead.
     *
     * @tparam T The value type of the wrapped argument
     * @tparam E The shared error type for the entire pipeline
     */
    template<typename T, typename E>
    class mwith_adaptor
    {
        fxt::expected<T, E> arg_;

    public:
        explicit mwith_adaptor(fxt::expected<T, E> ex) : arg_(std::move(ex)) {}

        /**
         * @brief Invoke the adaptor with the left-hand side of a pipe expression.
         *
         * Two cases are handled via if constexpr:
         *
         * **Case 2** — Left is expected_like (expected<Func, E>): unwrap both sides via
         * .and_then(), invoke Func on the argument. Because Func returns an expected,
         * .and_then() flattens the result instead of nesting it. Propagates whichever
         * error appears first.
         *
         * **Case 1** — Left is a plain callable returning expected<R, E>: apply it directly
         * to the stored argument via .and_then(), flattening the monadic result. This is
         * the pipeline entry point.
         *
         * @tparam Left Type of the left-hand side (expected<Func,E> or plain callable)
         * @param left  The left-hand side value
         * @return expected<R, E> where R is the value type produced by the callable
         */
        template<typename Left>
        auto operator()(Left&& left) const
        {
            if constexpr (expected_like<std::remove_cvref_t<Left>>) {
                static_assert(std::same_as<typename std::remove_cvref_t<Left>::error_type, E>,
                              "fxt::mwith(): all expected values in the pipeline must share the same error type E");
                return std::forward<Left>(left).and_then([this](auto&& f) {
                    return arg_.and_then([&f](auto&& t) {
                        return f(std::forward<decltype(t)>(t));
                    });
                });
            }
            else {
                using result_type = std::invoke_result_t<Left, T>;
                static_assert(expected_like<std::remove_cvref_t<result_type>>,
                              "fxt::mwith(): the callable must return an expected-like type — use fxt::with() for plain-value callables");
                static_assert(std::same_as<typename std::remove_cvref_t<result_type>::error_type, E>,
                              "fxt::mwith(): the callable's expected error type must match the pipeline error type E");
                return arg_.and_then([&left](auto&& t) {
                    return left(std::forward<decltype(t)>(t));
                });
            }
        }
    };

    /**
     * @brief Monadic applicative mwith() — wraps an argument for a curried pipeline
     *        whose callable returns an expected.
     *
     * Returns a pipe adaptor that feeds the given expected-wrapped argument into the
     * function on the left-hand side of |. Unlike fxt::with(), the callable is expected
     * to return an expected-like type; mwith() flattens that result via .and_then()
     * rather than wrapping it, avoiding a nested expected<expected<R, E>, E>.
     *
     * All expected values in the chain must share the same error type E.
     *
     * @code
     * // A curried function whose callable returns fxt::expected<R, E>:
     * auto divide = fxt::curry([](double a, double b) -> fxt::expected<double, XLError> {
     *     if (b == 0.0) return fxt::unexpected<XLError>(XLError::DivByZero);
     *     return a / b;
     * });
     *
     * // Use fxt::with() to feed intermediate arguments (each step yields a partial
     * // application wrapped in expected), then fxt::mwith() for the final step where
     * // the callable returns expected<R, E> — mwith() flattens that result:
     * auto result = divide
     *             | fxt::with(validate_numerator(raw_n))      // expected<partial, XLError>
     *             | fxt::mwith(validate_denominator(raw_d));  // expected<double,  XLError>
     * // result : expected<double, XLError>  (flattened, not nested)
     *
     * // For a single-argument callable returning expected<R, E>, mwith() is the entry point:
     * auto reciprocal = fxt::curry([](double x) -> fxt::expected<double, XLError> {
     *     if (x == 0.0) return fxt::unexpected<XLError>(XLError::DivByZero);
     *     return 1.0 / x;
     * });
     * auto r = reciprocal | fxt::mwith(validate(raw_x));      // expected<double, XLError>
     * @endcode
     *
     * @tparam T Value type of the argument
     * @tparam E Shared error type for the pipeline
     * @param ex The expected-wrapped argument to feed into the pipeline
     * @return A mwith_adaptor<T,E> pipe adaptor
     */
    template<typename T, typename E>
    auto mwith(fxt::expected<T, E> ex) -> mwith_adaptor<T, E>
    {
        return mwith_adaptor<T, E>{std::move(ex)};
    }

    /**
     * @brief Pipe operator for the pipeline entry point: plain callable | mwith(ex).
     *
     * Handles Case 1 — the left-hand side is a plain callable (e.g., the result of
     * fxt::curry()) rather than an expected. Applies it to the argument via .and_then(),
     * flattening the callable's expected<R, E> result.
     *
     * Case 2 (expected<Func,E> | mwith(ex)) is handled by the pipe operator in
     * Expected.hpp, which dispatches to mwith_adaptor::operator().
     *
     * @requires F must not satisfy expected_like
     */
    template<typename F, typename T, typename E>
        requires (!expected_like<std::remove_cvref_t<F>>)
    auto operator|(F&& f, mwith_adaptor<T, E> adaptor)
    {
        return adaptor(std::forward<F>(f));
    }

}    // namespace fxt
