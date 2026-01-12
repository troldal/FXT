//
// Created by kenne on 02-01-2026.
//

/**
 * @file StringEnum.hpp
 * @brief String-based enum with compile-time type safety
 *
 * This file defines StringEnum, an enum-like type where each element is a distinct
 * compile-time string. Each string becomes a unique type through typed_string,
 * enabling type-safe visitation patterns and switch statements.
 */

#pragma once

#include "TypedString.hpp"
#include "EnumBase.hpp"
#include <fixed_string.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

namespace fxt
{

    /**
     * @brief Wrapper to mark a string as the default value for a StringEnum.
     * @tparam Str The string to use as default
     *
     * @details When used in a StringEnum template parameter list, this marks
     * the specified string as the default value to use when an invalid string
     * is assigned.
     *
     * @code
     * using MyEnum = StringEnum<Default("S1"), "S2", "S3">;
     * MyEnum e = "invalid"; // Will be set to "S1" instead of invalid_t
     * @endcode
     */
    template<fixstr::basic_fixed_string Str>
    struct Default
    {
        static constexpr auto value = Str;
    };

    /**
     * @brief Type representing an invalid state in StringEnum
     *
     * This type is passed to visitors when a StringEnum is in an invalid state
     * (e.g., assigned a string that doesn't match any of its template arguments).
     */
    struct invalid_t
    {
    };

    /**
     * @brief Helper to find the index of a fixed_string in a parameter pack
     *
     * Provides compile-time lookup of string position in a pack. Uses template
     * specialization to recursively search through the parameter pack.
     *
     * @tparam Str The string to search for
     * @tparam Strs The parameter pack of strings to search in
     */
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string... Strs>
    struct StringIndex;

    /// Specialization: String found at first position (base case)
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string... Strs>
    struct StringIndex<Str, Str, Strs...> : std::integral_constant<std::size_t, 0>
    {
    };

    /// Specialization: Recursive case - continue searching in remaining strings
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string First, fixstr::basic_fixed_string... Strs>
    struct StringIndex<Str, First, Strs...> : std::integral_constant<std::size_t, 1 + StringIndex<Str, Strs...>::value>
    {
    };

    /// Variable template for convenient access to StringIndex value
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string... Strs>
    inline constexpr std::size_t StringIndex_v = StringIndex<Str, Strs...>::value;

    /**
     * @brief Helper to check if a string exists in a parameter pack
     *
     * Uses template specialization to recursively search for a string match.
     *
     * @tparam Str The string to search for
     * @tparam Strs The parameter pack of strings to search in
     */
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string... Strs>
    struct StringInPack : std::false_type
    {
    };

    /// Specialization: Check if string matches first element or exists in rest
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string First, fixstr::basic_fixed_string... Rest>
    struct StringInPack<Str, First, Rest...> : std::conditional_t<Str == First, std::true_type, StringInPack<Str, Rest...>>
    {
    };

    /// Variable template for convenient access to StringInPack value
    template<fixstr::basic_fixed_string Str, fixstr::basic_fixed_string... Strs>
    inline constexpr bool StringInPack_v = StringInPack<Str, Strs...>::value;

    /**
     * @brief String-based enum with compile-time type safety
     *
     * StringEnum behaves like a traditional enum but uses compile-time strings as elements.
     * Each string becomes a distinct type through typed_string, enabling:
     * - Type-safe visitation with overloaded lambdas
     * - Switch statement support via index conversion
     * - Runtime validation with invalid state handling
     *
     * @tparam Strings Compile-time string literals that define the enum elements
     *
     * @details Example usage:
     * @code
     * using MyEnum = StringEnum<"Option1", "Option2", "Option3">;
     *
     * MyEnum e = "Option1";
     *
     * // Switch statement
     * switch (e) {
     *     case MyEnum::Index<"Option1">():
     *         // Handle Option1
     *         break;
     *     case MyEnum::Index<"Option2">():
     *         // Handle Option2
     *         break;
     * }
     *
     * // Type-safe visitation
     * e.visit(overloaded{
     *     [](MyEnum::Type<"Option1"> s) { std::cout << "Option1: " << s << '\n'; },
     *     [](MyEnum::Type<"Option2"> s) { std::cout << "Option2: " << s << '\n'; },
     *     [](MyEnum::Type<"Option3"> s) { std::cout << "Option3: " << s << '\n'; },
     *     [](invalid_t) { std::cout << "Invalid state\n"; }
     * });
     * @endcode
     */
    template<fixstr::basic_fixed_string... Strings>
    class string_enum : public detail::enum_base<string_enum<Strings...>, sizeof...(Strings)>
    {
        friend class detail::enum_base<string_enum, sizeof...(Strings)>;

