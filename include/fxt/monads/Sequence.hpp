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
 * @file Sequence.hpp
 * @brief sequence() and traverse() — applicatives over containers and tuples for fxt::expected
 *
 * ## Overview
 *
 * Two overload families, both with the same short-circuit-on-first-error semantics:
 *
 * ### Homogeneous container overloads (vector, list, deque, …)
 *
 *     sequence(Container<expected<T,E>>) -> expected<Container<T>, E>
 *     traverse(Container<T>, F)          -> expected<Container<U>, E>
 *     container | traverse(f)
 *
 * Container must have begin/end and push_back; must not be a tuple.
 * reserve() is called automatically for sized ranges.
 *
 * ### fxt::tuple overloads (heterogeneous, compile-time)
 *
 *     sequence(tuple<expected<T0,E>, expected<T1,E>, …>)
 *         -> expected<tuple<T0, T1, …>, E>
 *
 *     traverse(tuple<T0, T1, …>, F)
 *         -> expected<tuple<U0, U1, …>, E>   where F(Ti) -> expected<Ui, E>
 *
 *     tuple_value | traverse(f)
 *
 * All expected elements in a tuple sequence must share the same error type E —
 * enforced at compile time via pattern matching on the function signature.
 * Unlike the container version, the result type is fully known at compile time
 * with no type erasure.
 *
 * Note: fxt::traverse(F) is the single pipe-adaptor factory for both overload
 * families. Dispatch is performed at the call site by the type of the left-hand
 * side of |.
 *
 * ## Usage
 *
 * @code{.cpp}
 * // Container sequence
 * std::vector<fxt::expected<int, std::string>> v = {1, 2, fxt::unexpected("bad")};
 * auto r = fxt::sequence(v);   // expected<vector<int>, string>{"bad"}
 *
 * // Container traverse (pipe)
 * auto validated = raw_doubles | fxt::traverse(validate_pressure);
 *
 * // Tuple sequence (heterogeneous)
 * auto args = fxt::make_tuple(
 *     validate_pressure(raw_p),     // expected<double, XLError>
 *     validate_temperature(raw_t),  // expected<double, XLError>
 *     validate_quality(raw_q)       // expected<double, XLError>
 * );
 * auto result = fxt::sequence(std::move(args));
 * // result : expected<tuple<double, double, double>, XLError>
 *
 * // Tuple traverse (pipe, homogeneous function over heterogeneous elements)
 * auto pressures = fxt::make_tuple(10.0, 20.0, 30.0);
 * auto validated_p = pressures | fxt::traverse(validate_pressure);
 * // validated_p : expected<tuple<double, double, double>, XLError>
 * @endcode
 */

#pragma once

#include <functional>
#include <iterator>
#include <ranges>
#include <tuple>
#include <utility>
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsTuple.hpp"
#include "Expected.hpp"

namespace fxt
{
    // =========================================================================
    // sequenceable_container concept
    // =========================================================================

    /**
     * @brief Concept for homogeneous containers that can hold sequenced results.
     *
     * Satisfied by any type with begin/end iterators and push_back, excluding
     * tuple-like types (handled by separate overloads below).
     */
    // TODO: COMPLETENESS — sequence()/traverse() only support fxt::expected. The classic
    //       optional counterparts (Container<optional<T>> -> optional<Container<T>>, and the
    //       tuple equivalents) are missing; other m-prefixed operations in the library
    //       uniformly support both monads.
    // TODO: COMPLETENESS — sequenceable_container requires push_back, which excludes
    //       std::array, std::set, std::map and other non-push_back ranges; consider building
    //       results via std::ranges::to or an output-iterator strategy.
    template<typename C>
    concept sequenceable_container = requires(C& c) {
        std::begin(c);
        std::end(c);
        c.push_back(std::declval<typename C::value_type>());
    } && !tuple_like<std::remove_cvref_t<C>>;

    // =========================================================================
    // rebind_container_t — Container<Old> -> Container<New>
    // =========================================================================

    namespace impl
    {
        template<typename Container, typename NewValue>
        struct rebind_container;

