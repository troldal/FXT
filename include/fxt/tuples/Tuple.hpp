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
 * ## Main Types
 *
 * ### fxt::tuple
 * An alias for std::tuple that provides a consistent FXT namespace interface.
 * This allows users to write `fxt::tuple` instead of `std::tuple`, maintaining
 * consistency across the FXT library while leveraging all standard tuple functionality.
 *
 * ## Main Functions
 *
 * ### fxt::make_tuple
 * Factory function to create a tuple with type deduction from arguments.
 * This is a forwarding wrapper around `std::make_tuple` that provides a consistent
 * FXT interface. All arguments are perfectly forwarded to maintain value categories.
 *
 * ## Pipe Operators
 *
 * This file defines pipe operators (`operator|`) for `fxt::tuple` that enable
 * functional-style composition. The pipe operators allow tuples to be passed to callables,
 * supporting all value categories for maximum flexibility.
 *
 * ### Pipe Operator Overloads
 * - **Lvalue reference**: `tuple& | callable` - Pipes a mutable lvalue tuple to a callable
 * - **Const lvalue reference**: `const tuple& | callable` - Pipes a const lvalue tuple to a callable
 * - **Rvalue reference**: `tuple&& | callable` - Pipes an rvalue tuple to a callable (move semantics)
 * - **Const rvalue reference**: `const tuple&& | callable` - Pipes a const rvalue tuple to a callable
 *
 * The pipe operators use `std::invoke` to call the callable with the tuple, supporting
 * function pointers, function objects, lambdas, and member function pointers.
 *
 * ## Key Features
 * - Consistent namespace interface across the FXT library
 * - Perfect forwarding support for all operations
 * - Pipeline-friendly design with pipe operator support
 * - Universal value category support (lvalue, rvalue, const)
 * - Compatible with standard library tuple operations
 * - Foundation for higher-level tuple operations (transform, reverse, append, etc.)
 *
 * ## Examples
 *
 * ### Creating tuples
 * @code
 * // Direct construction
 * fxt::tuple<int, double, std::string> t1{42, 3.14, "hello"};
 *
 * // Using make_tuple with type deduction
 * auto t2 = fxt::make_tuple(42, 3.14, "hello");
 * // Type: fxt::tuple<int, double, const char*>
 * @endcode
 *
 * ### Using the pipe operator
 * @code
 * auto t = fxt::make_tuple(1, 2, 3);
 *
 * // Pipe to a transformation function
 * auto result = t | fxt::tuple_reverse();
 * // result is fxt::tuple<int, int, int>{3, 2, 1}
 *
 * // Chain multiple operations
 * auto result2 = t
 *     | fxt::tuple_transform([](auto x) { return x * 2; })
 *     | fxt::take<2>();
 * // result2 is fxt::tuple<int, int>{2, 4}
 *
 * // Pipe with rvalue
 * auto result3 = fxt::make_tuple(5, 10, 15)
 *     | fxt::tuple_reverse();
 * // result3 is fxt::tuple<int, int, int>{15, 10, 5}
 * @endcode
 *
 * ### Chaining operations
 * @code
 * auto t = fxt::make_tuple(1, 2, 3, 4, 5);
 * auto result = t
 *     | fxt::drop<2>()
 *     | fxt::tuple_reverse()
 *     | fxt::take<2>();
 * // result is fxt::tuple<int, int>{5, 4}
 * @endcode
 *
 * ## Type Summary
 *
 * | Type | Description |
 * |------|-------------|
 * | `fxt::tuple<Ts...>` | Alias for std::tuple, provides consistent FXT interface |
 *
 * ## Function Summary
 *
 * | Function | Description |
 * |----------|-------------|
 * | `fxt::make_tuple(args...)` | Create a tuple with type deduction from arguments |
 *
 * ## Operator Summary
 *
 * | Operator | Description |
 * |----------|-------------|
 * | `tuple& \| callable` | Pipe lvalue tuple to callable |
 * | `const tuple& \| callable` | Pipe const lvalue tuple to callable |
 * | `tuple&& \| callable` | Pipe rvalue tuple to callable |
 * | `const tuple&& \| callable` | Pipe const rvalue tuple to callable |
 *
 * @see fxt::flat_tuple
 * @see fxt::tuple_transform
 * @see fxt::tuple_reverse
 * @see fxt::tuple_append
 * @see fxt::tuple_size
 * @see fxt::get
 */

#pragma once

#include <concepts>
#include <functional>
#include <tuple>

namespace fxt
{
    /**
     * @brief Alias for std::tuple
     *
     * Provides a consistent fxt namespace interface for tuple types,
     * allowing users to write fxt::tuple instead of std::tuple.
     */
    template<typename... Ts>
    using tuple = std::tuple<Ts...>;

    /**
     * @brief Create a tuple, deducing the target type from the types of arguments
     *
     * Forwards to std::make_tuple. Provides a consistent fxt::make_tuple
     * interface that matches fxt::tuple.
     *
     * @tparam Ts Types of the elements
     * @param args Values to initialize the tuple with
     * @return A tuple containing the given values
     *
     * @code
     * auto t = fxt::make_tuple(42, 3.14, "hello");
     * // Creates fxt::tuple<int, double, const char*>
     * @endcode
     */
    template<typename... Ts>
    constexpr auto make_tuple(Ts&&... args)
    {
        return std::make_tuple(std::forward<Ts>(args)...);
    }

}    // namespace fxt

/**
 * @brief Pipe operator for fxt::tuple with callable (lvalue reference)
 *
 * Allows piping a tuple to a callable function, enabling functional-style composition.
 * The callable receives the tuple and returns the result.
 *
 * @tparam Ts Types in the tuple
 * @tparam Callable Type of the callable
 * @param tuple The tuple to pipe
 * @param callable The callable to apply to the tuple
 * @return The result of invoking the callable with the tuple
 *
 * @code
 * fxt::tuple<int, double> t{42, 3.14};
 * auto result = t | fxt::get<0>;  // Returns 42
 * @endcode
 */
template<typename... Ts, typename Callable>
    requires requires(fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
constexpr auto operator|(fxt::tuple<Ts...>& tuple, Callable&& callable) -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
{
    return std::invoke(std::forward<Callable>(callable), tuple);
}

/**
 * @brief Pipe operator for fxt::tuple with callable (const lvalue reference)
 */
template<typename... Ts, typename Callable>
    requires requires(const fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
constexpr auto operator|(const fxt::tuple<Ts...>& tuple, Callable&& callable)
    -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
{
    return std::invoke(std::forward<Callable>(callable), tuple);
}

/**
 * @brief Pipe operator for fxt::tuple with callable (rvalue reference)
 */
template<typename... Ts, typename Callable>
    requires requires(fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
constexpr auto operator|(fxt::tuple<Ts...>&& tuple, Callable&& callable)
    -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
{
    return std::invoke(std::forward<Callable>(callable), std::move(tuple));
}

/**
 * @brief Pipe operator for fxt::tuple with callable (const rvalue reference)
 */
template<typename... Ts, typename Callable>
    requires requires(const fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
constexpr auto operator|(const fxt::tuple<Ts...>&& tuple, Callable&& callable)
    -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
{
    return std::invoke(std::forward<Callable>(callable), std::move(tuple));
}
