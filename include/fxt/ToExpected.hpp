//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <optional>

namespace fxt
{

    template<typename TValue, typename TError = std::monostate>
    fxt::expected<TValue, TError> to_expected(const std::optional<TValue>& opt, const TError& err = TError())
    {
        if (opt) return *opt;
        return fxt::unexpected(err);
    }

}    // namespace fxt