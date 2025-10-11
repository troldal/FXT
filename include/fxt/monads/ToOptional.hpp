//
// Created by kenne on 26/02/2025.
//

#pragma once

#include "Expected.hpp"
#include "Optional.hpp"

namespace fxt
{
    /**
     * @brief Converts a fxt::expected to fxt::optional, discarding error information.
     *
     * This operation extracts the success value from an expected and wraps it in an optional.
     * If the expected contains an error, an empty optional is returned.
     *
     * @return A callable that can be used with the pipe operator
     *
     * @example
     * auto result = fxt::expected<int, std::string>{42}
     *             | fxt::to_optional();
     * // result is fxt::optional<int>{42}
     *
     * auto error = fxt::expected<int, std::string>{fxt::unexpected("error")}
     *            | fxt::to_optional();
     * // error is fxt::optional<int>{fxt::nullopt}
     */
    inline constexpr auto to_optional = []() {
        return []<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) -> fxt::optional<TValue> {
            if (ex.has_value()) return ex.value();
            return fxt::nullopt;
        };
    };

}    // namespace fxt