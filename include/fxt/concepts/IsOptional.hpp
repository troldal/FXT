//
// Created by kenne on 10/04/2025.
//

#pragma once

#include "../monads/Optional.hpp"
#include <concepts>
#include <type_traits>

namespace fxt
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

}    // namespace fxt
