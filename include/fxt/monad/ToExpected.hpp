//
// Created by kenne on 26/02/2025.
//

#pragma once

#include "Expected.hpp"
#include "Optional.hpp"

namespace fxt
{
    /**
     * @brief Converts a fxt::optional to fxt::expected, using a provided error value if empty.
     *
     * This operation extracts the value from an optional and wraps it in an expected.
     * If the optional is empty, an expected with the provided error is returned.
     *
     * @tparam TError The error type for the resulting expected
     * @param err The error value to use if the optional is empty
     * @return A callable that can be used with the pipe operator
     *
     * @example
     * auto result = fxt::optional<int>{42}
     *             | fxt::to_expected<std::string>("no value");
     * // result is fxt::expected<int, std::string>{42}
     *
     * auto error = fxt::optional<int>{fxt::nullopt}
     *            | fxt::to_expected<std::string>("no value");
     * // error is fxt::expected<int, std::string>{fxt::unexpected("no value")}
     */
    template<typename TError>
    inline constexpr auto to_expected(TError&& err)
    {
        return [err = std::forward<TError>(err)]<typename TValue>(const fxt::optional<TValue>& opt) -> fxt::expected<TValue, TError> {
            if (opt.has_value()) return opt.value();
            return fxt::unexpected(err);
        };
    }

}    // namespace fxt