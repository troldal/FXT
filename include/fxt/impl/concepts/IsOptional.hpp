//
// Created by kenne on 30-09-2025.
//

#pragma once

#include "../../Optional.hpp"
#include <type_traits>
#include <concepts>

namespace fxt::impl
{

    /**
     * @brief Concept to check if a type behaves like an optional type
     *
     * This concept verifies both structural requirements (type aliases) and
     * behavioral requirements (core operations) of an optional-like type.
     * It ensures the type has the necessary members and operations to be
     * used in monadic contexts.
     */
    template<typename T>
    concept optional_like = requires(T t) {
        // Required type alias
        typename T::value_type;

        // Core optional-like operations
        { t.has_value() } -> std::convertible_to<bool>;
        { static_cast<bool>(t) } -> std::convertible_to<bool>;

        // Value access operations
        { t.value() } -> std::convertible_to<typename T::value_type>;
        { *t } -> std::convertible_to<typename T::value_type>;

        // Reset operation (optional specific)
        t.reset();

        // Optional: Monadic operations (uncomment if you want to enforce these)
        // { t.transform(std::declval<std::function<int(typename T::value_type)>>()) };
        // { t.and_then(std::declval<std::function<T(typename T::value_type)>>()) };
    } && std::is_same_v<std::decay_t<T>, T>; // Ensure we work with decayed types

    /**
     * @brief Helper concept for checking if a type is specifically fxt::optional
     *
     * This is a more restrictive check that verifies the type is exactly
     * a specialization of fxt::optional, useful when you need to distinguish
     * between fxt::optional and other optional-like types (e.g., std::optional).
     */
    template<typename T>
    struct is_fxt_optional : std::false_type {};

    template<typename TValue>
    struct is_fxt_optional<fxt::optional<TValue>> : std::true_type {};

    template<typename T>
    concept is_fxt_optional_v = is_fxt_optional<std::decay_t<T>>::value;

    /**
     * @brief Helper to check if a type is std::optional specifically
     *
     * Useful for distinguishing between std::optional and other optional-like types
     * when you need library-specific behavior.
     */
    template<typename T>
    struct is_std_optional : std::false_type {};

    template<typename TValue>
    struct is_std_optional<std::optional<TValue>> : std::true_type {};

    template<typename T>
    concept is_std_optional_v = is_std_optional<std::decay_t<T>>::value;

}    // namespace fxt::impl
