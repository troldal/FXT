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
 * @file TupleContains.hpp
 * @brief Compile-time membership test for tuple-like types
 *
 * This file provides `fxt::tuple_type_count` / `fxt::tuple_type_count_v` (how many
 * elements of a tuple-like type exactly match a given type) and `fxt::tuple_contains`
 * / `fxt::tuple_contains_v` (whether at least one element matches).
 *
 * ## Overview
 *
 * Both traits are implemented purely in terms of `fxt::tuple_size_v` and
 * `fxt::tuple_element_t`, so they work uniformly for `fxt::tuple` (std::tuple),
 * `fxt::flat_tuple`, and any other type that participates in the standard tuple
 * protocol (std::pair, std::array, std::ranges::subrange, ...) — anything that
 * satisfies `fxt::tuple_like`. No per-tuple-kind specialization is needed.
 *
 * ## Main Components
 *
 * ### fxt::tuple_type_count<T, Tuple> / fxt::tuple_type_count_v<T, Tuple>
 * The number of elements in `Tuple` whose type is exactly `T` (no decay, no
 * implicit conversion — matching the exactness of `std::get<T>`/`fxt::get<T>`).
 *
 * ### fxt::tuple_contains<T, Tuple> / fxt::tuple_contains_v<T, Tuple>
 * True if `tuple_type_count_v<T, Tuple>` is greater than zero, i.e. `Tuple`
 * contains at least one element of type `T`.
 *
 * ## Usage
 *
 * @code
 * static_assert(fxt::tuple_contains_v<int, fxt::tuple<int, double, std::string>>);
 * static_assert(!fxt::tuple_contains_v<float, fxt::tuple<int, double, std::string>>);
 *
 * static_assert(fxt::tuple_contains_v<double, fxt::flat_tuple<int, double>>);
 *
 * // Also works with any type satisfying fxt::tuple_like
 * static_assert(fxt::tuple_contains_v<int, std::pair<int, double>>);
 * static_assert(fxt::tuple_contains_v<int, std::array<int, 3>>);
 *
 * // Duplicate elements are counted, not just detected
 * static_assert(fxt::tuple_type_count_v<int, fxt::tuple<int, int, double>> == 2);
 *
 * // Useful as a SFINAE / requires-clause constraint
 * template<typename Tuple>
 *     requires fxt::tuple_contains_v<std::string, Tuple>
 * void needs_a_string(Tuple&& t) { ... }
 * @endcode
 *
 * @see fxt::tuple_like
 * @see fxt::tuple_size_v
 * @see fxt::tuple_element_t
 * @see fxt::get
 */

#pragma once

#include "TupleSize.hpp"
#include "TupleElement.hpp"
#include "../concepts/IsTuple.hpp"
#include <cstddef>
#include <type_traits>
#include <utility>

namespace fxt::impl
{
    template<typename T, typename Tuple, std::size_t... Is>
    consteval std::size_t tuple_type_count_impl(std::index_sequence<Is...>)
    {
        return (std::size_t{std::is_same_v<T, fxt::tuple_element_t<Is, Tuple>>} + ... + std::size_t{0});
    }
}    // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Number of elements in a tuple-like type whose type is exactly T
     *
     * @tparam T The element type to search for (matched exactly, no decay)
     * @tparam Tuple Any type satisfying fxt::tuple_like
     */
    template<typename T, typename Tuple>
        requires tuple_like<Tuple>
    struct tuple_type_count
        : std::integral_constant<std::size_t,
              impl::tuple_type_count_impl<T, std::remove_cvref_t<Tuple>>(
                  std::make_index_sequence<fxt::tuple_size_v<std::remove_cvref_t<Tuple>>>{})>
    {};

    /// @brief Convenience variable template for fxt::tuple_type_count
    template<typename T, typename Tuple>
    inline constexpr std::size_t tuple_type_count_v = tuple_type_count<T, Tuple>::value;

    /**
     * @brief Whether a tuple-like type contains at least one element of type T
     *
     * @tparam T The element type to search for (matched exactly, no decay)
     * @tparam Tuple Any type satisfying fxt::tuple_like
     */
    template<typename T, typename Tuple>
        requires tuple_like<Tuple>
    struct tuple_contains : std::bool_constant<(tuple_type_count_v<T, Tuple> > 0)> {};

    /// @brief Convenience variable template for fxt::tuple_contains
    template<typename T, typename Tuple>
    inline constexpr bool tuple_contains_v = tuple_contains<T, Tuple>::value;

}    // namespace fxt
