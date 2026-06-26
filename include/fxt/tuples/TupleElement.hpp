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
 * @file TupleElement.hpp
 * @brief Compile-time type extraction for tuple elements
 *
 * This file provides the `fxt::tuple_element` trait and its helper type alias `fxt::tuple_element_t`,
 * which extract the type of an element at a specific index from a tuple type at compile time.
 * These traits work seamlessly with both `fxt::tuple` (std::tuple) and `fxt::flat_tuple`.
 *
 * ## Overview
 *
 * The `tuple_element` trait is similar to `std::tuple_element` but is extended to work with
 * `fxt::flat_tuple` in addition to `fxt::tuple`. It provides specializations for all
 * cv-qualified variations (const, volatile, const volatile) of both tuple types, ensuring
 * that cv-qualifiers are properly propagated to the extracted element type.
 *
 * ## Main Components
 *
 * ### fxt::tuple_element
 * A type trait that extracts the type of the element at index I from a tuple type.
 * Available specializations:
 * - `tuple_element<I, tuple<Ts...>>` - For fxt::tuple (forwards to std::tuple_element)
 * - `tuple_element<I, flat_tuple<Ts...>>` - For fxt::flat_tuple (custom implementation)
 * - cv-qualified versions for both types (properly propagates cv-qualifiers)
 *
 * ### fxt::tuple_element_t
 * A convenient type alias that extracts the element type directly.
 * Equivalent to `typename tuple_element<I, T>::type`.
 *
 * ## Usage
 *
 * The tuple_element trait is primarily used internally by other tuple operations and in
 * template metaprogramming, but can also be used directly in user code:
 *
 * @code
 * // Extract element types at compile time
 * using T1 = fxt::tuple_element_t<0, fxt::tuple<int, double, std::string>>;  // int
 * using T2 = fxt::tuple_element_t<1, fxt::tuple<int, double, std::string>>;  // double
 * using T3 = fxt::tuple_element_t<2, fxt::tuple<int, double, std::string>>;  // std::string
 *
 * // Works with fxt::flat_tuple
 * using T4 = fxt::tuple_element_t<0, fxt::flat_tuple<char, float, bool>>;  // char
 * using T5 = fxt::tuple_element_t<2, fxt::flat_tuple<char, float, bool>>;  // bool
 *
 * // CV-qualifiers are properly propagated
 * using T6 = fxt::tuple_element_t<0, const fxt::tuple<int, double>>;  // const int
 * using T7 = fxt::tuple_element_t<1, volatile fxt::flat_tuple<int, double>>;  // volatile double
 *
 * // Use in template metaprogramming
 * template<typename Tuple>
 * using FirstElement = fxt::tuple_element_t<0, Tuple>;
 *
 * template<typename Tuple>
 * using LastElement = fxt::tuple_element_t<fxt::tuple_size_v<Tuple> - 1, Tuple>;
 *
 * // Use in function return types
 * template<std::size_t I, typename Tuple>
 * auto get_element(Tuple&& t) -> fxt::tuple_element_t<I, std::remove_cvref_t<Tuple>> {
 *     return fxt::get<I>(std::forward<Tuple>(t));
 * }
 * @endcode
 *
 * ## Design Notes
 *
 * - For `fxt::tuple`, the implementation forwards to `std::tuple_element` to maintain
 *   compatibility with the standard library
 * - For `fxt::flat_tuple`, a custom recursive implementation is used to extract the type
 *   at the specified index from the parameter pack
 * - All cv-qualified variations properly propagate qualifiers to the extracted element type
 * - The trait is undefined for indices out of range (compile-time error), providing type safety
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::tuple_size
 * @see std::tuple_element
 */

#pragma once

#include "FlatTuple.hpp"
#include <tuple>

// Only the unqualified specialization is needed. Since C++17, <tuple>
// provides template<size_t I, class T> struct tuple_element<I, const T>
// (and volatile / const volatile variants) that forward to the unqualified
// form, so the cv-qualified forms are automatically derived from this one.
template<size_t I, typename... Ts>
struct std::tuple_element<I, fxt::flat_tuple<Ts...>>{ using type = fxt::flat_tuple<Ts...>::template type_at<I>; };

namespace fxt
{
    // std::tuple_element already covers fxt::tuple (= std::tuple) and
    // fxt::flat_tuple (specialised above in namespace std), including all
    // cv-qualified variants via C++17's built-in propagation helpers.
    using std::tuple_element;
    using std::tuple_element_t;
}    // namespace fxt
