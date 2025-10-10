//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "../impl/concepts/IsExpected.hpp"
#include "../impl/concepts/IsOptional.hpp"
#include "../impl/utils/TupleAppend.hpp"
#include "../utils/Overload.hpp"
#include "Append.hpp"
#include <concepts>
#include <string>
#include <type_traits>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // Unified concept for monadic containers (expected-like or optional-like)
        template<typename T>
        concept monadic_container = expected_like<T> || optional_like<T>;

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
    concept returns_expected_like = impl::expected_like<std::invoke_result_t<TFunction, TArgs...>>;

    template<typename TFunction, typename... TArgs>
    concept returns_optional_like = impl::optional_like<std::invoke_result_t<TFunction, TArgs...>>;

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
        // Case 1a: Optional-like container + Function returning monadic type
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires impl::optional_like<TContainer<std::tuple<TElems...>>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TContainer<std::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>>
        {
            return tupleContainer
                ? mappend(std::apply(function, *std::forward<decltype(tupleContainer)>(tupleContainer)))(std::forward<decltype(tupleContainer)>(tupleContainer))
                : fxt::nullopt;
        }

        // ========================================================================
        // Case 1b: Expected-like container + Function returning monadic type
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
              && returns_monadic<TFunction, TElems...>
        auto operator()(TExpected<std::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
        {
            return tupleExpected
                ? mappend(std::apply(function, *std::forward<decltype(tupleExpected)>(tupleExpected)))(std::forward<decltype(tupleExpected)>(tupleExpected))
                : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tupleExpected.error());
        }

        // ========================================================================
        // Case 2a: Optional-like container + Function returning void
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires impl::optional_like<TContainer<std::tuple<TElems...>>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TContainer<std::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<std::tuple<TElems...>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const std::tuple<TElems...>& tuple) {
                std::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 2b: Expected-like container + Function returning void
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
              && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        auto operator()(TExpected<std::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<std::tuple<TElems...>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const std::tuple<TElems...>& tuple) {
                std::apply(function, tuple);
                return tuple;
            });
        }

        // ========================================================================
        // Case 3a: Optional-like container + Function returning regular value
        // ========================================================================
        template<template<typename...> class TContainer, typename... TElems>
        requires impl::optional_like<TContainer<std::tuple<TElems...>>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TContainer<std::tuple<TElems...>>&& tupleContainer) const
            -> TContainer<std::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>>
        {
            return std::forward<decltype(tupleContainer)>(tupleContainer).transform([this](const std::tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, std::apply(function, tuple));
            });
        }

        // ========================================================================
        // Case 3b: Expected-like container + Function returning regular value
        // ========================================================================
        template<template<typename, typename> class TExpected, typename... TElems, typename TError>
        requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
              && (!returns_monadic<TFunction, TElems...>)
              && (!std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
        auto operator()(TExpected<std::tuple<TElems...>, TError>&& tupleExpected) const
            -> TExpected<std::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>, TError>
        {
            return std::forward<decltype(tupleExpected)>(tupleExpected).transform([this](const std::tuple<TElems...>& tuple) {
                return fxt::tuple_append(tuple, std::apply(function, tuple));
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
