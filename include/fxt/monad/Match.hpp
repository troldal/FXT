//
// Created by kenne on 04/10/2025.
//

#pragma once

#include "../impl/concepts/IsExpected.hpp"
#include "../impl/concepts/IsOptional.hpp"
#include <type_traits>
#include <utility>

namespace fxt
{
    namespace detail
    {
        /**
         * @brief Helper to match on expected types with proper value category handling
         */
        template<typename OnValue, typename OnError>
        struct match_expected_t
        {
            OnValue on_value;
            OnError on_error;

            // Rvalue reference overload for expected
            template<template<typename, typename> class TExpected, typename T, typename E>
                requires impl::expected_like<TExpected<T, E>>
            constexpr auto operator()(TExpected<T, E>&& exp) && -> decltype(auto)
            {
                if (exp.has_value()) {
                    return std::move(on_value)(std::move(*exp));
                } else {
                    return std::move(on_error)(std::move(exp.error()));
                }
            }

            // Const lvalue reference overload for expected
            template<template<typename, typename> class TExpected, typename T, typename E>
                requires impl::expected_like<TExpected<T, E>>
            constexpr auto operator()(const TExpected<T, E>& exp) const& -> decltype(auto)
            {
                if (exp.has_value()) {
                    return on_value(*exp);
                } else {
                    return on_error(exp.error());
                }
            }

            // Non-const lvalue reference overload for expected
            template<template<typename, typename> class TExpected, typename T, typename E>
                requires impl::expected_like<TExpected<T, E>>
            constexpr auto operator()(TExpected<T, E>& exp) & -> decltype(auto)
            {
                if (exp.has_value()) {
                    return on_value(*exp);
                } else {
                    return on_error(exp.error());
                }
            }
        };

        /**
         * @brief Helper to match on optional types with proper value category handling
         */
        template<typename OnValue, typename OnNone>
        struct match_optional_t
        {
            OnValue on_value;
            OnNone on_none;

            // Rvalue reference overload for optional
            template<template<typename> class TOptional, typename T>
                requires impl::optional_like<TOptional<T>>
            constexpr auto operator()(TOptional<T>&& opt) && -> decltype(auto)
            {
                if (opt.has_value()) {
                    return std::move(on_value)(std::move(*opt));
                } else {
                    return std::move(on_none)();
                }
            }

            // Const lvalue reference overload for optional
            template<template<typename> class TOptional, typename T>
                requires impl::optional_like<TOptional<T>>
            constexpr auto operator()(const TOptional<T>& opt) const& -> decltype(auto)
            {
                if (opt.has_value()) {
                    return on_value(*opt);
                } else {
                    return on_none();
                }
            }

            // Non-const lvalue reference overload for optional
            template<template<typename> class TOptional, typename T>
                requires impl::optional_like<TOptional<T>>
            constexpr auto operator()(TOptional<T>& opt) & -> decltype(auto)
            {
                if (opt.has_value()) {
                    return on_value(*opt);
                } else {
                    return on_none();
                }
            }
        };
    }

    /**
     * @brief Pattern matching for expected and optional types
     *
     * Provides functional pattern matching that exhaustively handles all cases.
     * For expected types, provides handlers for both success and error cases.
     * For optional types, provides handlers for value present and empty cases.
     *
     * Both handlers must return the same type (or convertible types).
     * The function properly forwards value categories (lvalue/rvalue references).
     *
     * @section Usage with expected
     * @code
     * auto result = safe_divide(10, 2)
     *     | fxt::match(
     *         [](int value) { return "Success: " + std::to_string(value); },
     *         [](const std::string& err) { return "Error: " + err; }
     *     );
     *
     * // Or with function call syntax
     * auto result2 = fxt::match(
     *     safe_divide(10, 0),
     *     [](int value) { return value * 2; },
     *     [](const std::string& err) { return -1; }
     * );
     * @endcode
     *
     * @section Usage with optional
     * @code
     * auto result = parse_int("42")
     *     | fxt::match(
     *         [](int value) { return value * 2; },
     *         []() { return 0; }  // Called when empty
     *     );
     * @endcode
     */
    inline constexpr auto match = []<typename OnValue, typename OnError>(OnValue&& on_value, OnError&& on_error)
    {
        // Return a callable that can work with both expected and optional
        return [on_value = std::forward<OnValue>(on_value),
                on_error = std::forward<OnError>(on_error)]<typename TMonad>(TMonad&& monad) mutable
        {
            using TDecayed = std::remove_cvref_t<TMonad>;

            // Check if it's an expected-like type
            if constexpr (requires {
                typename TDecayed::value_type;
                typename TDecayed::error_type;
                monad.has_value();
                monad.error();
            })
            {
                // Handle expected
                using match_t = detail::match_expected_t<
                    std::decay_t<decltype(on_value)>,
                    std::decay_t<decltype(on_error)>
                >;

                return std::move(match_t{
                    std::move(on_value),
                    std::move(on_error)
                })(std::forward<TMonad>(monad));
            }
            // Check if it's an optional-like type
            else if constexpr (requires {
                typename TDecayed::value_type;
                monad.has_value();
            })
            {
                // Handle optional (on_error acts as on_none)
                using match_t = detail::match_optional_t<
                    std::decay_t<decltype(on_value)>,
                    std::decay_t<decltype(on_error)>
                >;

                return std::move(match_t{
                    std::move(on_value),
                    std::move(on_error)
                })(std::forward<TMonad>(monad));
            }
            else
            {
                static_assert(sizeof(TMonad) == 0, "match requires an expected-like or optional-like type");
            }
        };
    };

}    // namespace fxt
