//
// Created by kenne on 03-01-2026.
//

/**
 * @file EnumBase.hpp
 * @brief CRTP base class for type-safe enum-like types with index-based visitation
 *
 * This file defines the EnumBase template class that provides common functionality
 * for enum-like types such as TypeEnum and StringEnum. It uses the Curiously
 * Recurring Template Pattern (CRTP) to achieve compile-time polymorphism without
 * virtual function overhead.
 */

#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace fxt::detail
{

    /**
     * @brief CRTP base class for enum-like types
     *
     * Provides common functionality for index-based operations and visitation patterns.
     * This class uses the Curiously Recurring Template Pattern (CRTP) to enable static
     * polymorphism without runtime overhead.
     *
     * @tparam Derived The derived class type (CRTP pattern)
     * @tparam NumElements The number of elements in the enum (compile-time constant)
     *
     * @details The derived class must provide:
     * - `std::size_t index() const` - Returns the current index
     * - `decltype(auto) call_visitor_at<I>(Visitor&&)` - Calls visitor with element at index I
     *
     * @note This class should not be used directly; it's intended as a base class for
     * TypeEnum and StringEnum.
     */
    template<typename Derived, std::size_t NumElements>
    class enum_base
    {
    protected:
        /// Special index value indicating an invalid state (one past the last valid index)
        static constexpr std::size_t INVALID_INDEX = NumElements;

        /**
         * @brief Get the current index from the derived class (const version)
         * @return The current index
         * @note This method uses CRTP to call the derived class's index() method
         */
        [[nodiscard]] constexpr std::size_t current_index() const { return static_cast<const Derived*>(this)->index(); }

        /**
         * @brief Get the current index from the derived class (non-const version)
         * @return The current index
         * @note This method uses CRTP to call the derived class's index() method
         */
        [[nodiscard]] constexpr std::size_t current_index() { return static_cast<Derived*>(this)->index(); }

        /**
         * @brief Check if the current index is valid
         * @return true if the index is less than NumElements, false otherwise
         */
        [[nodiscard]] constexpr bool is_valid_index() const { return current_index() < NumElements; }

    public:
        /**
         * @brief Get the total number of elements in the enum
         * @return The number of elements (compile-time constant)
         */
        static constexpr std::size_t size() { return NumElements; }

        /**
         * @brief Check if the enum is in a valid state
         * @return true if the current index is valid, false otherwise
         */
        [[nodiscard]] constexpr bool is_valid() const { return is_valid_index(); }

        /**
         * @brief Conversion operator to std::size_t for use in switch statements
         * @return The current index
         *
         * @details This allows enum objects to be used directly in switch statements:
         * @code
         * MyEnum e;
         * switch (e) {
         *     case MyEnum::Index<SomeType>():
         *         // handle SomeType
         *         break;
         * }
         * @endcode
         */
        // TODO: SAFETY — an IMPLICIT conversion to std::size_t lets unrelated enum types
        //       compare and mix arithmetically (`string_enum_a == type_enum_b`, `e + 1`,
        //       accidental use as an array index) without any diagnostic, defeating the
        //       type safety these classes exist to provide. Make the conversion explicit
        //       and let switch statements use e.index() — or keep it implicit but document
        //       the trade-off prominently.
        constexpr operator std::size_t() const
        {    // NOLINT
            return current_index();
        }

        /**
         * @brief Visit the current element with a callable (const version)
         *
         * Dispatches to the derived class's call_visitor_at<I>() method based on the
         * current index. This provides type-safe visitation at compile time.
         *
         * @tparam Visitor A callable type that can accept the element type
         * @param visitor The visitor callable to invoke with the current element
         * @return The result of calling the visitor with the current element
         *
         * @note The derived class must implement:
         * @code
         * template<std::size_t I, typename Visitor>
         * decltype(auto) call_visitor_at(Visitor&& visitor) const;
         * @endcode
         */
        template<typename Visitor>
        constexpr decltype(auto) visit_with_index(Visitor&& visitor) const
        { return visit_index_impl(std::forward<Visitor>(visitor), current_index(), std::make_index_sequence<NumElements> {}); }

        /**
         * @brief Visit the current element with a callable (non-const version)
         *
         * Dispatches to the derived class's call_visitor_at<I>() method based on the
         * current index. This provides type-safe visitation at compile time.
         *
         * @tparam Visitor A callable type that can accept the element type
         * @param visitor The visitor callable to invoke with the current element
         * @return The result of calling the visitor with the current element
         *
         * @note The derived class must implement:
         * @code
         * template<std::size_t I, typename Visitor>
         * decltype(auto) call_visitor_at(Visitor&& visitor);
         * @endcode
         */
        template<typename Visitor>
        constexpr decltype(auto) visit_with_index(Visitor&& visitor)
        { return visit_index_impl(std::forward<Visitor>(visitor), current_index(), std::make_index_sequence<NumElements> {}); }

    private:
        /**
         * @brief Implementation of index-based visitation (const version)
         *
         * Creates an index sequence and delegates to dispatch_at for compile-time dispatch.
         *
         * @tparam Visitor The visitor type
         * @tparam Is Index sequence for all possible indices
         * @param visitor The visitor callable
         * @param idx The current runtime index
         * @return The result of the visitation
         */
        template<typename Visitor, std::size_t... Is>
        constexpr decltype(auto) visit_index_impl(Visitor&& visitor, const std::size_t idx, std::index_sequence<Is...>) const
        { return dispatch_at<Visitor, Is...>(std::forward<Visitor>(visitor), idx); }

        /**
         * @brief Implementation of index-based visitation (non-const version)
         *
         * Creates an index sequence and delegates to dispatch_at for compile-time dispatch.
         *
         * @tparam Visitor The visitor type
         * @tparam Is Index sequence for all possible indices
         * @param visitor The visitor callable
         * @param idx The current runtime index
         * @return The result of the visitation
         */
        template<typename Visitor, std::size_t... Is>
        constexpr decltype(auto) visit_index_impl(Visitor&& visitor, const std::size_t idx, std::index_sequence<Is...>)
        { return dispatch_at<Visitor, Is...>(std::forward<Visitor>(visitor), idx); }

        /**
         * @brief Dispatch to the correct index handler (const version)
         *
         * Recursively checks each index I against the runtime index and calls the
         * derived class's call_visitor_at<I>() when a match is found.
         *
         * @tparam Visitor The visitor type
         * @tparam I Current index being checked
         * @tparam Rest Remaining indices to check
         * @param visitor The visitor callable
         * @param idx The runtime index to match
         * @return The result of calling the visitor with the element at the matched index
         * @throws std::runtime_error if no index matches (should be unreachable if is_valid() is checked)
         */
        template<typename Visitor, std::size_t I, std::size_t... Rest>
        constexpr decltype(auto) dispatch_at(Visitor&& visitor, std::size_t idx) const
        {
            if (idx == I) { return static_cast<const Derived*>(this)->template call_visitor_at<I>(std::forward<Visitor>(visitor)); }

            if constexpr (sizeof...(Rest) > 0) { return dispatch_at<Visitor, Rest...>(std::forward<Visitor>(visitor), idx); }
            else {
                // Should be unreachable if is_valid() is properly checked
                throw std::runtime_error("Invalid index in visit");
            }
        }

        /**
         * @brief Dispatch to the correct index handler (non-const version)
         *
         * Recursively checks each index I against the runtime index and calls the
         * derived class's call_visitor_at<I>() when a match is found.
         *
         * @tparam Visitor The visitor type
         * @tparam I Current index being checked
         * @tparam Rest Remaining indices to check
         * @param visitor The visitor callable
         * @param idx The runtime index to match
         * @return The result of calling the visitor with the element at the matched index
         * @throws std::runtime_error if no index matches (should be unreachable if is_valid() is checked)
         */
        template<typename Visitor, std::size_t I, std::size_t... Rest>
        constexpr decltype(auto) dispatch_at(Visitor&& visitor, const std::size_t idx)
        {
            if (idx == I) { return static_cast<Derived*>(this)->template call_visitor_at<I>(std::forward<Visitor>(visitor)); }

            if constexpr (sizeof...(Rest) > 0) { return dispatch_at<Visitor, Rest...>(std::forward<Visitor>(visitor), idx); }
            else {
                // Should be unreachable if is_valid() is properly checked
                throw std::runtime_error("Invalid index in visit");
            }
        }
    };

}    // namespace fxt::detail
