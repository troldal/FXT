//
// Created by kenne on 25/02/2025.
//

#pragma once


#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Append.hpp"
#include <string>
#include <type_traits>
#include <concepts>

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
     * @brief ApplyWrapper provides monadic application of functions to tuple values in expected-like containers
     *
     * This structure provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in an expected-like container. It handles three distinct cases:
     * 1. Functions returning expected-like types (monadic bind)
     * 2. Functions with void return type
     * 3. Functions returning any other type
     */
    struct ApplyWrapper
    {
        /**
         * @brief Apply a function to the contents of an expected tuple
         *
         * @tparam TFunction Type of the function to apply
         * @param f Function to apply to tuple elements
         * @return A lambda that takes an expected-like container and returns a new one with the function applied
         *
         * The returned lambda handles different return type scenarios through the private apply methods.
         */
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                       const TExpected<std::tuple<TElems...>, TError>& tup)
                       -> TExpected<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError> {
                return ApplyWrapper::apply(f, tup);
            };
        }

    private:
        /**
         * @brief Handle functions that return expected-like types
         *
         * @tparam TFunction Function type
         * @tparam TExpected Expected-like container template
         * @tparam TElems Tuple element types
         * @tparam TError Error type
         * @param f Function to apply
         * @param tup Expected container with tuple
         * @return A new expected container with the result appended to the tuple or propagated error
         */
        template<typename TFunction, template<typename, typename> class TExpected, typename... TElems, typename TError>
            requires impl::expected_like<std::invoke_result_t<TFunction, TElems...>>
        static auto apply(const TFunction& f, const TExpected<std::tuple<TElems...>, TError>& tup)
        {
            return tup ? append(std::apply(f, *tup))(tup)
                       : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tup.error());
        }

        /**
         * @brief Handle functions that return void
         *
         * @tparam TFunction Function type
         * @tparam TExpected Expected-like container template
         * @tparam TElems Tuple element types
         * @tparam TError Error type
         * @param f Function to apply
         * @param tup Expected container with tuple
         * @return The original tuple if successful, otherwise propagates the error
         */
        template<typename TFunction, template<typename, typename> class TExpected, typename... TElems, typename TError>
            requires std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
        static auto apply(const TFunction& f, const TExpected<std::tuple<TElems...>, TError>& tup)
        {
            return tup.transform([&](const std::tuple<TElems...>& t) {
                std::apply(f, *tup);
                return t;
            });
        }

        /**
         * @brief Handle functions that return any non-expected, non-void type
         *
         * @tparam TFunction Function type
         * @tparam TExpected Expected-like container template
         * @tparam TElems Tuple element types
         * @tparam TError Error type
         * @param f Function to apply
         * @param tup Expected container with tuple
         * @return A new expected container with the function result appended to the tuple
         */
        template<typename TFunction, template<typename, typename> class TExpected, typename... TElems, typename TError>
        static auto apply(const TFunction& f, const TExpected<std::tuple<TElems...>, TError>& tup)
        {
            return tup.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, *tup)); });
        }
    };

    inline constexpr ApplyWrapper apply = {};

}    // namespace fxt
