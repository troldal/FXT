//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "impl/concepts/IsExpected.hpp"
#include "impl/concepts/IsOptional.hpp"
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
     * @brief Monadic apply operation for applying functions to tuple values in monadic containers
     *
     * This function object provides a uniform interface for applying a function to the contents of a tuple
     * that is wrapped in a monadic container (expected-like or optional-like). It handles three distinct cases
     * through an overload set:
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
    inline constexpr auto apply = []<typename TFunction>(TFunction&& function) {
        return overload{
            // ========================================================================
            // Case 1a: Optional-like container + Function returning monadic type
            // ========================================================================
            // This overload is selected when:
            // - The container is optional-like (e.g., fxt::optional<std::tuple<...>>)
            // - The function returns an expected-like or optional-like type (monadic bind)
            //
            // Purpose: Enables chaining operations that may fail. When the optional
            // has a value, it extracts the tuple elements, applies the function, and
            // then appends the unwrapped result to the tuple. If the optional is empty
            // or the function returns an error/nullopt, propagates the failure.
            //
            // Example:
            //   fxt::optional<std::tuple<int>>{5}
            //   | fxt::apply([](int x) -> fxt::optional<int> {
            //       return x > 0 ? fxt::optional{x * 2} : fxt::nullopt;
            //     })
            //   // Result: fxt::optional<std::tuple<int, int>>{std::tuple{5, 10}}
            [function = std::forward<TFunction>(function)]<template<typename...> class TContainer, typename... TElems>(
                TContainer<std::tuple<TElems...>>&& tupleContainer)
                -> TContainer<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>>
                requires impl::optional_like<TContainer<std::tuple<TElems...>>>
                      && returns_monadic<TFunction, TElems...>
            {
                return tupleContainer
                    ? append(std::apply(function, *std::forward<decltype(tupleContainer)>(tupleContainer)))(std::forward<decltype(tupleContainer)>(tupleContainer))
                    : fxt::nullopt;
            },

            // ========================================================================
            // Case 1b: Expected-like container + Function returning monadic type
            // ========================================================================
            // This overload is selected when:
            // - The container is expected-like (e.g., fxt::expected<std::tuple<...>, Error>)
            // - The function returns an expected-like or optional-like type (monadic bind)
            //
            // Purpose: Enables error-aware chaining. When the expected has a value,
            // it extracts the tuple elements, applies the function, and appends the
            // unwrapped result to the tuple. If the expected contains an error or the
            // function returns an error, propagates the error with proper error type handling.
            //
            // Example:
            //   fxt::expected<std::tuple<std::string>, Error>{"42"}
            //   | fxt::apply([](const std::string& s) -> fxt::expected<int, Error> {
            //       try { return std::stoi(s); }
            //       catch(...) { return unexpected(Error{"Parse failed"}); }
            //     })
            //   // Result: fxt::expected<std::tuple<std::string, int>, Error>{std::tuple{"42", 42}}
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
                requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
                      && returns_monadic<TFunction, TElems...>
            {
                return tupleExpected
                    ? append(std::apply(function, *std::forward<decltype(tupleExpected)>(tupleExpected)))(std::forward<decltype(tupleExpected)>(tupleExpected))
                    : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tupleExpected.error());
            },

            // ========================================================================
            // Case 2a: Optional-like container + Function returning void
            // ========================================================================
            // This overload is selected when:
            // - The container is optional-like (e.g., fxt::optional<std::tuple<...>>)
            // - The function returns void (used for side effects)
            //
            // Purpose: Allows executing side effects (like logging, validation, etc.)
            // without modifying the tuple. The function is called with the tuple elements
            // if present, but the original tuple is returned unchanged.
            //
            // Example:
            //   fxt::optional<std::tuple<int, std::string>>{std::tuple{42, "test"}}
            //   | fxt::apply([](int x, const std::string& s) {
            //       std::cout << "Processing: " << x << ", " << s << std::endl;
            //     })
            //   // Result: fxt::optional<std::tuple<int, std::string>>{std::tuple{42, "test"}}
            //   //         (side effect: prints to console)
            [function = std::forward<TFunction>(function)]<template<typename...> class TContainer, typename... TElems>(
                TContainer<std::tuple<TElems...>>&& tupleContainer)
                -> TContainer<std::tuple<TElems...>>
                requires impl::optional_like<TContainer<std::tuple<TElems...>>>
                      && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
            {
                return std::forward<decltype(tupleContainer)>(tupleContainer).transform([&](const std::tuple<TElems...>& tuple) {
                    std::apply(function, tuple);
                    return tuple;
                });
            },

            // ========================================================================
            // Case 2b: Expected-like container + Function returning void
            // ========================================================================
            // This overload is selected when:
            // - The container is expected-like (e.g., fxt::expected<std::tuple<...>, Error>)
            // - The function returns void (used for side effects)
            //
            // Purpose: Allows executing side effects while preserving error handling.
            // The function is called with the tuple elements only if the expected has
            // a value, otherwise the error is propagated. The original tuple is returned
            // unchanged if successful.
            //
            // Example:
            //   fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
            //   | fxt::apply([](int a, int b) {
            //       if (a + b != 7) throw std::runtime_error("Validation failed");
            //     })
            //   // Result: fxt::expected<std::tuple<int, int>, Error>{std::tuple{3, 4}}
            //   //         (or error if validation fails)
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems...>, TError>
                requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
                      && std::same_as<std::invoke_result_t<TFunction, TElems...>, void>
            {
                return std::forward<decltype(tupleExpected)>(tupleExpected).transform([&](const std::tuple<TElems...>& tuple) {
                    std::apply(function, tuple);
                    return tuple;
                });
            },

            // ========================================================================
            // Case 3a: Optional-like container + Function returning regular value
            // ========================================================================
            // This overload is selected when:
            // - The container is optional-like (e.g., fxt::optional<std::tuple<...>>)
            // - The function returns a regular (non-monadic, non-void) value
            //
            // Purpose: Standard case for computation. When the optional has a value,
            // it extracts the tuple elements, applies the function to compute a result,
            // and appends that result to the tuple. If the optional is empty, propagates
            // the empty state.
            //
            // This is the most common use case - applying a pure function to compute
            // a new value from the existing tuple elements.
            //
            // Example:
            //   fxt::optional<std::tuple<int, int>>{std::tuple{3, 4}}
            //   | fxt::apply([](int a, int b) { return a + b; })
            //   // Result: fxt::optional<std::tuple<int, int, int>>{std::tuple{3, 4, 7}}
            [function = std::forward<TFunction>(function)]<template<typename...> class TContainer, typename... TElems>(
                TContainer<std::tuple<TElems...>>&& tupleContainer)
                -> TContainer<std::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>>
                requires impl::optional_like<TContainer<std::tuple<TElems...>>>
            {
                return std::forward<decltype(tupleContainer)>(tupleContainer).transform([&](const std::tuple<TElems...>& tuple) {
                    return impl::tuple_append(tuple, std::apply(function, tuple));
                });
            },

            // ========================================================================
            // Case 3b: Expected-like container + Function returning regular value
            // ========================================================================
            // This overload is selected when:
            // - The container is expected-like (e.g., fxt::expected<std::tuple<...>, Error>)
            // - The function returns a regular (non-monadic, non-void) value
            //
            // Purpose: Standard case for error-aware computation. When the expected has
            // a value, it extracts the tuple elements, applies the function to compute
            // a result, and appends that result to the tuple. If the expected contains
            // an error, propagates the error with proper type handling.
            //
            // This is the most common use case for expected - applying a function that
            // can't fail itself but needs to respect existing error states.
            //
            // Example:
            //   fxt::expected<std::tuple<int, int>, Error>{std::tuple{5, 10}}
            //   | fxt::apply([](int a, int b) { return a * b; })
            //   // Result: fxt::expected<std::tuple<int, int, int>, Error>{std::tuple{5, 10, 50}}
            [function = std::forward<TFunction>(function)]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
                TExpected<std::tuple<TElems...>, TError>&& tupleExpected)
                -> TExpected<std::tuple<TElems..., std::invoke_result_t<TFunction, TElems...>>, TError>
                requires impl::expected_like<TExpected<std::tuple<TElems...>, TError>>
            {
                return std::forward<decltype(tupleExpected)>(tupleExpected).transform([&](const std::tuple<TElems...>& tuple) {
                    return impl::tuple_append(tuple, std::apply(function, tuple));
                });
            }
        };
    };

}    // namespace fxt
