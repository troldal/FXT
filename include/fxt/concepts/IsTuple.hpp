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
#include <concepts>
#include <cstddef>
#include <tuple>
#include <type_traits>
#include <utility>

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

    // ------------------------------------------------------------------
    // Generic tuple-protocol detection: anything with std::tuple_size,
    // std::tuple_element, and a working get<I> (member or ADL) for every
    // index — e.g. std::pair, std::array, std::ranges::subrange, and other
    // third-party types that opt into the structured-bindings protocol.
    // ------------------------------------------------------------------

    template<typename T>
    concept has_tuple_size = requires {
        typename std::tuple_size<T>::type;
        requires std::derived_from<std::tuple_size<T>, std::integral_constant<std::size_t, std::tuple_size_v<T>>>;
    };

    template<typename T, std::size_t I>
    concept has_tuple_element = requires(T& t) {
        typename std::tuple_element_t<I, T>;
    } && (requires(T& t) { t.template get<I>(); } || requires(T& t) { get<I>(t); });

    template<typename T, std::size_t... Is>
    consteval bool all_tuple_elements_gettable(std::index_sequence<Is...>)
    {
        return (has_tuple_element<T, Is> && ...);
    }

    template<typename T>
    concept has_tuple_protocol =
        !std::is_reference_v<T> &&
        has_tuple_size<T> &&
        all_tuple_elements_gettable<T>(std::make_index_sequence<std::tuple_size_v<T>>{});

} // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Concept to check if a type is a tuple-like type
     *
     * A tuple-like type is either fxt::tuple (std::tuple) / fxt::flat_tuple, or any
     * other type that participates in the standard tuple protocol — i.e. has
     * std::tuple_size, std::tuple_element, and a working get<I>() (member or
     * ADL-found free function) for every index. This covers std::pair, std::array,
     * std::ranges::subrange, and other third-party "structured-bindable" types.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::tuple_like T>
     * void process_tuple(T&& tpl) {
     *     // Works with fxt::tuple, fxt::flat_tuple, std::pair, std::array, ...
     * }
     *
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto ft = fxt::make_flat_tuple(1.0, 2.0);
     * auto p  = std::pair{1, 2.0};
     * auto a  = std::array{1, 2, 3};
     * process_tuple(t);  // OK
     * process_tuple(ft); // OK
     * process_tuple(p);  // OK
     * process_tuple(a);  // OK
     * process_tuple(42); // Error: 42 is not tuple_like
     * @endcode
     */
    template<typename T>
    concept tuple_like = impl::is_tuple_like_v<std::remove_cvref_t<T>>
                      || impl::has_tuple_protocol<std::remove_cvref_t<T>>;

} // namespace fxt

