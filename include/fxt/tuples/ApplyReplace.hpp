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
 * @file ApplyReplace.hpp
 * @brief Apply and replace operations for tuples
 *
 * This file provides functions for applying a function to tuple elements and replacing
 * all elements with the result. Works with both fxt::tuple and fxt::flat_tuple,
 * in both plain and monadic contexts.
 *
 * ## Main Functions
 *
 * ### fxt::apply_replace
 * Applies a function to tuple elements and replaces all elements with the result.
 * - **Direct call**: `apply_replace(function, tuple)` - Takes a function and tuple
 * - **Curried form**: `apply_replace(function)` - Returns a callable for pipeline usage with operator|
 *
 * ### fxt::mapply_replace
 * Applies a function to tuple elements inside a monad and replaces all elements with the result.
 * Works with fxt::expected and fxt::optional containers.
 * - **Direct call**: `mapply_replace(function, monad<tuple>)` - Takes a function and monadic tuple
 * - **Curried form**: `mapply_replace(function)` - Returns a callable for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Preserves fxt::tuple vs fxt::flat_tuple distinction
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mapply_replace handles error/nullopt propagation automatically
 * - Handles void returns: Functions returning void result in an empty tuple
 * - Monadic returns: Functions returning monadic types are properly handled
 *
 * ## Examples
 *
 * ### Basic apply_replace
 * @code
 * auto t = fxt::tuple{3, 4};
 * auto result = fxt::apply_replace([](int a, int b) { return a + b; }, t);
 * // result is fxt::tuple{7}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{2, 3}
 *     | fxt::apply_replace([](int a, int b) { return a * b; })
 *     | fxt::apply_replace([](int product) { return product + 10; });
 * // result is fxt::tuple{16}
 * @endcode
 *
 * ### Monadic apply_replace
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{10, 5}};
 * auto result = exp | fxt::mapply_replace([](int a, int b) { return a / b; });
 * // result is fxt::expected<fxt::tuple<int>, Error>{2}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
 * auto result = ft | fxt::apply_replace([](double a, double b) { return a + b; });
 * // result is fxt::flat_tuple<double>{5.0}
 * @endcode
 *
 * @see fxt::apply
 * @see fxt::mapply
 * @see fxt::apply_append
 * @see fxt::tuple
 * @see fxt::flat_tuple
 */

#pragma once

