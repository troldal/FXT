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

#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include "TupleAppend.hpp"
#include "../utils/Overload.hpp"
#include "Append.hpp"
#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

namespace fxt
{
    // ========================================================================
    // fxt::apply - Direct apply function for tuples (non-monadic)
    // ========================================================================

    /**
     * @brief Apply a function to elements of fxt::tuple
     *
     * For fxt::tuple (which is an alias for std::tuple), this simply forwards to std::apply.
     *
     * @tparam F Function type
     * @tparam Ts Tuple element types
     * @param f Function to apply
     * @param t Tuple whose elements are used as function arguments
     * @return Result of invoking f with tuple elements as arguments
     */
    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, tuple<Ts...>& t)
    {
        return std::apply(std::forward<F>(f), t);
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, const tuple<Ts...>& t)
    {
        return std::apply(std::forward<F>(f), t);
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, tuple<Ts...>&& t)
    {
        return std::apply(std::forward<F>(f), std::move(t));
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, const tuple<Ts...>&& t)
    {
        return std::apply(std::forward<F>(f), std::move(t));
    }

    /**
     * @brief Apply a function to elements of fxt::flat_tuple
     *
     * For fxt::flat_tuple, this provides a custom implementation that unpacks
     * the flat_tuple elements and passes them as arguments to the function.
     *
     * @tparam F Function type
     * @tparam Ts flat_tuple element types
     * @param f Function to apply
     * @param t flat_tuple whose elements are used as function arguments
     * @return Result of invoking f with flat_tuple elements as arguments
     */
    namespace impl
    {
        template<typename F, typename... Ts, std::size_t... Is>
        constexpr decltype(auto) apply_flat_tuple_impl(F&& f, flat_tuple<Ts...>& t, std::index_sequence<Is...>)
        {
            return std::invoke(std::forward<F>(f), fxt::get<Is>(t)...);
        }

        template<typename F, typename... Ts, std::size_t... Is>
        constexpr decltype(auto) apply_flat_tuple_impl(F&& f, const flat_tuple<Ts...>& t, std::index_sequence<Is...>)
        {
            return std::invoke(std::forward<F>(f), fxt::get<Is>(t)...);
        }

        template<typename F, typename... Ts, std::size_t... Is>
        constexpr decltype(auto) apply_flat_tuple_impl(F&& f, flat_tuple<Ts...>&& t, std::index_sequence<Is...>)
        {
            return std::invoke(std::forward<F>(f), fxt::get<Is>(std::move(t))...);
        }

        template<typename F, typename... Ts, std::size_t... Is>
        constexpr decltype(auto) apply_flat_tuple_impl(F&& f, const flat_tuple<Ts...>&& t, std::index_sequence<Is...>)
        {
            return std::invoke(std::forward<F>(f), fxt::get<Is>(std::move(t))...);
        }
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, flat_tuple<Ts...>& t)
    {
        return impl::apply_flat_tuple_impl(std::forward<F>(f), t, std::index_sequence_for<Ts...>{});
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, const flat_tuple<Ts...>& t)
    {
        return impl::apply_flat_tuple_impl(std::forward<F>(f), t, std::index_sequence_for<Ts...>{});
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, flat_tuple<Ts...>&& t)
    {
        return impl::apply_flat_tuple_impl(std::forward<F>(f), std::move(t), std::index_sequence_for<Ts...>{});
    }

    template<typename F, typename... Ts>
    constexpr decltype(auto) apply(F&& f, const flat_tuple<Ts...>&& t)
    {
        return impl::apply_flat_tuple_impl(std::forward<F>(f), std::move(t), std::index_sequence_for<Ts...>{});
    }

    // ========================================================================
    // Curried versions of fxt::apply for use with pipe operator
    // ========================================================================

    /**
     * @brief Curried version of apply for use with pipe operator
     *
     * Returns a callable that applies the given function to a tuple passed to it.
     * Works with both fxt::tuple and fxt::flat_tuple.
     *
     * @tparam F Function type
     * @param f Function to apply to tuple elements
     * @return A callable that takes a tuple and returns the result of applying f
     *
     * Example:
     * @code
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto sum = t | fxt::apply([](int a, int b, int c) { return a + b + c; });
     * // sum = 6
     *
     * auto ft = fxt::make_flat_tuple(2.0, 3.0);
     * auto product = ft | fxt::apply([](double a, double b) { return a * b; });
     * // product = 6.0
     * @endcode
     */
    template<typename F>
    struct apply_curried
    {
        F func;

        // Overload for fxt::tuple (lvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(tuple<Ts...>& t) const
        {
            return fxt::apply(func, t);
        }

        // Overload for fxt::tuple (const lvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(const tuple<Ts...>& t) const
        {
            return fxt::apply(func, t);
        }

        // Overload for fxt::tuple (rvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(tuple<Ts...>&& t) const
        {
            return fxt::apply(func, std::move(t));
        }

        // Overload for fxt::tuple (const rvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(const tuple<Ts...>&& t) const
        {
            return fxt::apply(func, std::move(t));
        }

        // Overload for fxt::flat_tuple (lvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(flat_tuple<Ts...>& t) const
        {
            return fxt::apply(func, t);
        }

        // Overload for fxt::flat_tuple (const lvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(const flat_tuple<Ts...>& t) const
        {
            return fxt::apply(func, t);
        }

        // Overload for fxt::flat_tuple (rvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(flat_tuple<Ts...>&& t) const
        {
            return fxt::apply(func, std::move(t));
        }

        // Overload for fxt::flat_tuple (const rvalue)
        template<typename... Ts>
        constexpr decltype(auto) operator()(const flat_tuple<Ts...>&& t) const
        {
            return fxt::apply(func, std::move(t));
        }
    };

    /**
     * @brief Create a curried apply function for pipe operator usage
     *
     * This overload is called when apply is given just a function (no tuple argument).
     * It returns a callable that can be used with the pipe operator.
     *
     * @tparam F Function type
     * @param f Function to apply
     * @return A callable wrapper that applies f to a tuple
     */
    template<typename F>
    constexpr auto apply(F&& f)
    {
        return apply_curried<std::decay_t<F>>{std::forward<F>(f)};
    }

    namespace impl
    {
        // Helper to extract value type from expected-like or optional-like objects
        template<typename T>
        struct expected_value_type
        {
            using type = T;
        };

        // Expected-like specialization takes precedence (more specific)
        template<typename T>
            requires expected_like<T>
        struct expected_value_type<T>
        {
            using type = typename T::value_type;
        };

        // Optional-like specialization only applies if NOT expected-like
        template<typename T>
            requires optional_like<T> && (!expected_like<T>)
        struct expected_value_type<T>
        {
            using type = typename T::value_type;
        };

        template<typename T>
        using expected_value_type_t = typename expected_value_type<T>::type;

        // Main type trait for function return type processing
        template<typename TFunction, typename... TArgs>
        struct processed_invoke_result
        {
            using raw_result = std::invoke_result_t<TFunction, TArgs...>;
            using type       = expected_value_type_t<raw_result>;
        };

        template<typename TFunction, typename... TArgs>
        using processed_invoke_result_t = typename processed_invoke_result<TFunction, TArgs...>::type;
    }    // namespace impl

    // Concept version for easier use
    template<typename TFunction, typename... TArgs>
    concept returns_expected_like = expected_like<std::invoke_result_t<TFunction, TArgs...>>;

    template<typename TFunction, typename... TArgs>
    concept returns_optional_like = optional_like<std::invoke_result_t<TFunction, TArgs...>>;

    template<typename TFunction, typename... TArgs>
    concept returns_monadic = returns_expected_like<TFunction, TArgs...> || returns_optional_like<TFunction, TArgs...>;

    /**
     * @brief Wrapper struct that holds a function and provides overloaded operator() for applying it
     */
    template<typename TFunction>
    struct apply_wrapper
    {
        TFunction function;

        // ========================================================================
        // Case 1a: Optional-like container + fxt::tuple + Function returning monadic type
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::tuple<TElems...>>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TContainer<fxt::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>>
        {
            return tupleContainer
                ? mappend(fxt::apply(function, *std::forward<decltype(tupleContainer)>(tupleContainer)))(std::forward<decltype(tupleContainer)>(tupleContainer))
                : fxt::nullopt;
        }

        // ========================================================================
        // Case 1a-flat: Optional-like container + fxt::flat_tuple + Function returning monadic type
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::flat_tuple<TElems...>>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TContainer<fxt::flat_tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::flat_tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>>
        {
            return tupleContainer
                ? mappend(fxt::apply(function, *std::forward<decltype(tupleContainer)>(tupleContainer)))(std::forward<decltype(tupleContainer)>(tupleContainer))
                : fxt::nullopt;
        }

        // ========================================================================
        // Case 1b: Expected-like container + fxt::tuple + Function returning monadic type
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::tuple<TElems...>, TError>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TExpected<fxt::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
        {
            return tupleExpected
                ? mappend(fxt::apply(function, *std::forward<decltype(tupleExpected)>(tupleExpected)))(std::forward<decltype(tupleExpected)>(tupleExpected))
                : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tupleExpected.error());
        }

        // ========================================================================
        // Case 1b-flat: Expected-like container + fxt::flat_tuple + Function returning monadic type
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::flat_tuple<TElems...>, TError>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TExpected<fxt::flat_tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::flat_tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
        {
            return tupleExpected
                ? mappend(fxt::apply(function, *std::forward<decltype(tupleExpected)>(tupleExpected)))(std::forward<decltype(tupleExpected)>(tupleExpected))
                : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tupleExpected.error());
        }

        // ========================================================================
        // Case 2a: Optional-like container + fxt::tuple + Function returning void
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::tuple<TElems...>>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TContainer<fxt::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::tuple<TElems...>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const fxt::tuple<TElems...>& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 2a-flat: Optional-like container + fxt::flat_tuple + Function returning void
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::flat_tuple<TElems...>>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TContainer<fxt::flat_tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::flat_tuple<TElems...>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const fxt::flat_tuple<TElems...>& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 2b: Expected-like container + fxt::tuple + Function returning void
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::tuple<TElems...>, TError>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TExpected<fxt::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::tuple<TElems...>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const fxt::tuple<TElems...>& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 2b-flat: Expected-like container + fxt::flat_tuple + Function returning void
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::flat_tuple<TElems...>, TError>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TExpected<fxt::flat_tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::flat_tuple<TElems...>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const fxt::flat_tuple<TElems...>& tuple) {
                fxt::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 3a: Optional-like container + fxt::tuple + Function returning regular value
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::tuple<TElems...>>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TContainer<fxt::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const fxt::tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }

        // ========================================================================
        // Case 3a-flat: Optional-like container + fxt::flat_tuple + Function returning regular value
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires optional_like<TContainer<fxt::flat_tuple<TElems...>>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TContainer<fxt::flat_tuple<TElems...>>&& tupleContainer) const
            -> TContainer<fxt::flat_tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const fxt::flat_tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }

        // ========================================================================
        // Case 3b: Expected-like container + fxt::tuple + Function returning regular value
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::tuple<TElems...>, TError>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TExpected<fxt::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const fxt::tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }

        // ========================================================================
        // Case 3b-flat: Expected-like container + fxt::flat_tuple + Function returning regular value
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires expected_like<TExpected<fxt::flat_tuple<TElems...>, TError>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TExpected<fxt::flat_tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<fxt::flat_tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const fxt::flat_tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, fxt::apply(function, tuple));
            });
        }
    };

    /**
     * @brief Monadic apply operation for applying functions to tuple values in monadic containers
     *
     * This function object provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in a monadic container (expected-like or optional-like). It handles three distinct cases:
     * 1. Functions returning monadic types (expected-like or optional-like) - monadic bind
     * 2. Functions with void return type
     * 3. Functions returning any other type
     *
     * @tparam TFunction Type of the function to apply
     * @param f Function to apply to tuple elements
     * @return A callable that accepts a monadic container with a tuple and applies the function
     *
     * @section Usage
     * @code
     * // With expected
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::apply([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int, int, int>, Error>{std::tuple{3, 4, 7}}
     *
     * // With optional
     * auto result2 = fxt::optional<std::tuple<int, int>>{std::tuple{5, 6}}
     *              | fxt::apply([](int a, int b) { return a * b; });
     * // result2 is fxt::optional<std::tuple<int, int, int>>{std::tuple{5, 6, 30}}
     *
     * // Chain multiple applications
     * auto result3 = fxt::expected<std::tuple<>, Error>{std::tuple{}}
     *              | fxt::append(5)
     *              | fxt::append(10)
     *              | fxt::apply([](int a, int b) { return a + b; })
     *              | fxt::apply([](int a, int b, int sum) { return sum * 2; });
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

    inline constexpr apply_fn mapply{};

}    // namespace fxt
