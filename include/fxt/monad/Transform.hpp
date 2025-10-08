//
// Created by kenne on 23/02/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Monadic transform operation for types that provide a transform() member function
     *
     * This is a generic function object that works with any type that has a transform() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided function to
     * the container's transform() member function, enabling monadic value transformation.
     *
     * @tparam TFunction The type of the function to apply via transform()
     * @param f The function to forward to the container's transform() member function
     * @return A callable that accepts any container with a transform() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named transform() that accepts the provided function
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = parse_int("42") | fxt::transform(square);
     * auto result2 = divide(10, 2) | fxt::transform(to_string);
     *
     * // Function call syntax
     * auto result3 = fxt::transform(square)(parse_int("42"));
     *
     * // Chaining operations
     * auto result4 = parse_int("3")
     *              | fxt::transform(square)
     *              | fxt::transform(double_it)
     *              | fxt::transform(to_string);
     * @endcode
     */
    inline constexpr auto transform = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.transform(f); }
        { return std::forward<TContainer>(container).transform(f); };
    };
}    // namespace fxt