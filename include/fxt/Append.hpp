//
// Created by kenne on 23/02/2025.
//

#pragma once

// #include <Boost/hana.hpp>
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Expected.hpp"
#include <string>

namespace fxt
{
    template<typename T>
    concept HasValueType = requires { typename T::value_type; };



    // template<typename F, typename Tuple>
    // struct is_callable_with_tuple;
    //
    // template<typename F, typename... Args>
    // struct is_callable_with_tuple<F, std::tuple<Args...>> : std::is_invocable<F, Args...> {};
    //
    // // Variable template for easier usage.
    // template<typename F, typename Tuple>
    // constexpr bool is_callable_with_tuple_v = is_callable_with_tuple<F, std::decay_t<Tuple>>::value;
    //
    // template<typename F, typename Tuple>
    // concept IsCallableWithTuple = is_callable_with_tuple_v<F, Tuple>;


    struct AppendWrapper
    {
        template<typename TValue>
        auto operator()(const TValue& val) const
        {
            // return [val, this]<typename TTuple, typename TError>(const std::expected<TTuple, TError>& tuple) {
            //     if constexpr (std::invocable<TValue, TTuple> and IsExpected<std::invoke_result_t<TValue, TTuple>>)
            //         return tuple ? this->operator()(std::invoke(val, *tuple))(tuple) : std::unexpected(tuple.error());
            //
            //     else if constexpr (std::invocable<TValue, TTuple> and !IsExpected<std::invoke_result_t<TValue, TTuple>>)
            //         return tuple.transform([&](const TTuple& t) { return tuple_append(t, std::invoke(val, *tuple)); });
            //
            //     else
            //         return tuple.transform([&](const TTuple& t) { return tuple_append(t, val); });
            // };

            return [val, this]<typename... TElems, typename TError>(const fxt::expected<std::tuple<TElems...>, TError>& tuple) {
                // if constexpr (std::invocable<TValue, TElems...> and impl::IsExpected<std::invoke_result_t<TValue, TElems...>>)
                //     return tuple ? this->operator()(std::apply(val, *tuple))(tuple) : std::unexpected(tuple.error());
                //
                // else if constexpr (std::invocable<TValue, TElems...> and !impl::IsExpected<std::invoke_result_t<TValue, TElems...>>)
                //     return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(val, *tuple)); });
                //
                // else
                    return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, val); });
            };
        }

        template<typename TValue>
            requires HasValueType<TValue>
        auto operator()(const TValue& val) const
        {
            return [val]<template<typename,typename> class TExpected, typename TTuple, typename TError>(const TExpected<TTuple, TError>& tuple)
                requires impl::expected_like<TExpected<TTuple, TError>>
            {
                return tuple.transform([&](const TTuple& t) { return impl::tuple_append(t, val); });
            };
        }

        template<template<typename,typename> class TExpected, typename T, typename E>
            requires impl::expected_like<TExpected<T, E>>
        auto operator()(const TExpected<T, E>& val) const
        {
            using TValue = typename TExpected<T, E>::value_type;
            using TError = typename TExpected<T, E>::error_type;
            using TUnexpected = typename TExpected<T, E>::unexpected_type;

            return [val]<template<typename,typename> class TExpectedOut, typename TTuple, typename TErrorOut>(const TExpectedOut<TTuple, TErrorOut>& tuple)
                requires std::convertible_to<TError, TErrorOut>
            {
                return val
                    ? tuple.transform([val](const TTuple& t) { return impl::tuple_append(t, *val); })
                    : typename TExpectedOut<TTuple, TError>::unexpected_type(val.error());
            };
        }
    };

    inline constexpr AppendWrapper append = {};

}    // namespace fxt
