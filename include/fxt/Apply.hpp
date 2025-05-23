//
// Created by kenne on 25/02/2025.
//

#pragma once


#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Append.hpp"
#include <string>

namespace fxt
{
    struct ApplyWrapper
    {
        template<typename TFunction>
        auto operator()(const TFunction& f) const
        {
            return [f]<template<typename, typename> class TExpected, typename... TElems, typename TError>(const
            TExpected<std::tuple<TElems...>, TError>& tuple) {
                 if constexpr (impl::expected_like<std::invoke_result_t<TFunction, TElems...>>)
                     return tuple
                        ? append(std::apply(f, *tuple))(tuple)
                        : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tuple.error());
                 else if constexpr (std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
                     return tuple.transform([&](const std::tuple<TElems...>& t) { std::apply(f, *tuple); return t; });
                 else
                     return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, *tuple));
                     });
            };
        }
    };

    inline constexpr ApplyWrapper apply = {};

}    // namespace fxt

//
// Created by kenne on 25/02/2025.
//

// #pragma once
//
// #include "Append.hpp"
// #include "impl/concepts/IsExpected.hpp"
// #include "impl/utils/TupleAppend.hpp"
// #include <string>
// #include <tuple>
// #include <type_traits>
// #include <utility>
//
// namespace fxt
// {
//     namespace impl
//     {
//         // Helper: check if all tuple elements are expected_like
//
//         // Primary template: false for non-tuple types
//         template<typename T>
//         struct all_expected_like : std::false_type
//         {
//         };
//
//         // Specialization for std::tuple<Ts...>
//         template<typename... Ts>
//         struct all_expected_like<std::tuple<Ts...>> : std::bool_constant<expected_like<Ts...>>
//         {
//         };
//
//         // Helper: get first error from tuple of expected-like objects
//         template<typename Tuple, std::size_t... Is>
//         auto first_unexpected(const Tuple& t, std::index_sequence<Is...>)
//         {
//             using error_type      = std::common_type_t<typename std::tuple_element_t<Is, Tuple>::error_type...>;
//             const error_type* err = nullptr;
//             (..., (err = (!std::get<Is>(t) && !err) ? &std::get<Is>(t).error() : err));
//             return err;
//         }
//         template<typename Tuple>
//         auto first_unexpected(const Tuple& t)
//         {
//             return first_unexpected(t, std::make_index_sequence<std::tuple_size_v<Tuple>> {});
//         }
//
//         // Helper: extract values from tuple of expected-like
//         template<typename Tuple, std::size_t... Is>
//         auto tuple_values(const Tuple& t, std::index_sequence<Is...>)
//         {
//             return std::make_tuple((*std::get<Is>(t))...);
//         }
//         template<typename Tuple>
//         auto tuple_values(const Tuple& t)
//         {
//             return tuple_values(t, std::make_index_sequence<std::tuple_size_v<Tuple>> {});
//         }
//     }    // namespace impl
//
//     struct ApplyWrapper
//     {
//         template<typename TFunction>
//         auto operator()(const TFunction& f) const
//         {
//             return [f]<template<typename, typename> class TExpected, typename... TElems, typename TError>(
//                        const TExpected<std::tuple<TElems...>, TError>& tuple) {
//                 using result_t = std::invoke_result_t<TFunction, TElems...>;
//                 if constexpr (impl::expected_like<result_t>) {
//                     return tuple ? append(std::apply(f, *tuple))(tuple) : typename result_t::unexpected_type(tuple.error());
//                 }
//                 else if constexpr (std::same_as<result_t, void>) {
//                     return tuple.transform([&](const std::tuple<TElems...>& t) {
//                         std::apply(f, t);
//                         return t;
//                     });
//                 }
//                 else if constexpr (std::is_same_v<result_t, std::tuple<>>) {
//                     return tuple;
//                 }
//                 else if constexpr (impl::all_expected_like<result_t>::value) {
//                     if (!tuple)
//                         return TExpected<decltype(std::tuple_cat(*tuple, impl::tuple_values(std::declval<result_t>()))),
//                                          TError>::unexpected(tuple.error());
//                     auto result_tuple = std::apply(f, *tuple);
//                     auto err          = impl::first_unexpected(result_tuple);
//                     if (err) return TExpected<decltype(std::tuple_cat(*tuple, impl::tuple_values(result_tuple))), TError>::unexpected(*err);
//                     return append(impl::tuple_values(result_tuple))(tuple);
//                 }
//                 else if constexpr (impl::is_std_tuple<result_t>::value && !impl::all_expected_like<result_t>::value) {
//                     return tuple.transform([&](const std::tuple<TElems...>& t) { return std::tuple_cat(t, std::apply(f, t)); });
//                 }
//                 else {
//                     return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, t)); });
//                 }
//             };
//         }
//     };
//
//     inline constexpr ApplyWrapper apply = {};
//
// }    // namespace fxt