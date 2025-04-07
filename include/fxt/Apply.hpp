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
            return [f]<template<typename, typename> class TExpected, typename... TElems, typename TError>(const TExpected<std::tuple<TElems...>, TError>& tuple) {
                 if constexpr (impl::expected_like<std::invoke_result_t<TFunction, TElems...>>)
                     return tuple ? append(std::apply(f, *tuple))(tuple) : typename std::invoke_result_t<TFunction, TElems...>::unexpected_type(tuple.error());
                 else if constexpr (std::same_as<std::invoke_result_t<TFunction, TElems...>, void>)
                     return tuple.transform([&](const std::tuple<TElems...>& t) { std::apply(f, *tuple); return t; });
                 else
                     return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, *tuple)); });
            };
        }
    };

    inline constexpr ApplyWrapper apply = {};

}    // namespace fxt
