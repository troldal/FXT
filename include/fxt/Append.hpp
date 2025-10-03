//
// Created by kenne on 23/02/2025.
//

#pragma once

// #include <Boost/hana.hpp>
#include "Expected.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Overload.hpp" // added for overloaded lambda support
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
         * @tparam TValue The type stored in the input expected
         * @tparam TError The error type of the input expected
         * @param expectedValue The expected-like container holding the value to append
         * @return A function that takes an expected-like container with a tuple and returns a new one with the value appended
         *
         * @note Requires that the error type of expectedValue is convertible to the error type of the tuple container
         */
        template<template<typename, typename> class TExpected, typename TValue, typename TError>
            requires impl::expected_like<TExpected<TValue, TError>>
        auto operator()(const TExpected<TValue, TError>& expectedValue) const
        {
            return [value = expectedValue]<template<typename, typename> class TExpectedOutput, typename TTuple, typename TErrorOutput>(
                       const TExpectedOutput<TTuple, TErrorOutput>& tupleExpected)
                requires std::convertible_to<TError, TErrorOutput>
            {
                return value ? tupleExpected.transform([value](const TTuple& tuple) { return impl::tuple_append(tuple, *value); })
                             : typename TExpectedOutput<TTuple, TErrorOutput>::unexpected_type(value.error());
            };
        }

        /**
         * @brief Append a value from an expected-like container (rvalue) to a tuple in another expected-like container
         *
         * Move-enabled version for efficiency when the input expected is a temporary.
         */
        template<template<typename, typename> class TExpected, typename TValue, typename TError>
            requires impl::expected_like<TExpected<TValue, TError>>
        auto operator()(TExpected<TValue, TError>&& expectedValue) const
        {
            return
                [value =
                     std::move(expectedValue)]<template<typename, typename> class TExpectedOutput, typename TTuple, typename TErrorOutput>(
                    const TExpectedOutput<TTuple, TErrorOutput>& tupleExpected) mutable
                requires std::convertible_to<TError, TErrorOutput>
            {
                return value
                    ? tupleExpected.transform([value = std::move(value)](const TTuple& tuple) mutable {
                        return impl::tuple_append(tuple, std::move(*value)); })
                    : typename TExpectedOutput<TTuple, TErrorOutput>::unexpected_type(std::move(value.error()));
            };
        }

        /**
         * @brief Append a value from a container with a value_type member to a tuple in an expected-like container
         *
         * This overload handles types like std::optional and other container-like types.
         *
         * @tparam TValue The type holding the value to append (must have a value_type member)
         * @param value The container holding the value to append
         * @return A function that takes an expected-like container with a tuple and returns a new one with the value appended
         */
        template<typename TValue>
            requires HasValueType<TValue>
        auto operator()(const TValue& value) const
        {
            return [value]<template<typename, typename> class TExpected, typename TTuple, typename TError>(
                       const TExpected<TTuple, TError>& tupleExpected)
                requires impl::expected_like<TExpected<TTuple, TError>>
            {
                return tupleExpected.transform([value](const TTuple& tuple) {
                    return impl::tuple_append(tuple, value);
                });
            };
        }

        /**
         * @brief Append a value from a container with a value_type member (rvalue) to a tuple in an expected-like container
         */
        template<typename TValue>
            requires HasValueType<std::remove_cvref_t<TValue>> && (!std::is_lvalue_reference_v<TValue>)
        auto operator()(TValue&& value) const
        {
            return [value = std::forward<TValue>(value)]<template<typename, typename> class TExpected, typename TTuple, typename TError>(
                       const TExpected<TTuple, TError>& tupleExpected) mutable
                requires impl::expected_like<TExpected<TTuple, TError>>
            {
                return tupleExpected.transform([value = std::move(value)](const TTuple& tuple) mutable {
                    return impl::tuple_append(tuple, std::move(value));
                });
            };
        }

        /**
         * @brief Append an arbitrary value to a tuple in an expected-like container
         *
         * This is the fallback overload for handling any value type.
         *
         * @tparam TValue The type of the value to append
         * @param value The value to append
         * @return A function that takes an fxt::expected container with a tuple and returns a new one with the value appended
         */
        template<typename TValue>
            requires (!HasValueType<TValue>)
        auto operator()(const TValue& value) const
        {
            return [value]<template<typename, typename> class TExpected, typename TTuple, typename TError>(
                       const TExpected<TTuple, TError>& tupleExpected)
                requires impl::expected_like<TExpected<TTuple, TError>>
            {
                return tupleExpected.transform([value](const TTuple& tuple) {
                    return impl::tuple_append(tuple, value);
                });
            };
        }

        /**
         * @brief Append an arbitrary value (rvalue) to a tuple in an expected-like container
         */
        template<typename TValue>
            requires (!HasValueType<std::remove_cvref_t<TValue>>) && (!std::is_lvalue_reference_v<TValue>)
        auto operator()(TValue&& value) const
        {
            return [value = std::forward<TValue>(value)]<template<typename, typename> class TExpected, typename TTuple, typename TError>(
                       const TExpected<TTuple, TError>& tupleExpected) mutable
                requires impl::expected_like<TExpected<TTuple, TError>>
            {
                return tupleExpected.transform([value = std::move(value)](const TTuple& tuple) mutable {
                    return impl::tuple_append(tuple, std::move(value));
                });
            };
        }
    };

    inline constexpr AppendWrapper append = {};

}    // namespace fxt
