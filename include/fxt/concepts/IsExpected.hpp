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
     * @brief Concept to check if a type behaves like an expected type
     *
     * This concept verifies both structural requirements (type aliases) and
     * behavioral requirements (core operations) of an expected-like type.
     * It ensures the type has the necessary members and operations to be
     * used in monadic contexts.
     *
     * Deliberately minimal: it does not require a `std::unexpected`-style
     * `unexpected_type` alias, so expected-like result types from other
     * libraries (e.g. boost::outcome, custom Result types) can satisfy it
     * even if they lack that alias. Call sites that need to construct a new
     * error result generically (e.g. `T::unexpected_type{error}`) should
     * require expected_constructible_like instead.
     */
    template<typename T>
    concept expected_like = requires(T t) {
        // Required type aliases
        typename T::value_type;
        typename T::error_type;

        // Core expected-like operations
        { t.has_value() } -> std::convertible_to<bool>;
        { static_cast<bool>(t) } -> std::convertible_to<bool>;

        // Value access operations (just check they exist, don't require convertibility for move-only types)
        t.value();
        t.error();
        *t;

        // Monadic operations required for pipeline composition. and_then's return type
        // must be a specialization of the same expected template with the same
        // error_type, so T itself (never actually constructed, the body just throws)
        // is the only portable probe. Probe the rvalue-qualified overloads via
        // std::move(t): the lvalue overloads of transform/and_then build their error
        // path from `E&`, requiring error_type to be copy-constructible, which would
        // wrongly reject expected-likes with a move-only error_type.
        std::move(t).transform([](auto&&) -> typename T::value_type { throw 0; });
        std::move(t).and_then([](auto&&) -> T { throw 0; });
    } && std::is_same_v<std::decay_t<T>, T>; // Ensure we work with decayed types

    /**
     * @brief Bool variable-template wrapper for @ref expected_like.
     *
     * Exists for the same reason as @c fxt::monad_like_v (see IsMonad.hpp):
     * constraining a *variadic* function template directly on @ref expected_like
     * makes clang-cl's mangler fail with "cannot mangle this pack expansion yet".
     */
    template<typename T>
    inline constexpr bool expected_like_v = expected_like<T>;

    /**
     * @brief Refinement of expected_like for types that can be constructed from their
     *        own "unexpected" wrapper (the std::expected / tl::expected pattern).
     *
     * Required by adaptors that manufacture a brand-new error result generically,
     * e.g. `typename T::unexpected_type(error)`. Most code only needs expected_like;
     * this refinement is for the minority of call sites that build a new instance
     * of T representing an error rather than just consuming an existing one.
     */
    template<typename T>
    concept expected_constructible_like =
        expected_like<T> &&
        requires { typename T::unexpected_type; } &&
        std::constructible_from<T, typename T::unexpected_type>;

    namespace impl
    {
        template<typename T1, typename T2>
        struct same_expected_kind_impl : std::false_type
        {};

        template<template<typename, typename> class TExpected, typename V1, typename E1, typename V2, typename E2>
        struct same_expected_kind_impl<TExpected<V1, E1>, TExpected<V2, E2>> : std::true_type
        {};
    }    // namespace impl

    /**
     * @brief Concept to check that two expected-like types are instantiations of the
     *        same underlying template (e.g. both std::expected, or both tl::expected).
     *
     * Two expected-like types can share a value_type, an error_type, or both, while
     * still being incompatible "kinds" (e.g. std::expected<int, E> vs
     * tl::expected<int, E>) — mixing them in a single monadic chain typically fails
     * deep inside and_then's return-type constraints. This concept lets adaptors that
     * combine multiple expected-like operands fail with a clear diagnostic instead.
     */
    template<typename T1, typename T2>
    concept same_expected_kind = expected_like<T1> && expected_like<T2> && impl::same_expected_kind_impl<T1, T2>::value;

}    // namespace fxt
