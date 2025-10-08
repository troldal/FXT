//
// Created by kenne on 23/02/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Monadic transform_error operation for types that provide a transform_error() member function
     *
     * This is a generic function object that works with any type that has a transform_error() member
     * function, such as fxt::expected. It forwards the provided function to the container's
     * transform_error() member function, enabling monadic error transformation.
     *
     * @tparam TFunction The type of the function to apply via transform_error()
     * @param f The function to forward to the container's transform_error() member function
     * @return A callable that accepts any container with a transform_error() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named transform_error() that accepts the provided function
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = divide(10, 0) | fxt::transform_error(to_string);
     * auto result2 = parse_int("abc") | fxt::transform_error(wrap_error);
     *
     * // Function call syntax
     * auto result3 = fxt::transform_error(to_string)(divide(10, 0));
     *
     * // Chaining operations
     * auto result4 = parse_int("abc")
     *              | fxt::transform_error(add_context)
     *              | fxt::transform_error(log_error)
     *              | fxt::transform_error(to_user_message);
     * @endcode
     */
    inline constexpr auto transform_error = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.transform_error(f); }
        { return std::forward<TContainer>(container).transform_error(f); };
    };
}    // namespace fxt