    private:
        /// Special index value indicating an invalid state
        static constexpr std::size_t INVALID_INDEX = sizeof...(Strings);

        /// Current index into the Strings parameter pack (default to first string)
        std::size_t index_ = 0;

        /**
         * @brief Helper function to check if a runtime string matches any compile-time strings
         *
         * Searches through all template string arguments to find a match with the
         * provided runtime string.
         *
         * @param sv The string to search for
         * @return Optional containing the index if found, empty optional otherwise
         */
        static constexpr std::optional<std::size_t> find_index_impl(const std::string_view sv)
        {
            std::size_t result_idx  = 0;
            std::size_t current_idx = 0;    // NOLINT
            bool        found       = false;

            // Check each string in the pack using fold expression
            auto check_one = [&](auto str) {
                if (!found && sv == std::string_view(str)) {
                    result_idx = current_idx;
                    found      = true;
                }
                ++current_idx;
            };

            (check_one(Strings), ...);

            return found ? std::optional<std::size_t>(result_idx) : std::nullopt;
        }

    public:
        /**
         * @brief Type alias to get the typed_string type for a specific string
         *
         * Allows extracting the type corresponding to a compile-time string.
         *
         * @tparam Str The compile-time string to get the type for
         *
         * @details Example:
         * @code
         * using MyEnum = StringEnum<"A", "B">;
         * MyEnum::Type<"A"> myString;  // Type is typed_string<"A">
         * @endcode
         */
        template<fixstr::basic_fixed_string Str>
        using Type = fixstr::typed_string<Str>;

        /**
         * @brief Default constructor - initializes to the first string
         *
         * @details Requires at least one string in the template parameter pack
         */
        constexpr string_enum() { static_assert(sizeof...(Strings) > 0, "StringEnum must have at least one string"); }

        /**
         * @brief Constructor from string literal
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, it's set to an invalid state.
         *
         * @param str C-style string literal to initialize with
         */
        constexpr string_enum(const char* str) : index_(INVALID_INDEX)
        {    // NOLINT
            if (const auto idx = find_index_impl(std::string_view(str))) { index_ = *idx; }
        }

        /**
         * @brief Constructor from string_view
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, it's set to an invalid state.
         *
         * @param sv String view to initialize with
         */
        constexpr string_enum(const std::string_view sv) : index_(INVALID_INDEX)
        {    // NOLINT
            if (const auto idx = find_index_impl(sv)) { index_ = *idx; }
        }

        /**
         * @brief Assignment from string literal
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, it's set to an invalid state.
         *
         * @param str C-style string literal to assign
         * @return Reference to this object
         */
        constexpr string_enum& operator=(const char* str)
        {
            if (const auto idx = find_index_impl(std::string_view(str))) { index_ = *idx; }
            else {
                index_ = INVALID_INDEX;
            }
            return *this;
        }

        /**
         * @brief Assignment from string_view
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, it's set to an invalid state.
         *
         * @param sv String view to assign
         * @return Reference to this object
         */
        constexpr string_enum& operator=(const std::string_view sv)
        {
            if (const auto idx = find_index_impl(sv)) { index_ = *idx; }
            else {
                index_ = INVALID_INDEX;
            }
            return *this;
        }

        /**
         * @brief Get the current index
         *
         * @return The zero-based index of the current string, or INVALID_INDEX if invalid
         */
        [[nodiscard]] constexpr std::size_t index() const { return index_; }

        /**
         * @brief Static method to get the compile-time index of a specific string
         *
         * @tparam Str The compile-time string to get the index for
         * @return The zero-based index of the string in the template parameter pack
         *
         * @details Example:
         * @code
         * using MyEnum = StringEnum<"A", "B", "C">;
         * constexpr auto idx = MyEnum::Index<"B">();  // Returns 1
         * @endcode
         */
        template<fixstr::basic_fixed_string Str>
        static constexpr std::size_t IndexOf()
        {
            static_assert(StringInPack_v<Str, Strings...>, "String not in StringEnum");
            return StringIndex_v<Str, Strings...>;
        }