        template<template<typename, typename...> typename Container,
                 typename OldValue, typename... Extra,
                 typename NewValue>
        struct rebind_container<Container<OldValue, Extra...>, NewValue> {
            using type = Container<NewValue>;
        };
    }

    /**
     * @brief Rebinds a container template to a different element type.
     *
     * rebind_container_t<std::vector<int>, double> == std::vector<double>
     */
    template<typename Container, typename NewValue>
    using rebind_container_t = typename impl::rebind_container<Container, NewValue>::type;

    // =========================================================================
    // sequence_result — type machinery for tuple sequence
    //
    // Maps tuple<expected<T0,E>, expected<T1,E>, …> to expected<tuple<T0,T1,…>,E>.
    // The pattern match on a single E enforces that all error types are identical;
    // mismatched error types produce a substitution failure at the call site.
    // =========================================================================

    namespace impl
    {
        template<typename Tuple>
        struct sequence_result;

        template<typename... Ts, typename E>
        struct sequence_result<fxt::tuple<fxt::expected<Ts, E>...>> {
            using type = fxt::expected<fxt::tuple<Ts...>, E>;
        };
    }

    /**
     * @brief Computes the result type of sequencing a tuple of expected values.
     *
     * sequence_result_t<tuple<expected<int,E>, expected<string,E>>>
     *     == expected<tuple<int, string>, E>
     *
     * Fails to compile if element error types differ — the pattern match on a
     * single E is the static enforcement of the same-error-type requirement.
     */
    template<typename Tuple>
    using sequence_result_t = typename impl::sequence_result<std::remove_cvref_t<Tuple>>::type;

    // =========================================================================
    // traverse_adaptor — enables Container | traverse(f) and tuple | traverse(f)
    // =========================================================================

    /**
     * @brief Pipe adaptor returned by the single-argument traverse(f) overload.
     *
     * Handles both homogeneous containers (via the container operator()) and
     * fxt::tuple values (via the tuple operator(), dispatched through TuplePipe).
     * Not intended for direct construction — use fxt::traverse(f) instead.
     */
    template<typename F>
    struct traverse_adaptor
    {
        F f;

        // Container overload — for vector, list, deque, …
        template<typename Container>
            requires sequenceable_container<std::remove_cvref_t<Container>>
        auto operator()(Container&& container) const;

        // Tuple overload — for fxt::tuple<Ts...> (std::tuple)
        template<typename Tuple>
            requires impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>>
        auto operator()(Tuple&& tup) const;
    };

    // =========================================================================
    // SECTION A — Homogeneous container overloads
    // =========================================================================

    /**
     * @brief Maps F over every element of a container and collects the results.
     *
     * F must return expected<U,E>. Short-circuits on the first error.
     * If all elements succeed, returns expected<Container<U>, E>.
     * Elements are moved out of the container when it is passed as an rvalue.
     */
    template<typename Container, typename F>
        requires sequenceable_container<std::remove_cvref_t<Container>>
              && expected_like<std::remove_cvref_t<
                     std::invoke_result_t<F&, typename std::remove_cvref_t<Container>::value_type&>
                 >>
    auto traverse(Container&& container, F&& f)
    {
        using ContainerDecayed = std::remove_cvref_t<Container>;
        using T = typename ContainerDecayed::value_type;
        using ExpectedU = std::remove_cvref_t<std::invoke_result_t<F&, T&>>;
        using U = typename ExpectedU::value_type;
        using E = typename ExpectedU::error_type;
        using ResultContainer = rebind_container_t<ContainerDecayed, U>;

        ResultContainer result;
        if constexpr (std::ranges::sized_range<ContainerDecayed> &&
                      requires { result.reserve(std::size_t{}); }) {
            result.reserve(std::ranges::size(container));
        }

        for (auto& elem : container) {
            auto mapped = std::invoke(f, std::forward_like<Container>(elem));
            if (!mapped.has_value()) {
                return fxt::expected<ResultContainer, E>{
                    fxt::unexpected(std::move(mapped).error())
                };
            }
            result.push_back(std::move(*mapped));
        }

        return fxt::expected<ResultContainer, E>{std::move(result)};
    }

