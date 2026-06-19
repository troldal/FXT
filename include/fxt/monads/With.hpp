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
 * @brief Applicative with() and mwith() adaptors for fxt::expected and fxt::optional
 *
 * Provides the applicative pattern for monadic types, enabling monad-wrapped
 * arguments to be fed one at a time into a curried function via the pipe operator.
 *
 * ## Overview
 *
 * The with() and mwith() adaptors implement the Apply operation from applicative
 * functors. They allow a multi-argument function to be applied to validated
 * (monad-wrapped) arguments while automatically propagating the first error or
 * absent value encountered.
 *
 * Both adaptors accept any type satisfying `fxt::monad_like` — that is,
 * `fxt::expected<T, E>` or `fxt::optional<T>` (and compatible third-party types).
 * The factory functions use forwarding references constrained on `monad_like`,
 * so expected-like types from other libraries (e.g. tl::expected) also deduce
 * correctly without requiring an exact-type match.
 *
 * ## Key Features
 *
 * - **Monad-generic**: works with both fxt::expected and fxt::optional
 * - **Short-circuit semantics**: identical to and_then — the first error or nullopt
 *   stops the chain
 * - **Error type safety for expected**: all expected values in the chain must share
 *   the same error type E, enforced via static_assert
 * - **Curry integration**: designed to compose with fxt::curry for the primary use case
 *
 * ## Usage Examples
 *
 * @code{.cpp}
 * #include <fxt/monads/With.hpp>
 * #include <fxt/utils/Curry.hpp>
 *
 * auto compute = [](double p, double t, double q) { return p * t * q; };
 *
 * // expected pipeline
 * auto result = fxt::curry(compute)
 *             | fxt::with(validate_pressure(raw_p))     // expected<double, XLError>
 *             | fxt::with(validate_temperature(raw_t))  // expected<double, XLError>
 *             | fxt::with(validate_quality(raw_q));     // expected<double, XLError>
 * // result : expected<double, XLError>
 *
 * // optional pipeline
 * auto result2 = fxt::curry(compute)
 *              | fxt::with(maybe_pressure(raw_p))    // optional<double>
 *              | fxt::with(maybe_temperature(raw_t)) // optional<double>
 *              | fxt::with(maybe_quality(raw_q));    // optional<double>
 * // result2 : optional<double>
 * @endcode
 */

#pragma once

#include <type_traits>
#include <utility>
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsMonad.hpp"

namespace fxt
{
    /**
     * @brief Applicative adaptor returned by fxt::with().
     *
     * Holds one monad-wrapped argument and, when invoked via the pipe operator,
     * applies it to either a plain callable (pipeline entry) or a monad-wrapped
     * partial application (subsequent steps).
     *
     * Works with any monad_like argument type: fxt::expected<T,E>, fxt::optional<T>,
     * or any compatible third-party expected/optional type.
     *
     * Not intended for direct construction — use fxt::with() instead.
     *
     * @tparam Arg The monad type of the wrapped argument (e.g. expected<T,E> or optional<T>)
     */
    template<typename Arg>
        requires monad_like<Arg>
    class with_adaptor
    {
        Arg arg_;

    public:
        explicit with_adaptor(Arg arg) : arg_(std::move(arg)) {}

        /**
         * @brief Invoke the adaptor with the left-hand side of a pipe expression.
         *
         * Two cases are handled via if constexpr:
         *
         * **Case 2** — Left is monad_like (expected<Func, E> or optional<Func>): unwrap
         * both sides via .and_then() + .transform(), invoke Func on the argument, wrap
         * the result. Propagates whichever error or nullopt appears first.
         *
         * **Case 1** — Left is a plain callable: apply it directly to the stored argument
         * via .transform(), wrapping the result in the same monad. This is the pipeline
         * entry point.
         *
         * For expected pipelines, Case 2 additionally enforces that Left and Arg share
         * the same error type E and the same expected template (via static_assert).
         *
         * @tparam Left Type of the left-hand side (monad<Func> or plain callable)
         * @param left  The left-hand side value
         * @return monad<invoke_result_t<Func, T>>
         */
        template<typename Left>
        auto operator()(Left&& left) const
        {
            if constexpr (monad_like<std::remove_cvref_t<Left>>) {
                // Case 2: Left is a monad wrapping a callable (partial application).
                using LeftDecayed = std::remove_cvref_t<Left>;
                using ArgDecayed  = Arg;
                if constexpr (expected_like<LeftDecayed> && expected_like<ArgDecayed>) {
                    static_assert(std::same_as<typename LeftDecayed::error_type, typename ArgDecayed::error_type>,
                                  "fxt::with(): all expected values in the pipeline must share the same error type E");
                    static_assert(same_expected_kind<LeftDecayed, ArgDecayed>,
                                  "fxt::with(): all expected values in the pipeline must be the same expected type "
                                  "(e.g. all std::expected or all tl::expected)");
                }
                return std::forward<Left>(left).and_then([this](auto&& f) {
                    return arg_.transform([&f](auto&& t) {
                        return f(std::forward<decltype(t)>(t));
                    });
                });
            }
            else {
                // Case 1: Left is a plain callable — pipeline entry point.
                return arg_.transform([&left](auto&& t) {
                    return left(std::forward<decltype(t)>(t));
                });
            }
        }
    };

