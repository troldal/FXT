//
// Created by kenne on 25/02/2025.
//

#pragma once


#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"
#include "Append.hpp"
#include <expected>
#include <string>

namespace fxt
{
    struct ApplyWrapper
    {
        template<typename TFunction>
        auto operator()(const TFunction& f) const
        {
            return [f]<typename... TElems, typename TError>(const std::expected<std::tuple<TElems...>, TError>& tuple) {
                 if constexpr (impl::IsExpected<std::invoke_result_t<TFunction, TElems...>>)
                     //return tuple ? this->operator()(std::apply(f, *tuple))(tuple) : std::unexpected(tuple.error());
                     return tuple ? append(std::apply(f, *tuple))(tuple) : std::unexpected(tuple.error());

                 else
                     return tuple.transform([&](const std::tuple<TElems...>& t) { return impl::tuple_append(t, std::apply(f, *tuple)); });
            };
        }
    };

    inline constexpr ApplyWrapper apply = {};

}    // namespace fxt
