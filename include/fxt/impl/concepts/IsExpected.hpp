//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "../../Expected.hpp"
#include <type_traits>
#include <concepts>

namespace fxt::impl
{

    /**
     * @brief Concept to check if a type behaves like an expected type
     *
     * This concept verifies both structural requirements (type aliases) and
     * behavioral requirements (core operations) of an expected-like type.
     * It ensures the type has the necessary members and operations to be
     * used in monadic contexts.
     */
    template<typename T>
    concept expected_like = requires(T t) {
        // Required type aliases
        typename T::value_type;
        typename T::error_type;
        typename T::unexpected_type;

        // Core expected-like operations
        { t.has_value() } -> std::convertible_to<bool>;
        { static_cast<bool>(t) } -> std::convertible_to<bool>;

        // Value access operations (just check they exist, don't require convertibility for move-only types)
        t.value();
        t.error();
        *t;

        // Optional: Monadic operations (uncomment if you want to enforce these)
        // { t.transform(std::declval<std::function<int(typename T::value_type)>>()) };
        // { t.and_then(std::declval<std::function<T(typename T::value_type)>>()) };
    } && std::is_same_v<std::decay_t<T>, T>; // Ensure we work with decayed types

    /**
     * @brief Helper concept for checking if a type is specifically fxt::expected
     *
     * This is a more restrictive check that verifies the type is exactly
     * a specialization of fxt::expected, useful when you need to distinguish
     * between fxt::expected and other expected-like types.
     */
    template<typename T>
    struct is_fxt_expected : std::false_type {};

    template<typename TValue, typename TError>
    struct is_fxt_expected<fxt::expected<TValue, TError>> : std::true_type {};

    template<typename T>
    concept is_fxt_expected_v = is_fxt_expected<std::decay_t<T>>::value;

}    // namespace fxt::impl