    /**
     * @brief Applicative with() — wraps a monad argument for use in a curried pipeline.
     *
     * Returns a pipe adaptor that feeds the given monad-wrapped argument into the
     * function on the left-hand side of |. Works with both fxt::expected and
     * fxt::optional, and accepts any compatible expected-like or optional-like type
     * via a forwarding reference constrained on monad_like.
     *
     * For expected pipelines, all expected values must share the same error type E.
     * For optional pipelines, any absent value short-circuits the chain.
     *
     * @code
     * // expected pipeline
     * auto result = fxt::curry(f)
     *             | fxt::with(validate_x(raw_x))   // expected<partial, E>
     *             | fxt::with(validate_y(raw_y));  // expected<result, E>
     *
     * // optional pipeline
     * auto result2 = fxt::curry(f)
     *              | fxt::with(maybe_x())   // optional<partial>
     *              | fxt::with(maybe_y());  // optional<result>
     * @endcode
     *
     * @tparam M Monad type of the argument (deduced — satisfies monad_like)
     * @param m  The monad-wrapped argument to feed into the pipeline
     * @return A with_adaptor<M> pipe adaptor
     */
    template<typename M>
        requires monad_like<std::remove_cvref_t<M>>
    auto with(M&& m) -> with_adaptor<std::remove_cvref_t<M>>
    {
        return with_adaptor<std::remove_cvref_t<M>>{std::forward<M>(m)};
    }

    /**
     * @brief Pipe operator for the pipeline entry point: plain callable | with(m).
     *
     * Handles Case 1 — the left-hand side is a plain callable (e.g., the result of
     * fxt::curry()) rather than a monad. Lifts the callable implicitly and applies
     * it to the argument, returning the same monad type.
     *
     * Case 2 (monad<Func> | with(m)) is handled by the monad-specific pipe operators
     * in Expected.hpp or Optional.hpp, which dispatch to with_adaptor::operator().
     *
     * @requires F must not satisfy monad_like
     */
    template<typename F, typename Arg>
        requires (!monad_like<std::remove_cvref_t<F>>)
    auto operator|(F&& f, with_adaptor<Arg> adaptor)
    {
        return adaptor(std::forward<F>(f));
    }

    /**
     * @brief Monadic applicative adaptor returned by fxt::mwith().
     *
     * Structurally identical to with_adaptor, but uses .and_then() instead of
     * .transform() in both pipeline branches. This handles the case where the
     * curried callable itself returns a monad rather than a plain value, flattening
     * the result instead of producing a nested monad<monad<R>>.
     *
     * Works with any monad_like argument type: fxt::expected<T,E>, fxt::optional<T>,
     * or any compatible third-party expected/optional type.
     *
     * Not intended for direct construction — use fxt::mwith() instead.
     *
     * @tparam Arg The monad type of the wrapped argument (e.g. expected<T,E> or optional<T>)
     */
    template<typename Arg>
        requires monad_like<Arg>
    class mwith_adaptor
    {
        Arg arg_;

    public:
        explicit mwith_adaptor(Arg arg) : arg_(std::move(arg)) {}

