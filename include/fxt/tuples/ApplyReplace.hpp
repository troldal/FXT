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
 * ### fxt::tuple_apply_replace
 * Applies a function to tuple elements and returns a new single-element tuple holding
 * the result. If the function returns void, an empty tuple is returned.
 * - **Direct call**: `tuple_apply_replace(function, tuple)`
 * - **Curried form**: `tuple_apply_replace(function)` — for pipeline usage with `operator|`
 *
 * ### fxt::mtuple_apply_replace
 * Applies a function to tuple elements inside a monad and replaces all elements with
 * the result. Works with fxt::expected and fxt::optional containers.
 * - **Curried form**: `mtuple_apply_replace(function)` — for pipeline usage with `operator|`
 *
 * ## Key Features
 * - Type-preserving: Preserves fxt::tuple vs fxt::flat_tuple distinction
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_apply_replace handles error/nullopt propagation automatically
 * - Handles void returns: Functions returning void result in an empty tuple
 * - Monadic returns: Functions returning monadic types are properly handled
 *
 * ## Examples
 *
 * ### Basic tuple_apply_replace
 * @code
 * auto t = fxt::tuple{3, 4};
 * auto result = fxt::tuple_apply_replace([](int a, int b) { return a + b; }, t);
 * // result is fxt::tuple{7}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{2, 3}
 *     | fxt::tuple_apply_replace([](int a, int b) { return a * b; })
 *     | fxt::tuple_apply_replace([](int product) { return product + 10; });
 * // result is fxt::tuple{16}
 * @endcode
 *
 * ### Monadic mtuple_apply_replace
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{10, 5}};
 * auto result = exp | fxt::mtuple_apply_replace([](int a, int b) { return a / b; });
 * // result is fxt::expected<fxt::tuple<int>, Error>{2}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
 * auto result = ft | fxt::tuple_apply_replace([](double a, double b) { return a + b; });
 * // result is fxt::flat_tuple<double>{5.0}
 * @endcode
 *
 * @see fxt::apply
 * @see fxt::mtuple_apply
 * @see fxt::tuple_apply_append
 * @see fxt::tuple
 * @see fxt::flat_tuple
 */

#pragma once

