//
// Created by kenne on 28/02/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Monadic or_else operation for types that provide an or_else() member function
     *
     * This is a generic function object that works with any type that has an or_else() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided function to
     * the container's or_else() member function, enabling monadic error handling composition.
     *
     * @tparam TFunction The type of the function to apply via or_else()
     * @param f The function to forward to the container's or_else() member function
     * @return A callable that accepts any container with an or_else() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named or_else() that accepts the provided function
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage
     * auto result1 = divide(10, 0) | fxt::or_else(handle_error);
     * auto result2 = parse_int("abc") | fxt::or_else(return_default);
     *
     * // Function call syntax
     * auto result3 = fxt::or_else(handle_error)(divide(10, 0));
     *
     * // Chaining operations
     * auto result4 = parse_int("abc")
     *              | fxt::or_else(try_parse_hex)
     *              | fxt::or_else(return_zero);
     * @endcode
     */
    inline constexpr auto or_else = []<typename TFunction>(const TFunction& f) {
        return [f]<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.or_else(f); }
        { return std::forward<TContainer>(container).or_else(f); };
    };

}    // namespace fxt