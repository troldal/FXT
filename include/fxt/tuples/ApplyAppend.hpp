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
 * @file ApplyAppend.hpp
 * @brief Apply and append operations for tuples
 *
 * This file provides functions for applying a function to tuple elements and appending
 * the result to the original tuple. Works with both fxt::tuple and fxt::flat_tuple,
 * in both plain and monadic contexts.
 *
 * ## Main Functions
 *
 * ### fxt::apply_append
 * Applies a function to tuple elements and appends the result to the tuple.
 * - **Direct call**: `apply_append(function, tuple)` - Takes a function and tuple
 * - **Curried form**: `apply_append(function)` - Returns a callable for pipeline usage with operator|
 *
 * ### fxt::mapply_append
 * Applies a function to tuple elements inside a monad and appends the result.
 * Works with fxt::expected and fxt::optional containers.
 * - **Direct call**: `mapply_append(function, monad<tuple>)` - Takes a function and monadic tuple
 * - **Curried form**: `mapply_append(function)` - Returns a callable for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mapply_append handles error/nullopt propagation automatically
 * - Handles void returns: Functions returning void don't append anything
 * - Monadic returns: Functions returning monadic types are properly handled
 *
 * ## Examples
 *
 * ### Basic apply_append
 * @code
 * auto t = fxt::tuple{3, 4};
 * auto result = fxt::apply_append([](int a, int b) { return a + b; }, t);
 * // result is fxt::tuple{3, 4, 7}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{2, 3}
 *     | fxt::apply_append([](int a, int b) { return a * b; })
 *     | fxt::apply_append([](int a, int b, int c) { return a + b + c; });
 * // result is fxt::tuple{2, 3, 6, 11}
 * @endcode
 *
 * ### Monadic apply_append
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{10, 5}};
 * auto result = exp | fxt::mapply_append([](int a, int b) { return a / b; });
 * // result is fxt::expected<fxt::tuple<int, int, int>, Error>{10, 5, 2}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
 * auto result = ft | fxt::apply_append([](double a, double b) { return a + b; });
 * // result is fxt::flat_tuple<double, double, double>{2.0, 3.0, 5.0}
 * @endcode
 *
 * @see fxt::apply
 * @see fxt::mapply
 * @see fxt::tuple_append
 * @see fxt::tuple
 * @see fxt::flat_tuple
 */

#pragma once

