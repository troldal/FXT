//
// Created by kenne on 24/02/2025.
//

#pragma once

#include "impl/utils/TupleAppend.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"
#include <expected>

namespace fxt
{

    struct ValueOrWrapper
    {
        template<typename TAlternative>
        auto operator()(TAlternative&& val) const
        {
            return [val = std::forward<TAlternative>(val)]<typename TValue, typename TError>(const std::expected<TValue, TError>& ex) {
                return ex.value_or(val);
            };
        }

    };

    inline constexpr ValueOrWrapper value_or = {};
}    // namespace fxt