    /**
     * @brief Returns a pipe adaptor for use with operator|.
     *
     * Works with both homogeneous containers and fxt::tuple values.
     *
     * @code
     * auto r1 = raw_vector | fxt::traverse(validate);
     * auto r2 = fxt::make_tuple(a, b, c) | fxt::traverse(validate);
     * @endcode
     */
    template<typename F>
    auto traverse(F&& f) -> traverse_adaptor<std::decay_t<F>>
    {
        return traverse_adaptor<std::decay_t<F>>{std::forward<F>(f)};
    }

    // Out-of-class: container operator() — defined after traverse(Container, F)
    template<typename F>
    template<typename Container>
        requires sequenceable_container<std::remove_cvref_t<Container>>
    auto traverse_adaptor<F>::operator()(Container&& container) const
    {
        return traverse(std::forward<Container>(container), f);
    }

    // Pipe operator: Container | traverse(f)
    template<typename Container, typename F>
        requires sequenceable_container<std::remove_cvref_t<Container>>
    auto operator|(Container&& container, const traverse_adaptor<F>& adaptor)
    {
        return adaptor(std::forward<Container>(container));
    }

    /**
     * @brief Converts a container of expected values into an expected container.
     *
     * Short-circuits on the first error. Values are moved when the container
     * is passed as an rvalue.
     */
    template<typename Container>
        requires sequenceable_container<std::remove_cvref_t<Container>>
              && expected_like<typename std::remove_cvref_t<Container>::value_type>
    auto sequence(Container&& container)
    {
        using ContainerDecayed = std::remove_cvref_t<Container>;
        using ExpectedT = typename ContainerDecayed::value_type;
        using T = typename ExpectedT::value_type;
        using E = typename ExpectedT::error_type;
        using ResultContainer = rebind_container_t<ContainerDecayed, T>;

        ResultContainer result;
        if constexpr (std::ranges::sized_range<ContainerDecayed> &&
                      requires { result.reserve(std::size_t{}); }) {
            result.reserve(std::ranges::size(container));
        }

        for (auto& elem : container) {
            if (!elem.has_value()) {
                return fxt::expected<ResultContainer, E>{
                    fxt::unexpected(std::forward_like<Container>(elem).error())
                };
            }
            result.push_back(*std::forward_like<Container>(elem));
        }

        return fxt::expected<ResultContainer, E>{std::move(result)};
    }

    // =========================================================================
    // SECTION B — fxt::tuple overloads
    //
    // All type deduction happens at compile time via index sequences.
    // The result type is fully known statically — no type erasure.
    // =========================================================================

    namespace impl
    {
        /**
         * @brief Core implementation of sequence for a tuple of expected values.
         *
         * Two-phase approach:
         *   Phase 1 — short-circuit OR fold to detect the first error
         *   Phase 2 — pack expansion to extract all values (only reached if no errors)
         *
         * @tparam Tuple  The tuple type (fxt::tuple<expected<Ts,E>...>)
         * @tparam E      The shared error type
         * @tparam Is     Index sequence matching the tuple arity
         */
        template<typename Tuple, typename E, std::size_t... Is>
        auto sequence_tuple_impl(Tuple&& t, std::index_sequence<Is...>)
            -> sequence_result_t<std::remove_cvref_t<Tuple>>
        {
            using Result = sequence_result_t<std::remove_cvref_t<Tuple>>;

            // Phase 1: scan left-to-right, stop at the first error.
            // The || fold evaluates left to right and short-circuits on true.
            E* first_error = nullptr;
            (... || (std::get<Is>(t).has_value()
                     ? false
                     : (first_error = &std::get<Is>(t).error(), true)));

            if (first_error) {
                return fxt::unexpected(std::move(*first_error));
            }

            // Phase 2: all elements have values — extract them into a result tuple.
            // std::get<Is>(forward(t)) on an rvalue tuple returns Ti&&, enabling moves.
            return Result{fxt::make_tuple(*std::get<Is>(std::forward<Tuple>(t))...)};
        }

