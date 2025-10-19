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

//#include "../monads/Expected.hpp"
#include <concepts>
#include <type_traits>

namespace fxt
{
    /**
     * @brief Concept to check if a type behaves like an expected type
     *
     * This concept verifies both structural requirements (type aliases) and
     * behavioral requirements (core operations) of an expected-like type.
     * It ensures the type has the necessary members and operations to be
     * used in monadic contexts.
     */
    template<typename T>
    concept expected_like = requires(T t) {
        // Required type aliases
        typename T::value_type;
        typename T::error_type;
        typename T::unexpected_type;

        // Core expected-like operations
        { t.has_value() } -> std::convertible_to<bool>;
        { static_cast<bool>(t) } -> std::convertible_to<bool>;

        // Value access operations (just check they exist, don't require convertibility for move-only types)
        t.value();
        t.error();
        *t;

        // Optional: Monadic operations (uncomment if you want to enforce these)
        // { t.transform(std::declval<std::function<int(typename T::value_type)>>()) };
        // { t.and_then(std::declval<std::function<T(typename T::value_type)>>()) };
    } && std::is_same_v<std::decay_t<T>, T>; // Ensure we work with decayed types

}    // namespace fxt