#include "Apply.hpp"
#include "FlatTuple.hpp"
#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::tuple_apply_replace — direct apply and replace for tuples
    // ========================================================================

    /**
     * @brief Apply a function to tuple elements and return a single-element tuple
     *
     * Applies @p f to the elements of @p t and wraps the result in a new tuple of
     * the same kind (fxt::tuple or fxt::flat_tuple). If @p f returns void, an empty
     * tuple of the matching kind is returned instead.
     *
     * @tparam F     Callable type
     * @tparam TTuple Tuple type (fxt::tuple or fxt::flat_tuple)
     * @param  f     Callable to apply to the tuple elements
     * @param  t     Source tuple
     * @return Single-element tuple holding the result, or empty tuple for void returns
     */
    template<typename F, typename TTuple>
    constexpr auto tuple_apply_replace(F&& f, TTuple&& t)
    {
        using CleanTuple = std::remove_cvref_t<TTuple>;
        using result_type = impl::tuple_elements<CleanTuple>::template invoke_result_t<F>;
        if constexpr (std::is_void_v<result_type>) {
            fxt::apply(std::forward<F>(f), std::forward<TTuple>(t));
            if constexpr (impl::is_flat_tuple_v<CleanTuple>) {
                return fxt::flat_tuple<>{};
            } else {
                return fxt::tuple<>{};
            }
        } else {
            auto result = fxt::apply(std::forward<F>(f), std::forward<TTuple>(t));
            if constexpr (impl::is_flat_tuple_v<CleanTuple>) {
                return fxt::make_flat_tuple(std::move(result));
            } else {
                return fxt::make_tuple(std::move(result));
            }
        }
    }

    // Deprecated forwarder — routes old name to tuple_apply_replace.
    template<typename F, typename TupleT>
    [[deprecated("Use fxt::tuple_apply_replace")]]
    constexpr auto apply_replace(F&& f, TupleT&& t)
        -> decltype(tuple_apply_replace(std::forward<F>(f), std::forward<TupleT>(t)))
    {
        return tuple_apply_replace(std::forward<F>(f), std::forward<TupleT>(t));
    }

    // ========================================================================
    // fxt::tuple_apply_replace (curried) — pipe-operator form
    // ========================================================================

    template<typename F>
    struct tuple_apply_replace_curried
    {
        F func;

        template<typename TTuple>
        constexpr auto operator()(TTuple&& t) const
        {
            return fxt::tuple_apply_replace(func, std::forward<TTuple>(t));
        }
    };

    /**
     * @brief Create a curried tuple_apply_replace function for pipe operator usage
     */
    template<typename F>
    constexpr auto tuple_apply_replace(F&& f)
    {
        return tuple_apply_replace_curried<std::decay_t<F>>{std::forward<F>(f)};
    }

    template<typename F>
    [[deprecated("Use fxt::tuple_apply_replace")]]
    constexpr auto apply_replace(F&& f)
    {
        return tuple_apply_replace(std::forward<F>(f));
    }

    // ========================================================================
    // apply_replace_wrapper — three-case dispatch for mtuple_apply_replace
    // ========================================================================

    template<typename TFunction>
    struct apply_replace_wrapper
    {
        TFunction function;

        // Case 1: monadic<tuple> + monadic return → and_then, wrap inner value in tuple
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).and_then([this]<typename T0>(T0&& tuple) {
                auto result = fxt::apply(function, std::forward<T0>(tuple));
                return result.transform([]<typename TValue>(TValue&& value) {
                    if constexpr (impl::is_flat_tuple_v<TTuple>) {
                        return fxt::make_flat_tuple(std::forward<TValue>(value));
                    } else {
                        return fxt::make_tuple(std::forward<TValue>(value));
                    }
                });
            });
        }

        // Case 2: monadic<tuple> + void return → transform, return empty tuple
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this]<typename TValue>(TValue&& tuple) {
                fxt::apply(function, std::forward<TValue>(tuple));
                if constexpr (impl::is_flat_tuple_v<TTuple>) {
                    return fxt::flat_tuple<>{};
                } else {
                    return fxt::tuple<>{};
                }
            });
        }

        // Case 3: monadic<tuple> + plain return → transform, wrap value in tuple
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && (!impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>)
                  && (!std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>)
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this]<typename TValue>(TValue&& tuple) {
                auto result = fxt::apply(function, std::forward<TValue>(tuple));
                if constexpr (impl::is_flat_tuple_v<TTuple>) {
                    return fxt::make_flat_tuple(std::move(result));
                } else {
                    return fxt::make_tuple(std::move(result));
                }
            });
        }
    };

    // ========================================================================
    // fxt::mtuple_apply_replace / fxt::mapply_replace
    // ========================================================================

    /**
     * @brief Monadic apply_replace — applies a function to tuple elements inside a monad,
     *        replacing all elements with the function's result.
     *
     * Three dispatch cases:
     *   1. **Monadic return**: `.and_then()` — inner value is unwrapped, wrapped in a new tuple.
     *   2. **`void` return**: `.transform()` — side effects only, empty tuple returned.
     *   3. **Plain return**: `.transform()` — result wrapped in a single-element tuple.
     *
     * @code
     * // With expected
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int>, Error>{std::tuple{7}}
     *
     * // With optional
     * auto result2 = fxt::optional<std::tuple<int, int>>{std::tuple{5, 6}}
     *              | fxt::mtuple_apply_replace([](int a, int b) { return a * b; });
     * // result2 is fxt::optional<std::tuple<int>>{std::tuple{30}}
     *
     * // Chain multiple applications
     * auto result3 = fxt::expected<std::tuple<int, int>, Error>{std::tuple{2, 3}}
     *              | fxt::mtuple_apply_replace([](int a, int b) { return a + b; })
     *              | fxt::mtuple_apply_replace([](int sum) { return sum * 2; });
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

    inline constexpr mapply_replace_fn mtuple_apply_replace{};

    [[deprecated("Use fxt::mtuple_apply_replace")]]
    inline constexpr mapply_replace_fn mapply_replace{};

}    // namespace fxt
