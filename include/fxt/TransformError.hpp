//
// Created by kenne on 23/02/2025.
//

#pragma once

#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"

namespace fxt
{

    struct TransformErrorWrapper
    {
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                    return ex.transform_error(f);
            };
        }

    };

    inline constexpr TransformErrorWrapper transform_error = {};
}    // namespace fxt