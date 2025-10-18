/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

#pragma once

#include <concepts>
#include <memory>
#include <type_traits>
#include <utility>

namespace fxt
{
    /**
     * @brief A wrapper that provides move-enabled const semantics for contained values
     *
     * The `immutable` class provides a "move-enabled const" - wrapping a value to make it
     * immutable while still allowing the wrapper itself to be moved and reassigned. This
     * enables building objects gradually and then wrapping them in `immutable` to prevent
     * further modification, while still maintaining efficient move semantics.
     *
     * ## Key Distinction from `const`
     *
     * Unlike the `const` keyword which makes both the variable and its value immutable,
     * `fxt::immutable` makes only the **contained value** immutable, not the wrapper itself.
     * This allows:
     * - **Reassignment**: You can assign a new `immutable` to an existing variable
     * - **Moving**: The wrapper can be moved efficiently without copying the value
     * - **Gradual construction**: Build an object mutably, then wrap it for immutable use
     *
     * ## Usage Pattern
     *
     * ```cpp
     * // Build an object mutably
     * std::vector<int> data;
     * data.push_back(1);
     * data.push_back(2);
     * data.push_back(3);
     *
     * // Wrap it to prevent modification
     * fxt::immutable<std::vector<int>> immut_data(std::move(data));
     *
     * // Can read but not modify
     * size_t size = immut_data->size();  // OK
     * // immut_data->push_back(4);       // Error: push_back() is not const
     *
     * // Can reassign the wrapper itself
     * immut_data = fxt::immutable<std::vector<int>>({4, 5, 6});  // OK
     *
     * // Can move the wrapper efficiently
     * auto moved = std::move(immut_data);  // OK
     *
     * // Can extract value from rvalue
     * std::vector<int> extracted = std::move(moved).extract();  // OK
     * ```
     *
     * This implementation is based on Ivan Čukić's presentation at Meeting C++ 2023,
     * "Prog C++", with additional improvements for performance and usability.
     *
     * @tparam T The type of value to wrap
     *
     * @author Ivan Čukić (original design)
     * @see Meeting C++ 2023: "Prog C++"
     */
    template<typename T>
    class immutable
    {
        T m_value;

    public:
        /**
         * @brief Constructs an immutable wrapper with perfect forwarding
         *
         * Uses perfect forwarding to efficiently construct the wrapped value,
         * avoiding unnecessary copies when possible.
         *
         * @tparam U The type of the value to forward (deduced)
         * @param val The value to wrap
         */
        template<typename U = T>
            requires std::constructible_from<T, U>
        explicit immutable(U&& val)
            noexcept(std::is_nothrow_constructible_v<T, U>)
            : m_value(std::forward<U>(val)) {}

        /**
         * @brief Copy constructor
         * @param other The immutable instance to copy from
         */
        immutable(const immutable& other)
            noexcept(std::is_nothrow_copy_constructible_v<T>) = default;

        /**
         * @brief Move constructor
         * @param other The immutable instance to move from
         */
        immutable(immutable&& other)
            noexcept(std::is_nothrow_move_constructible_v<T>) = default;

        /**
         * @brief Copy assignment operator
         *
         * Assigns a new immutable value to this wrapper. Note that this replaces
         * the entire wrapped value - it does not modify the existing value, which
         * remains immutable throughout its lifetime.
         *
         * @param other The immutable instance to copy from
         * @return Reference to this instance
         */
        immutable& operator=(const immutable& other)
            noexcept(std::is_nothrow_copy_assignable_v<T>) = default;

        /**
         * @brief Move assignment operator
         *
         * Moves a new immutable value into this wrapper. This is efficient as it
         * transfers ownership rather than copying. The previous value is replaced,
         * but neither the old nor new value is ever modified - only moved.
         *
         * @param other The immutable instance to move from
         * @return Reference to this instance
         */
        immutable& operator=(immutable&& other)
            noexcept(std::is_nothrow_move_assignable_v<T>) = default;

        /**
         * @brief Implicit conversion to const reference (lvalue)
         *
         * Provides read-only access to the wrapped value through implicit conversion.
         *
         * @return Const reference to the wrapped value
         */
        [[nodiscard]] operator const T&() const& noexcept { return m_value; } // NOLINT

        /**
         * @brief Dereference operator (lvalue)
         *
         * Provides read-only access to the wrapped value through dereference.
         *
         * @return Const reference to the wrapped value
         */
        [[nodiscard]] const T& operator*() const& noexcept { return m_value; }

        /**
         * @brief Arrow operator for member access
         *
         * Allows calling const member functions on the wrapped value.
         *
         * @return Const pointer to the wrapped value
         */
        [[nodiscard]] const T* operator->() const& noexcept { return std::addressof(m_value); }

        /**
         * @brief Implicit conversion to rvalue reference (rvalue)
         *
         * Allows moving the value out of an rvalue immutable wrapper.
         *
         * @return Rvalue reference to the wrapped value
         */
        [[nodiscard]] operator T&&() && noexcept { return std::move(m_value); } // NOLINT

        /**
         * @brief Dereference operator (rvalue)
         *
         * Allows moving the value out through dereference of an rvalue wrapper.
         *
         * @return Rvalue reference to the wrapped value
         */
        [[nodiscard]] T&& operator*() && noexcept { return std::move(m_value); }

        /**
         * @brief Explicitly extract the value by moving (rvalue)
         *
         * Provides a named method for moving the value out of the wrapper,
         * making the intent explicit in code.
         *
         * @return The wrapped value (moved)
         */
        [[nodiscard]] T extract() && noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            return std::move(m_value);
        }

        /**
         * @brief Three-way comparison operator
         *
         * Enables spaceship operator comparisons if T supports them.
         *
         * @param other The immutable instance to compare with
         * @return Comparison result
         */
        [[nodiscard]] auto operator<=>(const immutable& other) const
            noexcept(noexcept(m_value <=> other.m_value))
            requires std::three_way_comparable<T> = default;

        /**
         * @brief Equality comparison operator
         *
         * Enables equality comparisons if T supports them.
         *
         * @param other The immutable instance to compare with
         * @return true if values are equal, false otherwise
         */
        [[nodiscard]] bool operator==(const immutable& other) const
            noexcept(noexcept(m_value == other.m_value))
            requires std::equality_comparable<T> = default;
    };

}