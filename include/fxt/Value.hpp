//
// Created by kenne on 28/04/2025.
//

#pragma once

#include "impl/utils/TupleAppend.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"
#include "Expected.hpp"

namespace fxt
{

    struct ValueWrapper
    {
        auto operator()() const
        {
            return []<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.value();
            };
        }

    };

    inline constexpr ValueWrapper value = {};
}    // nam