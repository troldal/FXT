//
// Created by kenne on 07/04/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Monadic and_then operation for types that provide an and_then() member function
     *
     * This is a generic function object that works with any type that has an and_then() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided function to
     * the container's and_then() member function, enabling monadic composition.
     *
     * @tparam TFunction The type of the function to apply via and_then()
     * @param f The function to forward to the container's and_then() member function
     * @return A callable that accepts any container with an and_then() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named and_then() that accepts the provided function
     * - Supports both lvalue and rvalue references
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
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.and_then(f); }
        { return std::forward<TContainer>(container).and_then(f); };
    };
}    // namespace fxt