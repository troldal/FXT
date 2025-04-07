//
// Created by kenne on 23/02/2025.
//

#pragma once

#include "impl/utils/TupleAppend.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"

namespace fxt
{

    struct TransformWrapper
    {
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                if constexpr (impl::IsExpected<std::invoke_result_t<TFunction, TValue>>)
                    return ex.and_then(f);
                else
                    return ex.transform(f);
            };
        }

    };

    inline constexpr TransformWrapper transform = {};
}    // namespace fxt