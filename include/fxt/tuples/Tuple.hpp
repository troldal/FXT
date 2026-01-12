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
 * @file Tuple.hpp
 * @brief Core tuple type and operations for the FXT library
 *
 * This file provides the foundational tuple type and operations for the FXT library.
 * It serves as a thin wrapper around std::tuple with additional FXT-specific functionality,
 * enabling functional-style operations and pipeline composition.
 *
 * ## Main Components
 *
 * **fxt::tuple<Ts...>** - Alias for std::tuple providing consistent FXT namespace interface
 * - Allows writing `fxt::tuple` instead of `std::tuple`
 * - Compatible with all standard library tuple operations
 *
 * **fxt::make_tuple(args...)** - Factory function to create tuples with type deduction
 * - Forwards to std::make_tuple with perfect forwarding
 * - Provides consistent FXT interface
 *
 * ## Pipe Operators
 *
 * Defines pipe operators (`operator|`) enabling functional-style composition. Supports all
 * value categories for maximum flexibility:
 * - `tuple& | callable` - Lvalue reference
 * - `const tuple& | callable` - Const lvalue reference
 * - `tuple&& | callable` - Rvalue reference (move semantics)
 * - `const tuple&& | callable` - Const rvalue reference
 *
 * ## Key Features
 * - Consistent namespace interface across FXT library
 * - Perfect forwarding support for all operations
 * - Pipeline-friendly design with pipe operator support
 * - Universal value category support (lvalue, rvalue, const)
 * - Foundation for higher-level tuple operations
 *
 * ## Examples
 *
 * @code
 * // Creating tuples
 * fxt::tuple<int, double, std::string> t1{42, 3.14, "hello"};
 * auto t2 = fxt::make_tuple(42, 3.14, "hello");
 *
 * // Using pipe operator
 * auto t = fxt::make_tuple(1, 2, 3);
 * auto result = t | fxt::tuple_reverse();
 * // result is fxt::tuple<int, int, int>{3, 2, 1}
 *
 * // Chaining operations
 * auto result2 = t
 *     | fxt::tuple_transform([](auto x) { return x * 2; })
 *     | fxt::take<2>();
 * // result2 is fxt::tuple<int, int>{2, 4}
 *
 * // Pipeline with rvalue
 * auto result3 = fxt::make_tuple(1, 2, 3, 4, 5)
 *     | fxt::drop<2>()
 *     | fxt::tuple_reverse()
 *     | fxt::take<2>();
 * // result3 is fxt::tuple<int, int>{5, 4}
 * @endcode
 *
 * @see fxt::flat_tuple
 * @see fxt::tuple_transform
 * @see fxt::tuple_reverse
 */

#pragma once

#include <concepts>
#include <functional>
#include <tuple>

// namespace fxt
// {
//     /**
//      * @brief Alias for std::tuple
//      *
//      * Provides a consistent fxt namespace interface for tuple types,
//      * allowing users to write fxt::tuple instead of std::tuple.
//      */
//     template<typename... Ts>
//     using tuple = std::tuple<Ts...>;
//
//     /**
//      * @brief Create a tuple, deducing the target type from the types of arguments
//      *
//      * Forwards to std::make_tuple. Provides a consistent fxt::make_tuple
//      * interface that matches fxt::tuple.
//      *
//      * @tparam Ts Types of the elements
//      * @param args Values to initialize the tuple with
//      * @return A tuple containing the given values
//      *
//      * @code
//      * auto t = fxt::make_tuple(42, 3.14, "hello");
//      * // Creates fxt::tuple<int, double, const char*>
//      * @endcode
//      */
//     template<typename... Ts>
//     constexpr auto make_tuple(Ts&&... args)
//     {
//         return std::make_tuple(std::forward<Ts>(args)...);
//     }
//
// }    // namespace fxt
//

