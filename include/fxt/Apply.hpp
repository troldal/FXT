//
// Created by kenne on 25/02/2025.
//

#pragma once


#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Append.hpp"
#include <string>
#include <type_traits>
#include <concepts>

namespace fxt
{

    namespace impl
    {
        // Helper to extract value type from expected-like objects
        template<typename T>
        struct expected_value_type
        {
            using type = T;
        };

        template<typename T>
            requires expected_like<T>
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

    struct ApplyWrapper
    {
        template<typename TFunction>
        auto operator()(const TFunction& f) const
        {
            return [f]<template<typename, typename> class TExpected, typename... TElems, typename TError>(const
            TExpected<std::tuple<TElems...>, TError>& tup)
            -> TExpected<std::tuple<TElems..., impl::processed_invoke_result_t<TFunction, TElems...>>, TError>
            {
                 if constexpr (impl::expected_like<std::invoke_result_t<TFunction, TElems...>>)
                     return tup
                        ? append(std::apply(f, *tup))(tup)
                        : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tup.error());
                 else if constexpr (std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
                     return tup.transform([&](const std::tuple<TElems...>& t) { std::apply(f, *tup); return t; });
                 else
                     return tup.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, *tup));
                     });
            };
        }
    };

    inline constexpr ApplyWrapper apply = {};

}    // namespace fxt