        /**
         * @brief Core implementation of traverse for a tuple.
         *
         * Applies F to each element, producing a tuple of expected values,
         * then delegates to sequence_tuple_impl.
         *
         * E is determined explicitly from the first element's invoke result to
         * avoid template argument deduction failures that arise when a non-pack
         * type appears inside a pack expansion at the call site of sequence().
         *
         * Note: argument evaluation order of make_tuple is unspecified in C++.
         * For stateless validators this is not observable; for stateful F the
         * invocation order is implementation-defined.
         */
        template<typename Tuple, typename F, std::size_t... Is>
        auto traverse_tuple_impl(Tuple&& t, F&& f, std::index_sequence<Is...>)
        {
            static_assert(sizeof...(Is) > 0,
                "fxt::traverse for tuple does not support empty tuples: "
                "cannot determine the error type E from zero invocation results");

            // Determine E from the first element's return type.
            // All elements must produce the same E — sequence_tuple_impl enforces this.
            using E = typename std::remove_cvref_t<
                std::invoke_result_t<F&, std::tuple_element_t<0, std::remove_cvref_t<Tuple>>&>
            >::error_type;

            auto mapped = fxt::make_tuple(
                std::invoke(std::forward<F>(f), std::get<Is>(std::forward<Tuple>(t)))...
            );
            using MappedTuple = std::remove_cvref_t<decltype(mapped)>;

            return sequence_tuple_impl<MappedTuple, E>(
                std::move(mapped),
                std::index_sequence<Is...>{}
            );
        }
    }    // namespace impl

    /**
     * @brief sequence() for a fxt::tuple of expected values.
     *
     * All elements must share the same error type E — enforced statically by the
     * function signature (a single E deduced from all expected<Ts,E>... elements).
     * Mismatched error types produce a substitution failure at the call site.
     *
     * Short-circuits on the first error encountered (left-to-right scan).
     *
     * @code
     * auto args = fxt::make_tuple(
     *     validate_pressure(p),     // expected<double, Err>
     *     validate_temperature(t),  // expected<double, Err>
     *     validate_quality(q)       // expected<double, Err>
     * );
     * auto result = fxt::sequence(std::move(args));
     * // result : expected<tuple<double, double, double>, Err>
     * @endcode
     */
    template<typename... Ts, typename E>
    auto sequence(fxt::tuple<fxt::expected<Ts, E>...> t) -> fxt::expected<fxt::tuple<Ts...>, E>
    {
        return impl::sequence_tuple_impl<decltype(t), E>(
            std::move(t),
            std::index_sequence_for<Ts...>{}
        );
    }

    /**
     * @brief traverse() for a fxt::tuple — maps F over each element and sequences.
     *
     * F is applied to each element Ti producing expected<Ui, E>. The result is
     * expected<tuple<U0, U1, …>, E>. All invocations of F must return the same
     * error type E; type mismatches are caught at compile time.
     *
     * Equivalent to sequence(make_tuple(f(get<0>(t)), f(get<1>(t)), …)) but
     * without a named intermediate variable.
     *
     * @code
     * auto pressures = fxt::make_tuple(10.0, 20.0, 30.0);
     * auto result = fxt::traverse(pressures, validate_pressure);
     * // result : expected<tuple<double, double, double>, Err>
     * @endcode
     */
    template<typename... Ts, typename F>
    auto traverse(fxt::tuple<Ts...> t, F&& f)
    {
        return impl::traverse_tuple_impl(
            std::move(t),
            std::forward<F>(f),
            std::index_sequence_for<Ts...>{}
        );
    }

    // Out-of-class: tuple operator() — defined after traverse(tuple<Ts...>, F)
    // The existing TuplePipe operator| handles "tuple | traverse(f)" dispatch by
    // checking std::invocable<traverse_adaptor<F>, Tuple&&>, which calls this overload.
    template<typename F>
    template<typename Tuple>
        requires impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>>
    auto traverse_adaptor<F>::operator()(Tuple&& tup) const
    {
        return traverse(std::forward<Tuple>(tup), f);
    }

}    // namespace fxt
