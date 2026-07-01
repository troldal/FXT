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
 * @file VariantAlternative.hpp
 * @brief Compile-time membership test for variant-like types
 *
 * This file provides `fxt::variant_alternative_count` / `fxt::variant_alternative_count_v`
 * (how many alternatives of a variant-like type exactly match a given type) and
 * `fxt::is_variant_alternative` / `fxt::is_variant_alternative_v` (whether at least one
 * alternative matches).
 *
 * ## Naming note
 *
 * This is deliberately *not* named `variant_contains` or similar: those names could be
 * misread as testing the variant's *currently held* value (the job of
 * `fxt::holds_alternative`, a runtime check on a variant instance). `is_variant_alternative`
 * is purely a compile-time question about the variant *type* — "is T one of the alternatives
 * this variant type could ever hold?" — with no relationship to any specific instance.
 *
 * ## Overview
 *
 * Both traits are implemented purely in terms of `std::variant_size_v` and
 * `std::variant_alternative_t`, the same customization points `std::get<T>` and
 * `std::holds_alternative<T>` rely on. Since `fxt::variant` is a plain alias for
 * `std::variant`, no fxt-specific specialization is needed; the traits work directly
 * for `fxt::variant` and for any third-party sum type that specializes
 * `std::variant_size`/`std::variant_alternative` (e.g. mpark::variant,
 * boost::variant2::variant).
 *
 * ## Main Components
 *
 * ### fxt::variant_alternative_count<T, Variant> / fxt::variant_alternative_count_v<T, Variant>
 * The number of alternatives in `Variant` whose type is exactly `T` (no decay, no
 * implicit conversion — matching the exactness of `std::get<T>`/`std::holds_alternative<T>`).
 *
 * ### fxt::is_variant_alternative<T, Variant> / fxt::is_variant_alternative_v<T, Variant>
 * True if `variant_alternative_count_v<T, Variant>` is greater than zero, i.e. `Variant`
 * has at least one alternative of type `T`.
 *
 * ## Usage
 *
 * @code
 * using V = fxt::variant<int, double, std::string>;
 *
 * static_assert(fxt::is_variant_alternative_v<int, V>);
 * static_assert(fxt::is_variant_alternative_v<std::string, V>);
 * static_assert(!fxt::is_variant_alternative_v<float, V>);
 *
 * // Type-level question, independent of any held value:
 * V v{42};
 * static_assert(fxt::is_variant_alternative_v<std::string, V>);   // true: V can hold a string
 * bool holds_string = fxt::holds_alternative<std::string>(v);    // false: v doesn't right now
 *
 * // Useful as a SFINAE / requires-clause constraint
 * template<typename Variant>
 *     requires fxt::is_variant_alternative_v<std::string, Variant>
 * void needs_a_string_alternative(Variant&&) { ... }
 * @endcode
 *
 * @see fxt::variant_like
 * @see fxt::holds_alternative
 * @see fxt::tuple_contains
 */

#pragma once

#include "../concepts/IsVariant.hpp"
#include <concepts>
#include <cstddef>
#include <type_traits>
#include <utility>
#include <variant>

namespace fxt::impl
{
    // ------------------------------------------------------------------
    // Generic variant-size protocol detection: std::variant_size and
    // std::variant_alternative specialized for every index. Kept separate
    // from fxt::variant_like (IsVariant.hpp), which deliberately checks
    // .index()/get<0> instead so it does not depend on variant_size being
    // specialized. This trait, however, needs variant_size/variant_alternative
    // directly, so it is checked here rather than folded into variant_like.
    // ------------------------------------------------------------------

    template<typename T>
    concept has_variant_size = requires {
        typename std::variant_size<T>::type;
        requires std::derived_from<std::variant_size<T>, std::integral_constant<std::size_t, std::variant_size_v<T>>>;
    };

    template<typename T, std::size_t I>
    concept has_variant_alternative = requires {
        typename std::variant_alternative_t<I, T>;
    };

    template<typename T, std::size_t... Is>
    consteval bool all_variant_alternatives_named(std::index_sequence<Is...>)
    {
        return (has_variant_alternative<T, Is> && ...);
    }

    template<typename T>
    concept has_variant_size_protocol =
        !std::is_reference_v<T> &&
        has_variant_size<T> &&
        all_variant_alternatives_named<T>(std::make_index_sequence<std::variant_size_v<T>>{});

    template<typename T, typename Variant, std::size_t... Is>
    consteval std::size_t variant_alternative_count_impl(std::index_sequence<Is...>)
    {
        return (std::size_t{std::is_same_v<T, std::variant_alternative_t<Is, Variant>>} + ... + std::size_t{0});
    }

}    // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Number of alternatives in a variant-like type whose type is exactly T
     *
     * @tparam T The alternative type to search for (matched exactly, no decay)
     * @tparam Variant Any type satisfying fxt::variant_like and specializing
     *                 std::variant_size / std::variant_alternative
     */
    template<typename T, typename Variant>
        requires variant_like<Variant> && impl::has_variant_size_protocol<std::remove_cvref_t<Variant>>
    struct variant_alternative_count
        : std::integral_constant<std::size_t,
              impl::variant_alternative_count_impl<T, std::remove_cvref_t<Variant>>(
                  std::make_index_sequence<std::variant_size_v<std::remove_cvref_t<Variant>>>{})>
    {};

    /// @brief Convenience variable template for fxt::variant_alternative_count
    template<typename T, typename Variant>
    inline constexpr std::size_t variant_alternative_count_v = variant_alternative_count<T, Variant>::value;

    /**
     * @brief Whether a variant-like type has at least one alternative of type T
     *
     * A purely type-level question — see the file-level docs for why this is not
     * named `variant_contains` and how it differs from `fxt::holds_alternative`.
     *
     * @tparam T The alternative type to search for (matched exactly, no decay)
     * @tparam Variant Any type satisfying fxt::variant_like and specializing
     *                 std::variant_size / std::variant_alternative
     */
    template<typename T, typename Variant>
        requires variant_like<Variant> && impl::has_variant_size_protocol<std::remove_cvref_t<Variant>>
    struct is_variant_alternative : std::bool_constant<(variant_alternative_count_v<T, Variant> > 0)> {};

    /// @brief Convenience variable template for fxt::is_variant_alternative
    template<typename T, typename Variant>
    inline constexpr bool is_variant_alternative_v = is_variant_alternative<T, Variant>::value;

}    // namespace fxt
