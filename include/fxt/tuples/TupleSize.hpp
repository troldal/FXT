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
 * @file TupleSize.hpp
 * @brief Compile-time tuple size query for fxt::tuple and fxt::flat_tuple
 *
 * This file provides a unified interface for querying the size of tuple types at compile time.
 * It defines the `fxt::tuple_size` trait and its helper variable template `fxt::tuple_size_v`,
 * which work seamlessly with both `fxt::tuple` (std::tuple) and `fxt::flat_tuple`.
 *
 * ## Overview
 *
 * The `tuple_size` trait is similar to `std::tuple_size` but is extended to work with
 * `fxt::flat_tuple` in addition to `fxt::tuple`. It provides specializations for all
 * cv-qualified variations (const, volatile, const volatile) of both tuple types.
 *
 * ## Main Components
 *
 * ### fxt::tuple_size
 * A type trait that provides the number of elements in a tuple type as a compile-time constant.
 * Available specializations:
 * - `tuple_size<tuple<Ts...>>` - For fxt::tuple (forwards to std::tuple_size)
 * - `tuple_size<flat_tuple<Ts...>>` - For fxt::flat_tuple
 * - cv-qualified versions for both types
 *
 * ### fxt::tuple_size_v
 * A convenient variable template that extracts the size value directly.
 * Equivalent to `tuple_size<T>::value`.
 *
 * ## Usage
 *
 * The tuple_size trait is primarily used internally by other tuple operations, but can
 * also be used directly in user code for compile-time size queries and SFINAE:
 *
 * @code
 * // Query tuple size at compile time
 * constexpr std::size_t size1 = fxt::tuple_size_v<fxt::tuple<int, double>>;  // 2
 * constexpr std::size_t size2 = fxt::tuple_size_v<fxt::flat_tuple<int, std::string, double>>;  // 3
 *
 * // Use in constexpr functions
 * template<typename T>
 * constexpr std::size_t get_tuple_size() {
 *     return fxt::tuple_size_v<T>;
 * }
 *
 * // Use in SFINAE constraints
 * template<typename T>
 * requires (fxt::tuple_size_v<T> > 0)
 * void process_non_empty_tuple(T&& t) {
 *     // Process tuple...
 * }
 *
 * // Works with cv-qualified types
 * constexpr std::size_t size3 = fxt::tuple_size_v<const fxt::tuple<int, int>>;  // 2
 * @endcode
 *
 * ## Design Notes
 *
 * - For `fxt::tuple`, the implementation forwards to `std::tuple_size` to maintain
 *   compatibility with the standard library
 * - For `fxt::flat_tuple`, the size is computed directly from the number of template
 *   parameters using `sizeof...(Ts)`
 * - All cv-qualified variations are supported to ensure the trait works correctly in
 *   generic contexts where tuple types may be const or volatile
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see std::tuple_size
 */

#pragma once

#include "FlatTuple.hpp"

// Only the unqualified specialization is needed. Since C++17, <tuple>
// provides template<class T> struct tuple_size<const T> : tuple_size<T> {}
// (and volatile / const volatile variants), so the cv-qualified forms are
// automatically derived from this primary specialization.
template<typename... Ts>
struct std::tuple_size<fxt::flat_tuple<Ts...>> : integral_constant<size_t, sizeof...(Ts)>{};

namespace fxt
{
    using std::tuple_size;
    using std::tuple_size_v;
}    // namespace fxt


