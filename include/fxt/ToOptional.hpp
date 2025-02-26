//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <expected>
#include <optional>

namespace fxt
{

    template<typename TValue, typename TError>
    std::optional<TValue> to_optional(const std::expected<TValue, TError>& ex)
    {
        if (ex.has_value()) return ex.value();
        return std::nullopt;
    }

}    // namespace fxt