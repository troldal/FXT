//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Monadic value_or operation for types that provide a value_or() member function
     *
     * This is a generic function object that works with any type that has a value_or() member
     * function, such as fxt::expected and fxt::optional. It forwards the provided alternative value
     * to the container's value_or() member function, enabling a functional approach to providing
     * default values.
     *
     * @tparam TAlternative The type of the alternative value
     * @param val The alternative value to use if the container is empty or contains an error
     * @return A callable that accepts any container with a value_or() member function
     *
     * @section Concepts
     * The returned callable accepts any type that satisfies:
     * - Has a member function named value_or() that accepts the provided alternative value
     * - Supports both lvalue and rvalue references
     *
     * @section Usage
     * @code
     * // Pipe operator usage with expected
     * auto result1 = divide(10, 0) | fxt::value_or(42);
     *
     * // Pipe operator usage with optional
     * auto result2 = parse_int("abc") | fxt::value_or(0);
     *
     * // Function call syntax
     * auto result3 = fxt::value_or(100)(maybe_value);
     *
     * // Chaining operations
     * auto result4 = parse_int("123")
     *              | fxt::transform([](int x) { return x * 2; })
     *              | fxt::value_or(0);
     * @endcode
     */
    inline constexpr auto value_or = []<typename TAlternative>(const TAlternative& val) {
        return [val]<typename TContainer>(TContainer&& container)
            requires requires(TContainer&& c) { c.value_or(val); }
        { return std::forward<TContainer>(container).value_or(val); };
    };

}    // namespace fxt