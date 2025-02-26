//
// Created by kenne on 25/02/2025.
//

#pragma once

#include <tuple>

namespace fxt::impl {



    // Trait to determine if a type is an instantiation of std::tuple.
    template<typename T>
    struct is_std_tuple : std::false_type
    {
    };

    template<typename... Args>
    struct is_std_tuple<std::tuple<Args...>> : std::true_type
    {
    };

    template<typename T>
    constexpr bool is_std_tuple_v = is_std_tuple<typename std::decay<T>::type>::value;

    // is_typle concept:
    template<typename T>
    concept is_tuple = is_std_tuple_v<T>;

    // Helper function that returns a tuple. If the argument is already a tuple, it is forwarded as is.
    // Otherwise, it is wrapped in a tuple.
    template<typename T>
    auto tuple_from(T&& arg)
    {
        if constexpr (is_std_tuple_v<T>) {
            return std::forward<T>(arg);
        }
        else {
            return std::make_tuple(std::forward<T>(arg));
        }
    }

    // Overload for a single argument:
    // If the argument is a tuple, it is returned unchanged,
    // otherwise it is wrapped in a tuple.
    template<typename T>
    auto tupelize(T&& arg)
    {
        return tuple_from(std::forward<T>(arg));
    }


}