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
 * @brief sequence() and traverse() — container-level applicatives for fxt::expected
 *
 * ## Overview
 *
 * These operations lift expected-aware functions from individual elements to whole
 * containers, with short-circuit semantics on the first error.
 *
 * ### sequence
 * Converts a container of expected values into an expected container of values.
 * Succeeds only if every element succeeds; on the first error it short-circuits.
 *
 *     sequence(Container<expected<T,E>>) -> expected<Container<T>, E>
 *
 * ### traverse
 * Maps a function over a container and sequences the results in one step — equivalent
 * to sequence(transform(f, container)) but without an intermediate container.
 *
 *     traverse(Container<T>, F) -> expected<Container<U>, E>
 *     where F : T -> expected<U, E>
 *
 * traverse() is also available as a pipe adaptor:
 *
 *     inputs | fxt::traverse(validate) -> expected<Container<U>, E>
 *
 * ## Container requirements
 * - Iterable (begin/end)
 * - Has push_back
 * - Not a tuple type
 * - reserve() is used automatically if the container supports it and the range is sized
 *
 * ## Usage
 *
 * @code{.cpp}
 * // sequence: container of expected -> expected<container>
 * std::vector<fxt::expected<int, std::string>> results = { 1, 2, fxt::unexpected("bad") };
 * auto r = fxt::sequence(results);  // expected<vector<int>, string>{"bad"}
 *
 * // traverse: map + sequence in one step
 * std::vector<double> raw = { 10.0, 20.0, 30.0 };
 * auto validated = fxt::traverse(raw, validate_pressure);
 *
 * // pipe form
 * auto validated2 = raw | fxt::traverse(validate_pressure);
 * @endcode
 */

#pragma once

#include <functional>
#include <iterator>
#include <ranges>
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
     * @brief Concept for containers that can hold sequenced results.
     *
     * Satisfied by any type with begin/end iterators and push_back, excluding
     * tuple-like types (which have a separate, out-of-scope traversal model).
     */
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
    // traverse_adaptor — enables Container | traverse(f) pipe syntax
    // =========================================================================

    /**
     * @brief Pipe adaptor returned by the single-argument traverse(f) overload.
     *
     * Not intended for direct construction — use fxt::traverse(f) instead.
     */
    template<typename F>
    struct traverse_adaptor
    {
        F f;

        template<typename Container>
            requires sequenceable_container<std::remove_cvref_t<Container>>
        auto operator()(Container&& container) const;
    };

    // =========================================================================
    // traverse(Container, F) — free function form
    // =========================================================================

    /**
     * @brief Maps F over every element of a container and collects the results.
     *
     * F must return expected<U,E> for some U and E. The function short-circuits
     * on the first error, returning it immediately without processing further elements.
     * If all elements succeed, returns expected<Container<U>, E> holding all results.
     *
     * Elements are moved out of the container when it is passed as an rvalue.
     *
     * @tparam Container A sequenceable_container whose elements F can be applied to
     * @tparam F         Callable returning expected<U,E>
     * @param container  Input container
     * @param f          Mapping function
     * @return           expected<Container<U>, E>
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

    // =========================================================================
    // traverse(F) — pipe adaptor factory
    // =========================================================================

    /**
     * @brief Returns a pipe adaptor for use with operator|.
     *
     * @code
     * auto result = raw_inputs | fxt::traverse(validate);
     * @endcode
     */
    template<typename F>
    auto traverse(F&& f) -> traverse_adaptor<std::decay_t<F>>
    {
        return traverse_adaptor<std::decay_t<F>>{std::forward<F>(f)};
    }

    // traverse_adaptor::operator() — delegates to the two-argument free function
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

    // =========================================================================
    // sequence(Container<expected<T,E>>) — free function
    // =========================================================================

    /**
     * @brief Converts a container of expected values into an expected container.
     *
     * Short-circuits on the first error. If all elements hold values, returns
     * expected<Container<T>, E> containing all unwrapped values in order.
     * Values are moved out when the container is passed as an rvalue.
     *
     * @tparam Container A sequenceable_container whose value_type is expected_like
     * @param container  Input container of expected<T,E>
     * @return           expected<Container<T>, E>
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

}    // namespace fxt