        /**
         * @brief Invoke the adaptor with the left-hand side of a pipe expression.
         *
         * Two cases are handled via if constexpr:
         *
         * **Case 2** — Left is monad_like (monad<Func>): unwrap both sides via
         * .and_then() twice, invoke Func on the argument. Because Func returns a monad,
         * .and_then() flattens the result instead of nesting it. Propagates whichever
         * absent/error value appears first.
         *
         * **Case 1** — Left is a plain callable returning a monad: apply it directly
         * to the stored argument via .and_then(), flattening the result. This is the
         * pipeline entry point.
         *
         * For expected pipelines, Case 2 additionally enforces that Left and Arg share
         * the same error type and expected template. Case 1 enforces that the callable's
         * return type is monad_like and (for expected results) matches the error type.
         *
         * @tparam Left Type of the left-hand side (monad<Func> or plain callable)
         * @param left  The left-hand side value
         * @return monad<R> where R is the value type produced by the callable
         */
        template<typename Left>
        auto operator()(Left&& left) const
        {
            if constexpr (monad_like<std::remove_cvref_t<Left>>) {
                // Case 2: Left is a monad wrapping a callable (partial application).
                using LeftDecayed = std::remove_cvref_t<Left>;
                using ArgDecayed  = Arg;
                if constexpr (expected_like<LeftDecayed> && expected_like<ArgDecayed>) {
                    static_assert(std::same_as<typename LeftDecayed::error_type, typename ArgDecayed::error_type>,
                                  "fxt::mwith(): all expected values in the pipeline must share the same error type E");
                    static_assert(same_expected_kind<LeftDecayed, ArgDecayed>,
                                  "fxt::mwith(): all expected values in the pipeline must be the same expected type "
                                  "(e.g. all std::expected or all tl::expected)");
                }
                return std::forward<Left>(left).and_then([this](auto&& f) {
                    return arg_.and_then([&f](auto&& t) {
                        return f(std::forward<decltype(t)>(t));
                    });
                });
            }
            else {
                // Case 1: Left is a plain callable returning a monad — pipeline entry point.
                using ArgDecayed  = Arg;
                using T           = typename ArgDecayed::value_type;
                using result_type = std::invoke_result_t<Left, T>;
                static_assert(monad_like<std::remove_cvref_t<result_type>>,
                              "fxt::mwith(): the callable must return a monad-like type (expected or optional) — "
                              "use fxt::with() for plain-value callables");
                if constexpr (expected_like<std::remove_cvref_t<result_type>> && expected_like<ArgDecayed>) {
                    static_assert(std::same_as<typename std::remove_cvref_t<result_type>::error_type,
                                               typename ArgDecayed::error_type>,
                                  "fxt::mwith(): the callable's expected error type must match the pipeline error type E");
                    static_assert(same_expected_kind<std::remove_cvref_t<result_type>, ArgDecayed>,
                                  "fxt::mwith(): the callable must return the same expected type as the pipeline "
                                  "(e.g. all std::expected or all tl::expected)");
                }
                return arg_.and_then([&left](auto&& t) {
                    return left(std::forward<decltype(t)>(t));
                });
            }
        }
    };

    /**
     * @brief Monadic applicative mwith() — wraps a monad argument for a curried pipeline
     *        whose callable returns a monad.
     *
     * Returns a pipe adaptor that feeds the given monad-wrapped argument into the
     * function on the left-hand side of |. Unlike fxt::with(), the callable is expected
     * to return a monad-like type; mwith() flattens that result via .and_then() rather
     * than wrapping it, avoiding a nested monad<monad<R>>.
     *
     * Works with both fxt::expected and fxt::optional, and accepts any compatible
     * monad-like type via a forwarding reference constrained on monad_like.
     *
     * @code
     * // expected pipeline — callable returns expected<R, E>
     * auto divide = fxt::curry([](double a, double b) -> fxt::expected<double, XLError> {
     *     if (b == 0.0) return fxt::unexpected<XLError>(XLError::DivByZero);
     *     return a / b;
     * });
     *
     * // Use fxt::with() to feed intermediate arguments, then fxt::mwith() for the final
     * // step where the callable returns expected<R, E> — mwith() flattens that result:
     * auto result = divide
     *             | fxt::with(validate_numerator(raw_n))      // expected<partial, XLError>
     *             | fxt::mwith(validate_denominator(raw_d));  // expected<double,  XLError>
     *
     * // optional pipeline — callable returns optional<R>
     * auto safe_div = fxt::curry([](double a, double b) -> fxt::optional<double> {
     *     if (b == 0.0) return fxt::nullopt;
     *     return a / b;
     * });
     * auto result2 = safe_div
     *              | fxt::with(maybe_numerator())
     *              | fxt::mwith(maybe_denominator());
     *
     * // Single-argument callable returning a monad: mwith() is the entry point
     * auto reciprocal = fxt::curry([](double x) -> fxt::expected<double, XLError> {
     *     if (x == 0.0) return fxt::unexpected<XLError>(XLError::DivByZero);
     *     return 1.0 / x;
     * });
     * auto r = reciprocal | fxt::mwith(validate(raw_x));   // expected<double, XLError>
     * @endcode
     *
     * @tparam M Monad type of the argument (deduced — satisfies monad_like)
     * @param m  The monad-wrapped argument to feed into the pipeline
     * @return A mwith_adaptor<M> pipe adaptor
     */
    template<typename M>
        requires monad_like<std::remove_cvref_t<M>>
    auto mwith(M&& m) -> mwith_adaptor<std::remove_cvref_t<M>>
    {
        return mwith_adaptor<std::remove_cvref_t<M>>{std::forward<M>(m)};
    }

    /**
     * @brief Pipe operator for the pipeline entry point: plain callable | mwith(m).
     *
     * Handles Case 1 — the left-hand side is a plain callable (e.g., the result of
     * fxt::curry()) rather than a monad. Applies it to the argument via .and_then(),
     * flattening the callable's monad result.
     *
     * Case 2 (monad<Func> | mwith(m)) is handled by the monad-specific pipe operators
     * in Expected.hpp or Optional.hpp, which dispatch to mwith_adaptor::operator().
     *
     * @requires F must not satisfy monad_like
     */
    template<typename F, typename Arg>
        requires (!monad_like<std::remove_cvref_t<F>>)
    auto operator|(F&& f, mwith_adaptor<Arg> adaptor)
    {
        return adaptor(std::forward<F>(f));
    }

}    // namespace fxt
