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
// #include "../impl/utils/TupleAppend.hpp"

#include "TupleAppend.hpp"
#include <fxt/tuples/FlatTuple.hpp>
#include <tuple>

namespace fxt
{
    /**
     * @brief Function object wrapper for appending values to tuples within monadic containers
     *
     * Provides overloaded call operators to append values to tuples contained in
     * expected-like or optional-like containers, preserving the monadic semantics.
     * Supports both lvalue and rvalue references for optimal performance.
     */
    struct AppendWrapper
    {
        /**
         * @brief Append a value from an expected-like container (lvalue) to a tuple in another expected-like container
         *
         * This overload handles monadic composition for expected types, propagating errors appropriately.
         * When the input expected contains an error, that error is propagated to the result.
         *
         * @tparam TExpected The expected-like template (e.g., std::expected)
         * @tparam TValue The type of value stored in the input expected
         * @tparam TError The error type of the input expected
         * @param expectedValue The expected-like container holding the value to append (copied)
         * @return A lambda that takes an expected-like container with a tuple and returns a new expected with the value appended
         *
         * @note Requires that TError is convertible to the error type of the output expected
         *
         * Example:
         * @code
         * std::expected<std::tuple<int>, Error> tuple_exp = std::tuple{42};
         * std::expected<std::string, Error> str_exp = std::string{"hello"};
         * auto result = tuple_exp | fxt::append(str_exp); // std::expected<std::tuple<int, std::string>, Error>
         * @endcode
         */
        template<template<typename, typename> class TExpected, typename TValue, typename TError>
            requires expected_like<TExpected<TValue, TError>>
        auto operator()(const TExpected<TValue, TError>& expectedValue) const
        {
            return [value = expectedValue]<template<typename, typename> class TExpectedOutput, typename TTuple, typename TErrorOutput>(
                       const TExpectedOutput<TTuple, TErrorOutput>& tupleExpected)
                requires std::convertible_to<TError, TErrorOutput>
            {
                return value ? tupleExpected.transform([value](const TTuple& tuple) { return fxt::tuple_append(tuple, *value); })
                             : typename TExpectedOutput<TTuple, TErrorOutput>::unexpected_type(value.error());
            };
        }

