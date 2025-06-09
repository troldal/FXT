//
// Created by kenne on 23/02/2025.
//

#pragma once

// #include <Boost/hana.hpp>
#include "Expected.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include <string>

namespace fxt
{
    template<typename T>
    concept HasValueType = requires { typename T::value_type; };

    /**
     * @brief Wrapper for appending values to tuples contained in expected-like objects
     *
     * AppendWrapper provides a set of overloaded operators to handle different appending scenarios:
     * - Appending values from expected-like containers (with proper error handling)
     * - Appending values from types with a value_type member
     * - Appending arbitrary values
     */
    struct AppendWrapper
    {
        /**
         * @brief Append a value from an expected-like container to a tuple in another expected-like container
         *
         * This overload handles monadic composition, propagating errors appropriately.
         *
         * @tparam TExpected The expected-like template used for the input value
         * @tparam T The type stored in the input expected
         * @tparam E The error type of the input expected
         * @param val The expected-like container holding the value to append
         * @return A function that takes an expected-like container with a tuple and returns a new one with the value appended
         *
         * @note Requires that the error type of val is convertible to the error type of the tuple container
         */
        template<template<typename, typename> class TExpected, typename T, typename E>
            requires impl::expected_like<TExpected<T, E>>
        auto operator()(const TExpected<T, E>& val) const
        {
            using TValue      = typename TExpected<T, E>::value_type;
            using TError      = typename TExpected<T, E>::error_type;
            using TUnexpected = typename TExpected<T, E>::unexpected_type;

            return [val]<template<typename, typename> class TExpectedOut, typename TTuple, typename TErrorOut>(
                       const TExpectedOut<TTuple, TErrorOut>& tuple)
                requires std::convertible_to<TError, TErrorOut>
            {
                return val ? tuple.transform([val](const TTuple& t) { return impl::tuple_append(t, *val); })
                           : typename TExpectedOut<TTuple, TError>::unexpected_type(val.error());
            };
        }

        /**
         * @brief Append a value from a container with a value_type member to a tuple in an expected-like container
         *
         * This overload handles types like std::optional and other container-like types.
         *
         * @tparam TValue The type holding the value to append (must have a value_type member)
         * @param val The container holding the value to append
         * @return A function that takes an expected-like container with a tuple and returns a new one with the value appended
         */
        template<typename TValue>
            requires HasValueType<TValue>
        auto operator()(const TValue& val) const
        {
            return [val]<template<typename, typename> class TExpected, typename TTuple, typename TError>(
                       const TExpected<TTuple, TError>& tuple)
                requires impl::expected_like<TExpected<TTuple, TError>>
            { return tuple.transform([&](const TTuple& t) { return impl::tuple_append(t, val); }); };
        }

        /**
         * @brief Append an arbitrary value to a tuple in an expected-like container
         *
         * This is the fallback overload for handling any value type.
         *
         * @tparam TValue The type of the value to append
         * @param val The value to append
         * @return A function that takes an fxt::expected container with a tuple and returns a new one with the value appended
         */
        template<typename TValue>
        auto operator()(const TValue& val) const
        {
            return [val, this]<typename... TElems, typename TError>(const fxt::expected<std::tuple<TElems...>, TError>& tuple) {
                return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, val); });
            };
        }
    };

    inline constexpr AppendWrapper append = {};

}    // namespace fxt
