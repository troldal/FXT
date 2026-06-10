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

#include "../tuples/Tuple.hpp"
#include "../tuples/FlatTuple.hpp"
#include <type_traits>

namespace fxt::impl
{
    // Helper to detect if a type is fxt::tuple (std::tuple)
    template<typename T>
    struct is_fxt_tuple : std::false_type {};

    template<typename... Ts>
    struct is_fxt_tuple<fxt::tuple<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_fxt_tuple_v = is_fxt_tuple<std::remove_cvref_t<T>>::value;

    // Helper to detect if a type is fxt::flat_tuple
    template<typename T>
    struct is_flat_tuple : std::false_type {};

    template<typename... Ts>
    struct is_flat_tuple<flat_tuple<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_flat_tuple_v = is_flat_tuple<std::remove_cvref_t<T>>::value;

    // Helper to detect if a type is a tuple-like type (either fxt::tuple or fxt::flat_tuple)
    template<typename T>
    inline constexpr bool is_tuple_like_v = is_fxt_tuple_v<T> || is_flat_tuple_v<T>;

} // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Concept to check if a type is a tuple-like type
     *
     * A tuple-like type is either an fxt::tuple (std::tuple) or an fxt::flat_tuple.
     * This concept can be used to constrain template parameters to only accept tuple types.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::tuple_like T>
     * void process_tuple(T&& tpl) {
     *     // Works with both fxt::tuple and fxt::flat_tuple
     * }
     *
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto ft = fxt::make_flat_tuple(1.0, 2.0);
     * process_tuple(t);  // OK
     * process_tuple(ft); // OK
     * process_tuple(42); // Error: 42 is not tuple_like
     * @endcode
     */
    // TODO: NAMING/COMPLETENESS — `tuple_like` only matches fxt::tuple (std::tuple) and
    //       fxt::flat_tuple, but the name suggests the broader C++23 tuple-like notion
    //       (std::pair, std::array, std::ranges::subrange all have tuple_size/tuple_element
    //       and would be rejected here). Either rename (e.g. fxt_tuple) or widen the concept
    //       to anything with std::tuple_size — note `variant_like` (IsVariant.hpp) has the
    //       same exact-match-only behavior behind a "-like" name.
    template<typename T>
    concept tuple_like = impl::is_tuple_like_v<std::remove_cvref_t<T>>;

} // namespace fxt

