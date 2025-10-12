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

#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include <tuple>
#include <type_traits>

namespace fxt
{
    // ========================================================================
    // Primary template - undefined for non-tuple types
    // ========================================================================

    template<std::size_t I, typename T>
    struct tuple_element;

    // ========================================================================
    // Specialization for fxt::tuple (std::tuple)
    // ========================================================================

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, fxt::tuple<Ts...>> : std::tuple_element<I, std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const fxt::tuple<Ts...>> : std::tuple_element<I, const std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, volatile fxt::tuple<Ts...>> : std::tuple_element<I, volatile std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const volatile fxt::tuple<Ts...>> : std::tuple_element<I, const volatile std::tuple<Ts...>>
    {
    };

    // ========================================================================
    // Specialization for fxt::flat_tuple
    // ========================================================================

    namespace impl
    {
        // Helper to extract type at index I from flat_tuple
        template<std::size_t I, typename... Ts>
        struct flat_tuple_element_impl;

        template<std::size_t I, typename T, typename... Ts>
        struct flat_tuple_element_impl<I, T, Ts...>
        {
            using type = typename flat_tuple_element_impl<I - 1, Ts...>::type;
        };

        template<typename T, typename... Ts>
        struct flat_tuple_element_impl<0, T, Ts...>
        {
            using type = T;
        };
    }

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, fxt::flat_tuple<Ts...>>
    {
        using type = typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const fxt::flat_tuple<Ts...>>
    {
        using type = const typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, volatile fxt::flat_tuple<Ts...>>
    {
        using type = volatile typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const volatile fxt::flat_tuple<Ts...>>
    {
        using type = const volatile typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    // ========================================================================
    // Convenience type alias
    // ========================================================================

    template<std::size_t I, typename T>
    using tuple_element_t = typename tuple_element<I, T>::type;

}    // namespace fxt
