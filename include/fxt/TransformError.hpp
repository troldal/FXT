//
// Created by kenne on 23/02/2025.
//

#pragma once

#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"
#include <expected>

namespace fxt
{

    struct TransformErrorWrapper
    {
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const std::expected<TValue, TError>& ex) {
                    return ex.transform_error(f);
            };
        }

    };

    inline constexpr TransformErrorWrapper transform_error = {};
}    // namespace fxt