// In Tuple.hpp
namespace fxt
{
    /**
     * @brief Wrapper around std::tuple providing FXT namespace interface
     *
     * This is a distinct type (not an alias) to enable proper ADL lookup
     * for fxt::get, allowing structured bindings to work correctly.
     */
    // template<typename... Ts>
    // struct tuple : private std::tuple<Ts...>
    // {
    //     using base = std::tuple<Ts...>;
    //     using base::base;  // Inherit constructors
    //
    //     // Allow implicit conversion from std::tuple
    //     constexpr tuple(const std::tuple<Ts...>& t) : base(t) {}
    //     constexpr tuple(std::tuple<Ts...>&& t) : base(std::move(t)) {}
    //
    //     // Allow implicit conversion to std::tuple
    //     constexpr operator const std::tuple<Ts...>&() const& noexcept { return *this; }
    //     constexpr operator std::tuple<Ts...>&() & noexcept { return *this; }
    //     constexpr operator std::tuple<Ts...>&&() && noexcept { return std::move(*this); }
    //
    //     // Access to underlying tuple
    //     constexpr const base& as_std_tuple() const& noexcept { return *this; }
    //     constexpr base& as_std_tuple() & noexcept { return *this; }
    //     constexpr base&& as_std_tuple() && noexcept { return std::move(*this); }
    //
    //     // For structured bindings support
    //     template<std::size_t I>
    //     friend constexpr auto& get(tuple& t) noexcept { return std::get<I>(static_cast<base&>(t)); }
    //
    //     template<std::size_t I>
    //     friend constexpr const auto& get(const tuple& t) noexcept { return std::get<I>(static_cast<const base&>(t)); }
    //
    //     template<std::size_t I>
    //     friend constexpr auto&& get(tuple&& t) noexcept { return std::get<I>(static_cast<base&&>(t)); }
    //
    //     template<std::size_t I>
    //     friend constexpr const auto&& get(const tuple&& t) noexcept { return std::get<I>(static_cast<const base&&>(t)); }
    // };


    // In Tuple.hpp - change private to public inheritance
    template<typename... Ts>
    struct tuple : public std::tuple<Ts...>  // Changed from private to public
    {
        using base = std::tuple<Ts...>;
        using base::base;  // Inherit constructors

        // Allow implicit conversion from std::tuple
        constexpr tuple(const std::tuple<Ts...>& t) : base(t) {}
        constexpr tuple(std::tuple<Ts...>&& t) : base(std::move(t)) {}

        // Allow implicit conversion to std::tuple
        constexpr operator const std::tuple<Ts...>&() const& noexcept {
            return static_cast<const base&>(*this);
        }
        constexpr operator std::tuple<Ts...>&() & noexcept {
            return static_cast<base&>(*this);
        }
        constexpr operator std::tuple<Ts...>&&() && noexcept {
            return static_cast<base&&>(*this);
        }

        // For structured bindings support - these find fxt::get via ADL
        template<std::size_t I>
        friend constexpr auto& get(tuple& t) noexcept { return std::get<I>(static_cast<base&>(t)); }

        template<std::size_t I>
        friend constexpr const auto& get(const tuple& t) noexcept { return std::get<I>(static_cast<const base&>(t)); }

        template<std::size_t I>
        friend constexpr auto&& get(tuple&& t) noexcept { return std::get<I>(static_cast<base&&>(t)); }

        template<std::size_t I>
        friend constexpr const auto&& get(const tuple&& t) noexcept { return std::get<I>(static_cast<const base&&>(t)); }
    };


    // Deduction guide
    template<typename... Ts>
    tuple(Ts...) -> tuple<Ts...>;

    template<typename... Ts>
    constexpr auto make_tuple(Ts&&... args)
    {
        return tuple<std::decay_t<Ts>...>(std::forward<Ts>(args)...);
    }

}

// Structured bindings support
template<typename... Ts>
struct std::tuple_size<fxt::tuple<Ts...>> : std::tuple_size<std::tuple<Ts...>> {};

template<std::size_t I, typename... Ts>
struct std::tuple_element<I, fxt::tuple<Ts...>> : std::tuple_element<I, std::tuple<Ts...>> {};