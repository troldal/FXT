//
// Created by kenne on 10/04/2025.
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

        // Value access operations (just check they exist, don't require convertibility for move-only types)
        t.value();
        *t;

    } && std::is_same_v<std::decay_t<T>, T>; // Ensure we work with decayed types

    /**
     * @brief Helper concept for checking if a type is specifically fxt::optional
     *
     * This is a more restrictive check that verifies the type is exactly
     * a specialization of fxt::optional, useful when you need to distinguish
     * between fxt::optional and other optional-like types.
     */
    template<typename T>
    struct is_fxt_optional : std::false_type {};

    template<typename T>
    struct is_fxt_optional<fxt::optional<T>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_fxt_optional_v = is_fxt_optional<T>::value;

}    // namespace fxt::impl
