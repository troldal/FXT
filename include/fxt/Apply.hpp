//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Append.hpp"
#include "Overload.hpp"
#include <string>
#include <type_traits>
#include <concepts>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // Helper to extract value type from expected-like objects
        template<typename T>
        struct expected_value_type
        {
            using type = T;
        };

        template<typename T>
            requires expected_like<T>
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

    /**
     * @brief Monadic apply operation for applying functions to tuple values in expected-like containers
     *
     * This function object provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in an expected-like container. It handles three distinct cases through an overload set:
     * 1. Functions returning expected-like types (monadic bind)
     * 2. Functions with void return type
     * 3. Functions returning any other type
     *
     * @tparam TFunction Type of the function to apply
     * @param f Function to apply to tuple elements
     * @return A callable that accepts an expected-like container with a tuple and applies the function
     *
     * @section Usage
     * @code
     * // Apply a function to tuple elements
     * auto result = fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
     *             | fxt::apply([](int a, int b) { return a + b; });
     * // result is fxt::expected<std::tuple<int, int, int>, Error>{std::tuple{3, 4, 7}}
     *
     * // Chain multiple applications
     * auto result2 = fxt::expected<std::tuple<>, Error>{std::tuple{}}
     *              | fxt::append(5)
     *              | fxt::append(10)
     *              | fxt::apply([](int a, int b) { return a + b; })
     *              | fxt::apply([](int a, int b, int sum) { return sum * 2; });
     * @endcode
     */
    inline constexpr auto apply = []<typename TFunction>(TFunction&& function) {
        return overload{
            // Case 1: Function returns expected-like type (monadic bind)
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
                requires impl::expected_like<std::invoke_result_t<TFunction, TElems...>>
            {
                return tupleExpected
                    ? append(std::apply(function, *std::forward<decltype(tupleExpected)>(tupleExpected)))(std::forward<decltype(tupleExpected)>(tupleExpected))
                    : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tupleExpected.error());
            },

            // Case 2: Function returns void
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems...>, TError>
                requires std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
            {
                return std::forward<decltype(tupleExpected)>(tupleExpected).transform([&](const std::tuple<TElems...>& tuple) {
                    std::apply(function, tuple);
                    return tuple;
                });
            },

            // Case 3: Function returns any other type (non-expected, non-void)
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>, TError>
            {
                return std::forward<decltype(tupleExpected)>(tupleExpected).transform([&](const std::tuple<TElems...>& tuple) {
                    return impl::tuple_append(tuple, std::apply(function, tuple));
                });
            }
        };
    };

}    // namespace fxt