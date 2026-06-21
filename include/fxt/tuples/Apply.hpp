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
 * @file Apply.hpp
 * @brief Tuple apply operations: non-monadic fxt::apply and monadic fxt::mtuple_apply
 *
 * ## fxt::apply
 * Extends std::apply to cover fxt::flat_tuple, and provides a curried single-argument
 * overload for pipe-operator usage:
 * @code
 * auto t  = fxt::make_tuple(1, 2, 3);
 * auto s  = t | fxt::apply([](int a, int b, int c) { return a + b + c; }); // 6
 *
 * auto ft = fxt::make_flat_tuple(2.0, 3.0);
 * auto p  = ft | fxt::apply([](double a, double b) { return a * b; });     // 6.0
 * @endcode
 *
 * ## fxt::mtuple_apply
 * Monadic apply that unpacks a tuple held inside an `expected`- or `optional`-like
 * container and invokes a callable with its elements. The container is updated to hold
 * only the function's return value — the original tuple is **replaced**.
 * Use fxt::mtuple_apply_append to extend the tuple instead.
 *
 * Three dispatch cases:
 *   1. **Monadic return**: `.and_then()` — early-exit on error/nullopt.
 *   2. **void return**: `.transform()` returning `fxt::unit`.
 *   3. **Plain return**: `.transform()` replacing the tuple with the value.
 *
 * @see fxt::mtuple_apply_append (ApplyAppend.hpp)
 */

#pragma once

#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include "TupleAppend.hpp"
#include "../utils/Unit.hpp"
#include <type_traits>
#include <utility>
#include <tuple>

namespace fxt
{
    // ========================================================================
    // fxt::apply — non-monadic apply for std::tuple and fxt::flat_tuple
    // ========================================================================

    // Re-export std::apply unchanged; it handles fxt::tuple (= std::tuple) directly.
    using std::apply;