        /**
         * @brief Check if currently holding a specific string
         *
         * @tparam Str The compile-time string to check for
         * @return true if the enum is valid and currently holds the specified string
         *
         * @details Example:
         * @code
         * MyEnum e = "A";
         * if (e.is<"A">()) {
         *     // Handle case A
         * }
         * @endcode
         */
        template<fixstr::basic_fixed_string Str>
        [[nodiscard]] constexpr bool is() const
        { return this->is_valid() && index_ == IndexOf<Str>(); }

        /**
         * @brief Get the current string value as std::string
         *
         * @return The current string as std::string, or empty string if invalid
         */
        [[nodiscard]] std::string value() const
        {
            if (!this->is_valid()) { return {}; }

            std::string result;
            std::size_t idx = 0;
            static_cast<void>(((idx++ == index_ ? (result = std::string(Strings), true) : false) || ...));
            return result;
        }

        /**
         * @brief Mark as invalid explicitly
         *
         * Sets the enum to an invalid state. Subsequent calls to is_valid() will
         * return false, and visit() will invoke the visitor with invalid_t.
         */
        constexpr void invalidate() { index_ = INVALID_INDEX; }

        /**
         * @brief Visit with a callable that accepts specific typed_string types (const version)
         *
         * Invokes the visitor with the current string's typed_string type. If the enum
         * is in an invalid state, invokes the visitor with invalid_t instead.
         *
         * @tparam Visitor A callable type that can handle all possible typed_string types
         * @param visitor The visitor callable to invoke
         * @return The result of invoking the visitor
         *
         * @details Example:
         * @code
         * MyEnum e = "A";
         * e.visit(overloaded{
         *     [](MyEnum::Type<"A"> a) { std::cout << "Got A\n"; },
         *     [](MyEnum::Type<"B"> b) { std::cout << "Got B\n"; },
         *     [](invalid_t) { std::cout << "Invalid\n"; }
         * });
         * @endcode
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor) const
        {
            if (!this->is_valid()) { return std::forward<Visitor>(visitor)(invalid_t {}); }

            return this->visit_with_index(std::forward<Visitor>(visitor));
        }

        /**
         * @brief Visit with a callable that accepts specific typed_string types (non-const version)
         *
         * Invokes the visitor with the current string's typed_string type. If the enum
         * is in an invalid state, invokes the visitor with invalid_t instead.
         *
         * @tparam Visitor A callable type that can handle all possible typed_string types
         * @param visitor The visitor callable to invoke
         * @return The result of invoking the visitor
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor)
        {
            if (!this->is_valid()) { return std::forward<Visitor>(visitor)(invalid_t {}); }

            return this->visit_with_index(std::forward<Visitor>(visitor));
        }

    private:
        /**
         * @brief Called by base class during visitation (const version)
         *
         * This is part of the CRTP pattern. The base class calls this method with
         * a compile-time index to invoke the visitor with the correct typed_string.
         *
         * @tparam I The compile-time index of the string to visit
         * @tparam Visitor The visitor type
         * @param visitor The visitor to invoke
         * @return The result of invoking the visitor
         */
        template<std::size_t I, typename Visitor>
        constexpr decltype(auto) call_visitor_at(Visitor&& visitor) const
        {
            constexpr auto str = get_string_at_index<I>();
            return std::forward<Visitor>(visitor)(fixstr::typed_string<str> {});
        }

        /**
         * @brief Called by base class during visitation (non-const version)
         *
         * This is part of the CRTP pattern. The base class calls this method with
         * a compile-time index to invoke the visitor with the correct typed_string.
         *
         * @tparam I The compile-time index of the string to visit
         * @tparam Visitor The visitor type
         * @param visitor The visitor to invoke
         * @return The result of invoking the visitor
         */
        template<std::size_t I, typename Visitor>
        constexpr decltype(auto) call_visitor_at(Visitor&& visitor)
        {
            constexpr auto str = get_string_at_index<I>();
            return std::forward<Visitor>(visitor)(fixstr::typed_string<str> {});
        }

        /**
         * @brief Get the string at a specific compile-time index
         *
         * @tparam I The zero-based index
         * @return The fixed_string at the specified index
         */
        template<std::size_t I>
        static constexpr auto get_string_at_index()
        { return std::get<I>(std::tuple { Strings... }); }
    };

}    // namespace fxt
