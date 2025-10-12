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

namespace fxt
{
    /**
     * @brief Get the size of a tuple type at compile time
     *
     * Primary template - provides tuple_size functionality for fxt tuple types.
     * Specializations exist for fxt::tuple (std::tuple) and fxt::flat_tuple.
     */
    template<typename T>
    struct tuple_size;

    /**
     * @brief Specialization for fxt::tuple (which is std::tuple)
     *
     * Forwards to std::tuple_size for std::tuple.
     */
    template<typename... Ts>
    struct tuple_size<tuple<Ts...>> : std::tuple_size<std::tuple<Ts...>>
    {
    };

    /**
     * @brief Specialization for const fxt::tuple
     */
    template<typename... Ts>
    struct tuple_size<const tuple<Ts...>> : std::tuple_size<const std::tuple<Ts...>>
    {
    };

    /**
     * @brief Specialization for volatile fxt::tuple
     */
    template<typename... Ts>
    struct tuple_size<volatile tuple<Ts...>> : std::tuple_size<volatile std::tuple<Ts...>>
    {
    };

    /**
     * @brief Specialization for const volatile fxt::tuple
     */
    template<typename... Ts>
    struct tuple_size<const volatile tuple<Ts...>> : std::tuple_size<const volatile std::tuple<Ts...>>
    {
    };

    /**
     * @brief Specialization for fxt::flat_tuple
     *
     * Provides the tuple size as the number of template parameters.
     */
    template<typename... Ts>
    struct tuple_size<flat_tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    /**
     * @brief Specialization for const fxt::flat_tuple
     */
    template<typename... Ts>
    struct tuple_size<const flat_tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    /**
     * @brief Specialization for volatile fxt::flat_tuple
     */
    template<typename... Ts>
    struct tuple_size<volatile flat_tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    /**
     * @brief Specialization for const volatile fxt::flat_tuple
     */
    template<typename... Ts>
    struct tuple_size<const volatile flat_tuple<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)>
    {
    };

    /**
     * @brief Helper variable template for getting tuple size
     *
     * Provides a convenient way to get the size of a tuple type.
     * Works with both fxt::tuple and fxt::flat_tuple.
     *
     * @tparam T The tuple type
     *
     * Usage:
     * @code
     * constexpr std::size_t size1 = fxt::tuple_size_v<fxt::tuple<int, double>>;  // 2
     * constexpr std::size_t size2 = fxt::tuple_size_v<fxt::flat_tuple<int, std::string, double>>;  // 3
     * @endcode
     */
    template<typename T>
    inline constexpr std::size_t tuple_size_v = tuple_size<T>::value;

}    // namespace fxt