    // Single forwarding-reference overload constrained to fxt::flat_tuple. The
    // requires clause keeps it from competing with the re-exported std::apply,
    // which still handles std::tuple and other tuple-protocol types. The index
    // pack is generated and expanded inline via an immediately-invoked lambda;
    // value-category correctness is delegated to fxt::get's four overloads via
    // std::forward.
    template<typename F, typename FlatTupleT>
        requires impl::is_flat_tuple_v<std::remove_cvref_t<FlatTupleT>>
    constexpr decltype(auto) apply(F&& f, FlatTupleT&& t)
    {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
            return std::invoke(std::forward<F>(f), fxt::get<Is>(std::forward<FlatTupleT>(t))...);
        }(std::make_index_sequence<std::remove_cvref_t<FlatTupleT>::size()>{});
    }

    /**
     * @brief Curried apply for pipe-operator usage.
     *
     * Returns a generic callable that, when invoked with a tuple (or flat_tuple),
     * applies @p f to its elements. Both `fxt::tuple` and `fxt::flat_tuple` are
     * supported via the overloaded two-argument `fxt::apply`.
     */
    template<typename F>
    constexpr auto apply(F&& f)
    {
        return [f = std::forward<F>(f)]<typename TupleT>(TupleT&& t) -> decltype(auto) {
            return fxt::apply(f, std::forward<TupleT>(t));
        };
    }

    // ========================================================================
    // impl helpers for fxt::mtuple_apply
    // ========================================================================

    namespace impl
    {
        template<typename TFunction, typename... TArgs>
        concept returns_expected_like = expected_like<std::invoke_result_t<TFunction, TArgs...>>;

        template<typename TFunction, typename... TArgs>
        concept returns_optional_like = optional_like<std::invoke_result_t<TFunction, TArgs...>>;

        template<typename TFunction, typename... TArgs>
        concept returns_monadic = returns_expected_like<TFunction, TArgs...> || returns_optional_like<TFunction, TArgs...>;

        template<typename T>
        struct tuple_elements;

        template<template<typename...> class TTuple, typename... Ts>
        struct tuple_elements<TTuple<Ts...>>
        {
            template<typename TFunction>
            static constexpr bool returns_monadic_v = returns_monadic<TFunction, Ts...>;

            template<typename TFunction>
            using invoke_result_t = std::invoke_result_t<TFunction, Ts...>;
        };

        template<typename T>
        concept monadic_container = optional_like<T> || expected_like<T>;
    }    // namespace impl

    // Thin wrappers kept for ApplyAppend.hpp / ApplyReplace.hpp compatibility.
    template<typename TFunction, typename TTuple>
    concept returns_monadic_with_tuple = impl::tuple_elements<TTuple>::template returns_monadic_v<TFunction>;

    template<typename TFunction, typename TTuple>
    using invoke_result_with_tuple_t = impl::tuple_elements<TTuple>::template invoke_result_t<TFunction>;

    // ========================================================================
    // apply_wrapper — three-case dispatch for mtuple_apply
    // ========================================================================

    template<typename TFunction>
    struct apply_wrapper
    {
        TFunction function;

        // Case 1: monadic<tuple> + monadic return → and_then (flattens the monad)
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).and_then([this](const TTuple& tuple) {
                return fxt::apply(function, tuple);
            });
        }

        // Case 2: monadic<tuple> + void return → transform returning fxt::unit
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this](const TTuple& tuple) {
                fxt::apply(function, tuple);
                return fxt::unit{};
            });
        }

        // Case 3: monadic<tuple> + plain return → transform (tuple replaced by value)
        template<typename TArg, typename TTuple = std::remove_cvref_t<TArg>::value_type>
            requires impl::monadic_container<std::remove_cvref_t<TArg>>
                  && tuple_like<std::remove_cvref_t<TTuple>>
                  && (!impl::tuple_elements<std::remove_cvref_t<TTuple>>::template returns_monadic_v<TFunction>)
                  && (!std::same_as<typename impl::tuple_elements<std::remove_cvref_t<TTuple>>::template invoke_result_t<TFunction>, void>)
        auto operator()(TArg&& arg) const
        {
            return std::forward<TArg>(arg).transform([this](const TTuple& tuple) {
                return fxt::apply(function, tuple);
            });
        }
    };

    // ========================================================================
    // fxt::mtuple_apply / fxt::mapply
    // ========================================================================

    /**
     * @brief Monadic apply — applies a function to tuple elements inside a monad,
     *        replacing the tuple with the function's result.
     *
     * `fxt::mtuple_apply(f)` unpacks the tuple held by an `expected`- or `optional`-like
     * container and invokes `f` with its elements. The container is updated to hold only
     * the function's return value — the original tuple is **replaced**, not extended.
     * Use `fxt::mtuple_apply_append(f)` to keep the tuple and append the result.
     *
     * Three dispatch cases are handled automatically:
     *   1. **Monadic return** (`expected`/`optional`): `.and_then()` — early-exit on error/nullopt.
     *   2. **`void` return**: `.transform()` returning `fxt::unit` — side effects only.
     *   3. **Plain return**: `.transform()` — tuple replaced by the return value.
     *
     * @tparam TFunction Type of the callable to apply to the tuple elements
     * @param  f         Callable accepting the unpacked tuple elements
     * @return A pipe adaptor that, when applied to a monad-of-tuple, returns a monad of
     *         the function's result type (or monad-of-unit for void functions).
     *
     * @code
     * // Case 3 — plain return: tuple replaced by function result
     * auto r1 = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(3, 4)}
     *         | fxt::mtuple_apply([](int a, int b) { return a + b; });
     * // r1 is fxt::expected<int, std::string>{7}
     *
     * // Case 2 — void return: side effects only, container holds fxt::unit
     * int logged = 0;
     * auto r2 = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(3, 4)}
     *         | fxt::mtuple_apply([&logged](int a, int b) { logged = a + b; });
     * // logged == 7; r2 is fxt::expected<fxt::unit, std::string>{}
     *
     * // Case 1 — monadic return: flattened via and_then
     * auto r3 = fxt::expected<std::tuple<double, double>, std::string>{std::make_tuple(10.0, 2.0)}
     *         | fxt::mtuple_apply([](double a, double b) -> fxt::expected<double, std::string> {
     *               if (b == 0.0) return fxt::unexpected("division by zero");
     *               return a / b;
     *           });
     * // r3 is fxt::expected<double, std::string>{5.0}
     * @endcode
     */
    struct apply_fn
    {
        template<typename TFunction>
        constexpr auto operator()(TFunction&& function) const
        {
            return apply_wrapper<std::decay_t<TFunction>>{std::forward<TFunction>(function)};
        }
    };

    inline constexpr apply_fn mtuple_apply{};

    [[deprecated("Use fxt::mtuple_apply")]]
    inline constexpr apply_fn mapply{};

}    // namespace fxt
