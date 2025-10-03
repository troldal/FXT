//
// Created by kenne on 07/04/2025.
//

#pragma once

#include "Overload.hpp"
#include "Optional.hpp"
#include "Expected.hpp"

namespace fxt
{
    /**
     * @brief Monadic and_then operation for both fxt::expected and fxt::optional
     *
     * Provides a unified way to chain operations on monadic types using and_then.
     * Works with both fxt::expected and fxt::optional, applying the function only
     * when the container holds a value or is in success state.
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = parse_int("42") | fxt::and_then(square);
     * auto result2 = divide(10, 2) | fxt::and_then(add_five);
     *
     * // Function call syntax
     * auto result3 = fxt::and_then(square)(parse_int("42"));
     *
     * // Chaining operations
     * auto result4 = parse_int("3")
     *              | fxt::and_then(square)
     *              | fxt::and_then(double_it);
     * @endcode
     */
    inline constexpr auto and_then = []<typename TFunction>(TFunction&& f) {
        return overload(
            [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.and_then(f);
            },
            [f = std::forward<TFunction>(f)]<typename TValue>(const fxt::optional<TValue>& opt) {
                return opt.and_then(f);
            }
        );
    };
}