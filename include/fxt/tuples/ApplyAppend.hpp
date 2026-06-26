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
 * ### fxt::tuple_apply_append
 * Applies a function to tuple elements and appends the result to the tuple.
 * - **Direct call**: `tuple_apply_append(function, tuple)` - Takes a function and tuple
 * - **Curried form**: `tuple_apply_append(function)` - Returns a callable for pipeline usage with operator|
 *
 * ### fxt::mtuple_apply_append
 * Applies a function to tuple elements inside a monad and appends the result.
 * Works with fxt::expected and fxt::optional containers.
 * - **Curried form**: `mtuple_apply_append(function)` - Returns a callable for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_apply_append handles error/nullopt propagation automatically
 * - Handles void returns: Functions returning void don't append anything
 * - Monadic returns: Functions returning monadic types are properly handled
 *
 * ## Examples
 *
 * ### Basic tuple_apply_append
 * @code
 * auto t = fxt::tuple{3, 4};
 * auto result = fxt::tuple_apply_append([](int a, int b) { return a + b; }, t);
 * // result is fxt::tuple{3, 4, 7}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{2, 3}
 *     | fxt::tuple_apply_append([](int a, int b) { return a * b; })
 *     | fxt::tuple_apply_append([](int a, int b, int c) { return a + b + c; });
 * // result is fxt::tuple{2, 3, 6, 11}
 * @endcode
 *
 * ### Monadic mtuple_apply_append
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{10, 5}};
 * auto result = exp | fxt::mtuple_apply_append([](int a, int b) { return a / b; });
 * // result is fxt::expected<fxt::tuple<int, int, int>, Error>{10, 5, 2}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
 * auto result = ft | fxt::tuple_apply_append([](double a, double b) { return a + b; });
 * // result is fxt::flat_tuple<double, double, double>{2.0, 3.0, 5.0}
 * @endcode
 *
 * @see fxt::apply
 * @see fxt::mtuple_apply
 * @see fxt::tuple_append
 * @see fxt::tuple
 * @see fxt::flat_tuple
 */

#pragma once

#include "Apply.hpp"
#include "TupleAppend.hpp"
#include "FlatTuple.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::tuple_apply_append — direct apply and append for tuples
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
    constexpr auto tuple_apply_append(F&& f, tuple<Ts...>& t)
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
    constexpr auto tuple_apply_append(F&& f, const tuple<Ts...>& t)
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
    constexpr auto tuple_apply_append(F&& f, tuple<Ts...>&& t)
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
    constexpr auto tuple_apply_append(F&& f, const tuple<Ts...>&& t)
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
    constexpr auto tuple_apply_append(F&& f, flat_tuple<Ts...>& t)
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
    constexpr auto tuple_apply_append(F&& f, const flat_tuple<Ts...>& t)
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
    constexpr auto tuple_apply_append(F&& f, flat_tuple<Ts...>&& t)
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
    constexpr auto tuple_apply_append(F&& f, const flat_tuple<Ts...>&& t)
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

    // Deprecated forwarder — routes old name to tuple_apply_append.
    template<typename F, typename TupleT>
    [[deprecated("Use fxt::tuple_apply_append")]]
    constexpr auto apply_append(F&& f, TupleT&& t)
        -> decltype(tuple_apply_append(std::forward<F>(f), std::forward<TupleT>(t)))
    {
        return tuple_apply_append(std::forward<F>(f), std::forward<TupleT>(t));
    }

    // ========================================================================
    // fxt::tuple_apply_append (curried) — pipe-operator form
    // ========================================================================

    template<typename F>
    struct tuple_apply_append_curried
    {
        F func;

        template<typename TupleT>
        constexpr auto operator()(TupleT&& t) const
        {
            return fxt::tuple_apply_append(func, std::forward<TupleT>(t));
        }
    };

    /**
     * @brief Create a curried tuple_apply_append function for pipe operator usage
     */
    template<typename F>
    constexpr auto tuple_apply_append(F&& f)
    {
        return tuple_apply_append_curried<std::decay_t<F>>{std::forward<F>(f)};
    }

    template<typename F>
    [[deprecated("Use fxt::tuple_apply_append")]]
    constexpr auto apply_append(F&& f)
    {
        return tuple_apply_append(std::forward<F>(f));
    }

    // ========================================================================
    // apply_append_wrapper — three-case dispatch for mtuple_apply_append
    // ========================================================================

    /**
     * @brief Wrapper struct for monadic apply_append operations
     */
    template<typename TFunction>
    struct apply_append_wrapper
    {
        TFunction function;

        // Case 1: monadic<tuple> + monadic return → and_then, then append inner value
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).and_then([this](const TTuple& tuple) {
                auto result = fxt::apply(function, tuple);
                return result.transform([&tuple]<typename TValue>(TValue&& value) {
                    return fxt::tuple_append(tuple, std::forward<TValue>(value));
                });
            });
        }

        // Case 2: monadic<tuple> + void return → transform, keep tuple unchanged
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this](const TTuple& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // Case 3: monadic<tuple> + plain return → transform, append result to tuple
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && (!impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>)
                  && (!std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>)
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this](const TTuple& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }
    };

    // ========================================================================
    // fxt::mtuple_apply_append / fxt::mapply_append
    // ========================================================================

    /**
     * @brief Monadic apply_append operation for applying functions to tuple values in monadic containers
     *
     * Unpacks the tuple held by an `expected`- or `optional`-like container, invokes `f`
     * with its elements, and appends the result to the original tuple. Three dispatch cases:
     *   1. **Monadic return**: `.and_then()` — inner value is unwrapped then appended.
     *   2. **`void` return**: `.transform()` — side effects only, tuple passes through unchanged.
     *   3. **Plain return**: `.transform()` — result appended to the tuple.
     *
     * @tparam TFunction Type of the function to apply
     * @param f Function to apply to tuple elements
     * @return A callable that accepts a monadic container with a tuple
     *
     * @code
     * // With expected
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::mtuple_apply_append([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int, int, int>, Error>{std::tuple{3, 4, 7}}
     *
     * // With optional
     * auto result2 = fxt::optional<std::tuple<int, int>>{std::tuple{5, 6}}
     *              | fxt::mtuple_apply_append([](int a, int b) { return a * b; });
     * // result2 is fxt::optional<std::tuple<int, int, int>>{std::tuple{5, 6, 30}}
     *
     * // Chain multiple applications
     * auto result3 = fxt::expected<std::tuple<int, int>, Error>{std::tuple{2, 3}}
     *              | fxt::mtuple_apply_append([](int a, int b) { return a + b; })
     *              | fxt::mtuple_apply_append([](int a, int b, int sum) { return sum * 2; });
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

    inline constexpr mapply_append_fn mtuple_apply_append{};

    [[deprecated("Use fxt::mtuple_apply_append")]]
    inline constexpr mapply_append_fn mapply_append{};

}    // namespace fxt