#include "Apply.hpp"
#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include "Drop.hpp"
#include "Take.hpp"
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include "../utils/Unit.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::apply_replace - Direct apply and replace function for tuples
    // ========================================================================

    /**
     * @brief Apply a function to tuple elements and replace with the result
     *
     * Applies the given function to the elements of the tuple and returns
     * a new tuple containing only the result.
     *
     * @tparam F Function type
     * @tparam Ts Tuple element types
     * @param f Function to apply
     * @param t Tuple whose elements are used as function arguments
     * @return Tuple containing only the function result
     */
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, tuple<Ts...>& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), t);
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, const tuple<Ts...>& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), t);
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, tuple<Ts...>&& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), std::move(t));
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, const tuple<Ts...>&& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), std::move(t));
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_tuple(std::move(result));
    // }
    //
    // /**
    //  * @brief Apply a function to flat_tuple elements and replace with the result
    //  */
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, flat_tuple<Ts...>& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), t);
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_flat_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, const flat_tuple<Ts...>& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), t);
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_flat_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, flat_tuple<Ts...>&& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), std::move(t));
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_flat_tuple(std::move(result));
    // }
    //
    // template<typename F, typename... Ts>
    // constexpr auto apply_replace(F&& f, const flat_tuple<Ts...>&& t)
    // {
    //     auto result = fxt::apply_append(std::forward<F>(f), std::move(t));
    //     return fxt::take_last<1>(result);
    //     //return fxt::make_flat_tuple(std::move(result));
    // }

    template<typename F, typename TTuple>
    constexpr auto apply_replace(F&& f, TTuple&& t)
    {
             auto result = fxt::apply_append(std::forward<F>(f), std::forward<TTuple>(t));
             return fxt::take_last<1>(result);
    }


    // ========================================================================
    // Curried versions of fxt::apply_replace for use with pipe operator
    // ========================================================================

    /**
     * @brief Curried version of apply_replace for use with pipe operator
     */
    template<typename F>
    struct apply_replace_curried
    {
        F func;

        // Overloads for fxt::tuple
        // template<typename... Ts>
        // constexpr auto operator()(tuple<Ts...>& t) const
        // {
        //     return fxt::apply_replace(func, t);
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(const tuple<Ts...>& t) const
        // {
        //     return fxt::apply_replace(func, t);
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(tuple<Ts...>&& t) const
        // {
        //     return fxt::apply_replace(func, std::move(t));
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(const tuple<Ts...>&& t) const
        // {
        //     return fxt::apply_replace(func, std::move(t));
        // }
        //
        // // Overloads for fxt::flat_tuple
        // template<typename... Ts>
        // constexpr auto operator()(flat_tuple<Ts...>& t) const
        // {
        //     return fxt::apply_replace(func, t);
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(const flat_tuple<Ts...>& t) const
        // {
        //     return fxt::apply_replace(func, t);
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(flat_tuple<Ts...>&& t) const
        // {
        //     return fxt::apply_replace(func, std::move(t));
        // }
        //
        // template<typename... Ts>
        // constexpr auto operator()(const flat_tuple<Ts...>&& t) const
        // {
        //     return fxt::apply_replace(func, std::move(t));
        // }

        template<typename TTuple>
        constexpr auto operator()(TTuple&& t) const
        {
            return fxt::apply_replace(func, std::forward<TTuple>(t));
        }
    };

    // TODO: NAMING — per the fxt/tuples convention in Tuple.hpp, rename the
    //       fxt-original ops to carry the `tuple_` prefix: apply_replace ->
    //       tuple_apply_replace and mapply_replace -> mtuple_apply_replace (keep
    //       old names as [[deprecated]] aliases for one release).
    /**
     * @brief Create a curried apply_replace function for pipe operator usage
     */
    template<typename F>
    constexpr auto apply_replace(F&& f)
    {
        return apply_replace_curried<std::decay_t<F>>{std::forward<F>(f)};
    }

    // ========================================================================
    // fxt::mapply_replace - Monadic apply and replace
    // ========================================================================

    /**
     * @brief Wrapper struct for monadic apply_replace operations
     */
    template<typename TFunction>
    struct apply_replace_wrapper
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
            return std::forward<TArg>(tupleContainer).and_then([this](auto&& tuple) {
                auto result = fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                return result.transform([](auto&& _value) {
                    // Preserve tuple type
                    if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                        return fxt::make_tuple(std::forward<decltype(_value)>(_value));
                    } else {
                        return fxt::make_flat_tuple(std::forward<decltype(_value)>(_value));
                    }
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
            return std::forward<TArg>(tupleExpected).and_then([this](auto&& tuple) {
                auto result = fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                return result.transform([](auto&& _value) {
                    // Preserve tuple type
                    if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                        return fxt::make_tuple(std::forward<decltype(_value)>(_value));
                    } else {
                        return fxt::make_flat_tuple(std::forward<decltype(_value)>(_value));
                    }
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
            return std::forward<TArg>(opt).transform([this](auto&& tuple) {
                fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                // Return empty tuple of the appropriate type
                if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                    return fxt::tuple<>{};
                } else {
                    return fxt::flat_tuple<>{};
                }
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
            return std::forward<TArg>(tupleExpected).transform([this](auto&& tuple) {
                fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                // Return empty tuple of the appropriate type
                if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                    return fxt::tuple<>{};
                } else {
                    return fxt::flat_tuple<>{};
                }
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
            return std::forward<TArg>(opt).transform([this](auto&& tuple) {
                auto result = fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                // Preserve tuple type
                if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                    return fxt::make_tuple(std::move(result));
                } else {
                    return fxt::make_flat_tuple(std::move(result));
                }
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
            return std::forward<TArg>(tupleExpected).transform([this](auto&& tuple) {
                auto result = fxt::apply(function, std::forward<decltype(tuple)>(tuple));
                // Preserve tuple type
                if constexpr (impl::is_fxt_tuple_v<TTuple>) {
                    return fxt::make_tuple(std::move(result));
                } else {
                    return fxt::make_flat_tuple(std::move(result));
                }
            });
        }
    };

    /**
     * @brief Monadic apply_replace operation for applying functions to tuple values in monadic containers
     *
     * This function provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in a monadic container and replacing all elements with the result.
     * It handles three distinct cases:
     * 1. Functions returning monadic types - the result is unwrapped before creating the new tuple
     * 2. Functions with void return type - returns an empty tuple
     * 3. Functions returning any other type - the result becomes the only element
     *
     * @tparam TFunction Type of the function to apply
     * @param f Function to apply to tuple elements
     * @return A callable that accepts a monadic container with a tuple
     *
     * @section Usage
     * @code
     * // With expected
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::mapply_replace([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int>, Error>{std::tuple{7}}
     *
     * // With optional
     * auto result2 = fxt::optional<std::tuple<int, int>>{std::tuple{5, 6}}
     *              | fxt::mapply_replace([](int a, int b) { return a * b; });
     * // result2 is fxt::optional<std::tuple<int>>{std::tuple{30}}
     *
     * // Chain multiple applications
     * auto result3 = fxt::expected<std::tuple<int, int>, Error>{std::tuple{2, 3}}
     *              | fxt::mapply_replace([](int a, int b) { return a + b; })
     *              | fxt::mapply_replace([](int sum) { return sum * 2; });
     * // result3 is fxt::expected<std::tuple<int>, Error>{std::tuple{10}}
     * @endcode
     */
    struct mapply_replace_fn
    {
        template<typename TFunction>
        constexpr auto operator()(TFunction&& function) const
        {
            return apply_replace_wrapper<std::decay_t<TFunction>>{std::forward<TFunction>(function)};
        }
    };

    inline constexpr mapply_replace_fn mapply_replace{};

}    // namespace fxt