        /**
         * @brief Append a value from an expected-like container (rvalue) to a tuple in another expected-like container
         *
         * Move-optimized version for use with temporary expected values. Moves the value instead of copying
         * for better performance when the input expected is an rvalue.
         *
         * @tparam TExpected The expected-like template (e.g., std::expected)
         * @tparam TValue The type of value stored in the input expected
         * @tparam TError The error type of the input expected
         * @param expectedValue The expected-like container holding the value to append (moved)
         * @return A lambda that takes an expected-like container with a tuple and returns a new expected with the value appended
         *
         * @note Requires that TError is convertible to the error type of the output expected
         *
         * Example:
         * @code
         * std::expected<std::tuple<int>, Error> tuple_exp = std::tuple{42};
         * auto result = tuple_exp | fxt::append(make_expected_string()); // Moves temporary
         * @endcode
         */
        template<template<typename, typename> class TExpected, typename TValue, typename TError>
            requires expected_like<TExpected<TValue, TError>>
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
                        return fxt::tuple_append(tuple, std::move(*value)); })
                    : typename TExpectedOutput<TTuple, TErrorOutput>::unexpected_type(std::move(value.error()));
            };
        }

        /**
         * @brief Append a value from an optional-like container (lvalue) to a tuple in another optional-like container
         *
         * This overload handles optional types, returning an empty optional if either the input optional
         * or the tuple optional is empty. Follows standard optional monadic composition semantics.
         *
         * @tparam TOptional The optional-like template (e.g., std::optional, fxt::optional)
         * @tparam TValue The type of value stored in the input optional
         * @param optionalValue The optional-like container holding the value to append (copied)
         * @return A lambda that takes an optional-like container with a tuple and returns a new optional with the value appended
         *
         * Example:
         * @code
         * std::optional<std::tuple<int>> tuple_opt = std::tuple{42};
         * std::optional<std::string> str_opt = std::string{"hello"};
         * auto result = tuple_opt | fxt::append(str_opt); // std::optional<std::tuple<int, std::string>>
         * @endcode
         */
        template<template<typename> class TOptional, typename TValue>
            requires optional_like<TOptional<TValue>>
        auto operator()(const TOptional<TValue>& optionalValue) const
        {
            return [value = optionalValue]<template<typename> class TOptionalOutput, typename TTuple>(
                       const TOptionalOutput<TTuple>& tupleOptional)
            {
                if (!value) {
                    return TOptionalOutput<decltype(fxt::tuple_append(std::declval<TTuple>(), *value))>{};
                }
                return tupleOptional.transform([value](const TTuple& tuple) { return fxt::tuple_append(tuple, *value); });
            };
        }

        /**
         * @brief Append a value from an optional-like container (rvalue) to a tuple in another optional-like container
         *
         * Move-optimized version for use with temporary optional values. Moves the value instead of copying
         * for better performance when the input optional is an rvalue.
         *
         * @tparam TOptional The optional-like template (e.g., std::optional, fxt::optional)
         * @tparam TValue The type of value stored in the input optional
         * @param optionalValue The optional-like container holding the value to append (moved)
         * @return A lambda that takes an optional-like container with a tuple and returns a new optional with the value appended
         *
         * Example:
         * @code
         * std::optional<std::tuple<int>> tuple_opt = std::tuple{42};
         * auto result = tuple_opt | fxt::append(make_optional_string()); // Moves temporary
         * @endcode
         */
        template<template<typename> class TOptional, typename TValue>
            requires optional_like<TOptional<TValue>>
        auto operator()(TOptional<TValue>&& optionalValue) const
        {
            return [value = std::move(optionalValue)]<template<typename> class TOptionalOutput, typename TTuple>(
                       const TOptionalOutput<TTuple>& tupleOptional) mutable {
                if (!value) {
                    return TOptionalOutput<decltype(fxt::tuple_append(std::declval<TTuple>(), std::move(*value)))>{};
                }
                return tupleOptional.transform([value = std::move(value)](const TTuple& tuple) mutable {
                    return fxt::tuple_append(tuple, std::move(*value));
                });
            };
        }

        /**
         * @brief Append an arbitrary value to a tuple in a monadic container
         *
         * This overload handles regular values (not expected-like or optional-like) and appends them
         * to a tuple contained in either an expected or optional. Uses deducing this (C++23) to handle
         * both lvalue and rvalue references with a single overload.
         *
         * @tparam Self The type of this (deduced via explicit this parameter)
         * @tparam TValue The type of value to append (must not be expected-like or optional-like)
         * @param value The value to append (perfectly forwarded)
         * @return An overloaded lambda that works with both expected-like and optional-like containers
         *
         * @note The NotExpectedLike constraint ensures this overload doesn't conflict with the expected-like overloads
         * @note Requires C++23 support for deducing this
         *
         * Example:
         * @code
         * std::expected<std::tuple<int>, Error> tuple_exp = std::tuple{42};
         * auto result = tuple_exp | fxt::append(std::string{"hello"}); // Appends plain string
         *
         * std::optional<std::tuple<int>> tuple_opt = std::tuple{42};
         * auto result2 = tuple_opt | fxt::append(std::string{"world"}); // Also works with optional
         * @endcode
         */
        template<typename Self, typename TValue>
            requires (!expected_like<std::remove_cvref_t<TValue>>) && (!optional_like<std::remove_cvref_t<TValue>>)
        auto operator()(this Self&&, TValue&& _value)
        {
            return [_value = std::forward<TValue>(_value)]<typename TContainer>(const TContainer& container) mutable
            {
                return container.transform(
                    [_value = std::move(_value)](const auto& tuple) mutable {
                        return fxt::tuple_append(tuple, std::move(_value));
                    });
            };
        }
    };

    /**
     * @brief Global instance of AppendWrapper for convenient pipeline-style usage
     *
     * Used with the pipe operator (|) to append values to tuples in monadic containers.
     *
     * Example:
     * @code
     * auto result = std::expected<std::tuple<int>, Error>{std::tuple{42}}
     *     | fxt::append(std::string{"hello"})
     *     | fxt::append(3.14);
     * // result: std::expected<std::tuple<int, std::string, double>, Error>
     * @endcode
     */
    inline constexpr AppendWrapper mappend = {};

}    // namespace fxt