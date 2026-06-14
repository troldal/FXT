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

#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    /**
     * @brief Concept to check if a type behaves like an optional type
     *
     * This concept verifies both structural requirements (type aliases) and
     * behavioral requirements (core operations) of an optional-like type.
     * It ensures the type has the necessary members and operations to be
     * used in monadic contexts.
     */
    template<typename T>
    concept optional_like = requires(T t) {
        // Required type alias
        typename T::value_type;

        // Core optional-like operations
        { t.has_value() } -> std::convertible_to<bool>;
        { static_cast<bool>(t) } -> std::convertible_to<bool>;

        // Value access operations (just check they exist, don't require convertibility for move-only types)
        t.value();
        *t;

        // Monadic operations required for pipeline composition. and_then's return type
        // must be a specialization of the same optional template, so T itself (never
        // actually constructed, the body just throws) is the only portable probe.
        // Probe the rvalue-qualified overloads via std::move(t), mirroring
        // expected_like, so move-only value_types are supported.
        std::move(t).transform([](auto&&) -> typename T::value_type { throw 0; });
        std::move(t).and_then([](auto&&) -> T { throw 0; });

    } && std::is_same_v<std::decay_t<T>, T> // Ensure we work with decayed types
      && !requires(T t) { t.error(); };     // Must NOT have .error() member

    namespace impl
    {
        template<typename T1, typename T2>
        struct same_optional_kind_impl : std::false_type
        {};

        template<template<typename> class TOptional, typename V1, typename V2>
        struct same_optional_kind_impl<TOptional<V1>, TOptional<V2>> : std::true_type
        {};
    }    // namespace impl

    /**
     * @brief Concept to check that two optional-like types are instantiations of the
     *        same underlying template (e.g. both std::optional, or both tl::optional).
     *
     * Mirrors same_expected_kind: two optional-like types can share a value_type while
     * still being incompatible "kinds" (e.g. std::optional<int> vs tl::optional<int>) —
     * mixing them in a single monadic chain typically fails deep inside and_then's
     * return-type constraints. This concept lets adaptors that combine multiple
     * optional-like operands fail with a clear diagnostic instead.
     */
    template<typename T1, typename T2>
    concept same_optional_kind = optional_like<T1> && optional_like<T2> && impl::same_optional_kind_impl<T1, T2>::value;

}    // namespace fxt