#include "Apply.hpp"
#include "TupleAppend.hpp"
#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include "../utils/Unit.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::apply_append - Direct apply and append function for tuples
    // ========================================================================

    /**
     * @brief Apply a function to tuple elements and append the result
     *
     * Applies the given function to the elements of the tuple and appends
     * the result to the original tuple, returning the extended tuple.
     * If the function returns void, the tuple is returned unchanged.
     *
     * @tparam F Function type
     * @tparam Ts Tuple element types
     * @param f Function to apply
     * @param t Tuple whose elements are used as function arguments
     * @return Tuple with original elements plus the function result (or unchanged if void)
     */
    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, tuple<Ts...>& t)
    {
        using result_type = std::invoke_result_t<F, Ts&...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return t;
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(t, std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, const tuple<Ts...>& t)
    {
        using result_type = std::invoke_result_t<F, const Ts&...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return t;
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(t, std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, tuple<Ts...>&& t)
    {
        using result_type = std::invoke_result_t<F, Ts...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return std::move(t);
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(std::move(t), std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, const tuple<Ts...>&& t)
    {
        using result_type = std::invoke_result_t<F, const Ts...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return std::move(t);
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(std::move(t), std::move(result));
        }
    }

    /**
     * @brief Apply a function to flat_tuple elements and append the result
     */
    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, flat_tuple<Ts...>& t)
    {
        using result_type = std::invoke_result_t<F, Ts&...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return t;
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(t, std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, const flat_tuple<Ts...>& t)
    {
        using result_type = std::invoke_result_t<F, const Ts&...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return t;
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(t, std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, flat_tuple<Ts...>&& t)
    {
        using result_type = std::invoke_result_t<F, Ts...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return std::move(t);
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(std::move(t), std::move(result));
        }
    }

    template<typename F, typename... Ts>
    constexpr auto apply_append(F&& f, const flat_tuple<Ts...>&& t)
    {
        using result_type = std::invoke_result_t<F, const Ts...>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), t);
            return std::move(t);
        } else {
            auto result = fxt::apply(std::forward<F>(f), t);
            return fxt::tuple_append(std::move(t), std::move(result));
        }
    }

    // ========================================================================
    // Curried versions of fxt::apply_append for use with pipe operator
    // ========================================================================

    /**
     * @brief Curried version of apply_append for use with pipe operator
     */
    template<typename F>
    struct apply_append_curried
    {
        F func;

        // Overloads for fxt::tuple
        template<typename... Ts>
        constexpr auto operator()(tuple<Ts...>& t) const
        {
            return fxt::apply_append(func, t);
        }

        template<typename... Ts>
        constexpr auto operator()(const tuple<Ts...>& t) const
        {
            return fxt::apply_append(func, t);
        }

        template<typename... Ts>
        constexpr auto operator()(tuple<Ts...>&& t) const
        {
            return fxt::apply_append(func, std::move(t));
        }

        template<typename... Ts>
        constexpr auto operator()(const tuple<Ts...>&& t) const
        {
            return fxt::apply_append(func, std::move(t));
        }

        // Overloads for fxt::flat_tuple
        template<typename... Ts>
        constexpr auto operator()(flat_tuple<Ts...>& t) const
        {
            return fxt::apply_append(func, t);
        }

        template<typename... Ts>
        constexpr auto operator()(const flat_tuple<Ts...>& t) const
        {
            return fxt::apply_append(func, t);
        }

        template<typename... Ts>
        constexpr auto operator()(flat_tuple<Ts...>&& t) const
        {
            return fxt::apply_append(func, std::move(t));
        }

        template<typename... Ts>
        constexpr auto operator()(const flat_tuple<Ts...>&& t) const
        {
            return fxt::apply_append(func, std::move(t));
        }
    };

    // TODO: NAMING — per the fxt/tuples convention in Tuple.hpp, rename the
    //       fxt-original ops to carry the `tuple_` prefix: apply_append ->
    //       tuple_apply_append and mapply_append -> mtuple_apply_append (keep old
    //       names as [[deprecated]] aliases for one release).
    /**
     * @brief Create a curried apply_append function for pipe operator usage
     */
    template<typename F>
    constexpr auto apply_append(F&& f)
    {
        return apply_append_curried<std::decay_t<F>>{std::forward<F>(f)};
    }

    // ========================================================================
    // fxt::mapply_append - Monadic apply and append
    // ========================================================================

    /**
     * @brief Wrapper struct for monadic apply_append operations
     */
    template<typename TFunction>
    struct apply_append_wrapper
    {
        TFunction function;

        // ========================================================================
        // Case 1a: Optional-like container + tuple + Function returning monadic type
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires optional_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (returns_monadic_with_tuple<TFunction, std::remove_cvref_t<TTuple>>)
        auto operator()(TArg&& tupleContainer) const
        {
            return std::forward<TArg>(tupleContainer).and_then([this](const TTuple& tuple) {
                auto result = fxt::apply(function, tuple);
                return result.transform([&tuple](auto&& _value) {
                    return fxt::tuple_append(tuple, std::forward<decltype(_value)>(_value));
                });
            });
        }

        // ========================================================================
        // Case 1b: Expected-like container + tuple + Function returning monadic type
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires expected_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (returns_monadic_with_tuple<TFunction, std::remove_cvref_t<TTuple>>)
        auto operator()(TArg&& tupleExpected) const
        {
            return std::forward<TArg>(tupleExpected).and_then([this](const TTuple& tuple) {
                auto result = fxt::apply(function, tuple);
                return result.transform([&tuple](auto&& _value) {
                    return fxt::tuple_append(tuple, std::forward<decltype(_value)>(_value));
                });
            });
        }

        // ========================================================================
        // Case 2a: Optional-like container + tuple + Function returning void
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires optional_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (std::same_as<invoke_result_with_tuple_t<TFunction, std::remove_cvref_t<TTuple>>, void>)
        auto operator()(TArg&& opt) const
        {
            return std::forward<TArg>(opt).transform([this](const TTuple& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 2b: Expected-like container + tuple + Function returning void
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires expected_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (std::same_as<invoke_result_with_tuple_t<TFunction, std::remove_cvref_t<TTuple>>, void>)
        auto operator()(TArg&& tupleExpected) const
        {
            return std::forward<TArg>(tupleExpected).transform([this](const TTuple& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 3a: Optional-like container + tuple + Function returning regular value
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires optional_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (!returns_monadic_with_tuple<TFunction, std::remove_cvref_t<TTuple>>)
                && (!std::same_as<invoke_result_with_tuple_t<TFunction, std::remove_cvref_t<TTuple>>, void>)
        auto operator()(TArg&& opt) const
        {
            return std::forward<TArg>(opt).transform([this](const TTuple& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }

        // ========================================================================
        // Case 3b: Expected-like container + tuple + Function returning regular value
        // ========================================================================
        template<typename TArg, typename TTuple = typename std::remove_cvref_t<TArg>::value_type>
            requires expected_like<std::remove_cvref_t<TArg>>
                && tuple_like<std::remove_cvref_t<TTuple>>
                && (!returns_monadic_with_tuple<TFunction, std::remove_cvref_t<TTuple>>)
                && (!std::same_as<invoke_result_with_tuple_t<TFunction, std::remove_cvref_t<TTuple>>, void>)
        auto operator()(TArg&& tupleExpected) const
        {
            return std::forward<TArg>(tupleExpected).transform([this](const TTuple& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }
    };

    /**
     * @brief Monadic apply_append operation for applying functions to tuple values in monadic containers
     *
     * This function provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in a monadic container and appending the result to the original tuple.
     * It handles three distinct cases:
     * 1. Functions returning monadic types - the result is unwrapped before appending
     * 2. Functions with void return type - nothing is appended
     * 3. Functions returning any other type - the result is appended
     *
     * @tparam TFunction Type of the function to apply
     * @param f Function to apply to tuple elements
     * @return A callable that accepts a monadic container with a tuple
     *
     * @section Usage
     * @code
     * // With expected
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::mapply_append([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int, int, int>, Error>{std::tuple{3, 4, 7}}
     *
     * // With optional
     * auto result2 = fxt::optional<std::tuple<int, int>>{std::tuple{5, 6}}
     *              | fxt::mapply_append([](int a, int b) { return a * b; });
     * // result2 is fxt::optional<std::tuple<int, int, int>>{std::tuple{5, 6, 30}}
     *
     * // Chain multiple applications
     * auto result3 = fxt::expected<std::tuple<int, int>, Error>{std::tuple{2, 3}}
     *              | fxt::mapply_append([](int a, int b) { return a + b; })
     *              | fxt::mapply_append([](int a, int b, int sum) { return sum * 2; });
     * // result3 is fxt::expected<std::tuple<int, int, int, int>, Error>{std::tuple{2, 3, 5, 10}}
     * @endcode
     */
    struct mapply_append_fn
    {
        template<typename TFunction>
        constexpr auto operator()(TFunction&& function) const
        {
            return apply_append_wrapper<std::decay_t<TFunction>>{std::forward<TFunction>(function)};
        }
    };

    inline constexpr mapply_append_fn mapply_append{};

}    // namespace fxt

