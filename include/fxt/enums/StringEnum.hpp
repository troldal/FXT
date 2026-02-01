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
#include <ostream>
#include <array>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>

namespace fxt
{
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
     * - Runtime validation with exceptions
     *
     * @tparam Strings Compile-time string literals that define the enum elements
     *
     * @details A string_enum always holds a valid value. Attempting to construct or assign
     * an invalid string throws std::invalid_argument. Example usage:
     * @code
     * using MyEnum = StringEnum<"Option1", "Option2", "Option3">;
     *
     * MyEnum e = "Option1";
     *
     * // Switch statement
     * switch (e.index()) {
     *     case MyEnum::IndexOf<"Option1">():
     *         // Handle Option1
     *         break;
     *     case MyEnum::IndexOf<"Option2">():
     *         // Handle Option2
     *         break;
     * }
     *
     * // Type-safe visitation
     * e.visit(overloaded{
     *     [](MyEnum::Type<"Option1"> s) { std::cout << "Option1: " << s << '\n'; },
     *     [](MyEnum::Type<"Option2"> s) { std::cout << "Option2: " << s << '\n'; },
     *     [](MyEnum::Type<"Option3"> s) { std::cout << "Option3: " << s << '\n'; }
     * });
     * @endcode
     */
    template<fixstr::basic_fixed_string... Strings>
    class string_enum : public detail::enum_base<string_enum<Strings...>, sizeof...(Strings)>
    {
        friend class detail::enum_base<string_enum, sizeof...(Strings)>;

    private:

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
        static constexpr std::optional<std::size_t> find_index_impl(const std::string_view sv) noexcept
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

        /**
         * @brief Validate and set index from string view
         *
         * Extracts common validation logic used by constructors and assignment operators.
         *
         * @param sv The string to validate and set
         * @throws std::invalid_argument if the string doesn't match any template argument
         */
        constexpr void validate_and_set(std::string_view sv)
        {
            if (const auto idx = find_index_impl(sv)) {
                index_ = *idx;
            } else {
                throw std::invalid_argument("Invalid string for string_enum: " + std::string(sv));
            }
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
         * string. Otherwise, throws std::invalid_argument.
         *
         * @param str C-style string literal to initialize with
         * @throws std::invalid_argument if the string doesn't match any template argument
         */
        explicit constexpr string_enum(const char* str)
        {
            validate_and_set(std::string_view(str));
        }

        /**
         * @brief Constructor from string_view
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, throws std::invalid_argument.
         *
         * @param sv String view to initialize with
         * @throws std::invalid_argument if the string doesn't match any template argument
         */
        explicit constexpr string_enum(const std::string_view sv)
        {
            validate_and_set(sv);
        }

        /**
         * @brief Assignment from string literal
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, throws std::invalid_argument.
         *
         * @param str C-style string literal to assign
         * @return Reference to this object
         * @throws std::invalid_argument if the string doesn't match any template argument
         */
        constexpr string_enum& operator=(const char* str)
        {
            validate_and_set(std::string_view(str));
            return *this;
        }

        /**
         * @brief Assignment from string_view
         *
         * If the string matches one of the template arguments, the enum is set to that
         * string. Otherwise, throws std::invalid_argument.
         *
         * @param sv String view to assign
         * @return Reference to this object
         * @throws std::invalid_argument if the string doesn't match any template argument
         */
        constexpr string_enum& operator=(const std::string_view sv)
        {
            validate_and_set(sv);
            return *this;
        }

        /**
         * @brief Get the current index
         *
         * @return The zero-based index of the current string
         */
        [[nodiscard]] constexpr std::size_t index() const noexcept { return index_; }

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
        static constexpr std::size_t IndexOf() noexcept
        {
            static_assert(StringInPack_v<Str, Strings...>, "String not in StringEnum");
            return StringIndex_v<Str, Strings...>;
        }

        /**
         * @brief Check if currently holding a specific string
         *
         * @tparam Str The compile-time string to check for
         * @return true if the enum currently holds the specified string
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
        [[nodiscard]] constexpr bool is() const noexcept
        { return index_ == IndexOf<Str>(); }

        /**
         * @brief Get the current string value as std::string_view
         *
         * @return The current string as std::string_view (points to compile-time string data)
         */
        [[nodiscard]] constexpr std::string_view value() const noexcept
        {
            constexpr auto values_array = std::array{std::string_view(Strings)...};
            return values_array[index_];
        }

        /**
         * @brief Get the number of enum values
         *
         * @return The number of strings in the enum
         */
        static constexpr std::size_t size() noexcept
        {
            return sizeof...(Strings);
        }

        /**
         * @brief Get an array of all enum values
         *
         * @return Array containing all enum strings as std::string_view
         *
         * @details Example:
         * @code
         * using MyEnum = StringEnum<"A", "B", "C">;
         * for (auto val : MyEnum::values()) {
         *     std::cout << val << '\n';
         * }
         * @endcode
         */
        static constexpr auto values() noexcept
        {
            return std::array<std::string_view, sizeof...(Strings)>{
                std::string_view(Strings)...
            };
        }

        /**
         * @brief Construct a string_enum from an index
         *
         * Creates a string_enum instance set to the string at the specified zero-based index.
         * This is useful for deserialization, when working with integer representations,
         * or when iterating through enum values programmatically.
         *
         * @param idx The zero-based index of the string to select (must be < size())
         * @return A string_enum set to the string at the given index
         * @throws std::out_of_range if idx >= size()
         *
         * @details Example:
         * @code
         * using Status = StringEnum<"Pending", "Active", "Completed">;
         *
         * // Construct from index
         * auto s = Status::from_index(1);  // s holds "Active"
         *
         * // Useful for deserialization
         * int stored_value = 2;
         * auto status = Status::from_index(stored_value);
         *
         * // Iterate through all values
         * for (std::size_t i = 0; i < Status::size(); ++i) {
         *     auto s = Status::from_index(i);
         *     std::cout << i << ": " << s << '\n';
         * }
         * @endcode
         */
        static constexpr string_enum from_index(std::size_t idx)
        {
            if (idx >= sizeof...(Strings)) {
                throw std::out_of_range("Index " + std::to_string(idx) +
                                       " out of range for string_enum (size: " +
                                       std::to_string(sizeof...(Strings)) + ")");
            }
            string_enum result;
            result.index_ = idx;
            return result;
        }

        /**
         * @brief Visit with a callable that accepts specific typed_string types (const version)
         *
         * Invokes the visitor with the current string's typed_string type.
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
         *     [](MyEnum::Type<"B"> b) { std::cout << "Got B\n"; }
         * });
         * @endcode
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor) const
        {
            return this->visit_with_index(std::forward<Visitor>(visitor));
        }

        /**
         * @brief Visit with a callable that accepts specific typed_string types (non-const version)
         *
         * Invokes the visitor with the current string's typed_string type.
         *
         * @tparam Visitor A callable type that can handle all possible typed_string types
         * @param visitor The visitor callable to invoke
         * @return The result of invoking the visitor
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor)
        {

            return this->visit_with_index(std::forward<Visitor>(visitor));
        }

        /**
         * @brief Equality comparison with another string_enum
         *
         * @param other The other string_enum to compare with
         * @return true if both enums hold the same string
         */
        constexpr bool operator==(const string_enum& other) const noexcept = default;

        /**
         * @brief Three-way comparison with another string_enum
         *
         * @param other The other string_enum to compare with
         * @return Ordering result based on index comparison
         */
        constexpr auto operator<=>(const string_enum& other) const noexcept = default;

        /**
         * @brief Equality comparison with a string_view
         *
         * @param sv The string_view to compare with
         * @return true if the enum's current value matches the string_view
         */
        constexpr bool operator==(std::string_view sv) const noexcept
        {
            return value() == sv;
        }

        /**
         * @brief Three-way comparison with a string_view
         *
         * @param sv The string_view to compare with
         * @return Ordering result based on index comparison
         */
        constexpr auto operator<=>(std::string_view sv) const noexcept
        {
            return value() <=> sv;
        }

        /**
         * @brief Convert to string for generic code (ADL-friendly)
         *
         * @param e The string_enum to convert
         * @return String representation of the current value
         */
        [[nodiscard]] friend std::string to_string(const string_enum& e)
        {
            return std::string(e.value());
        }

        /**
         * @brief Stream output operator
         *
         * Enables direct output of string_enum to streams like std::cout.
         *
         * @param os The output stream
         * @param e The string_enum to output
         * @return Reference to the stream for chaining
         *
         * @details Example:
         * @code
         * MyEnum e{"Option1"};
         * std::cout << "Current value: " << e << '\n';
         * @endcode
         */
        friend constexpr std::ostream& operator<<(std::ostream& os, const string_enum& e)
        {
            return os << e.value();
        }

        /**
         * @brief Compute hash value for string_enum (ADL-friendly)
         *
         * Provides a hash function that can be found via argument-dependent lookup.
         * The hash is computed based on the string value, not the index, ensuring
         * consistent hashing across different string_enum instantiations with the
         * same string values.
         *
         * @param e The string_enum to hash
         * @return Hash value of the current string
         *
         * @details This function enables ADL-based hashing. For standard library
         * containers like std::unordered_map, use the std::hash specialization
         * defined after the class.
         *
         * @code
         * using MyEnum = StringEnum<"A", "B", "C">;
         * MyEnum e{"B"};
         * std::size_t h = hash_value(e);  // ADL finds this function
         * @endcode
         */
        [[nodiscard]] friend constexpr std::size_t hash_value(const string_enum& e) noexcept
        {
            return std::hash<std::string_view>{}(e.value());
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

// std::format support for C++20 and later
#if __cpp_lib_format >= 201907L
#include <format>

template<fixstr::basic_fixed_string... Strings>
struct std::formatter<fxt::string_enum<Strings...>> : std::formatter<std::string_view>
{
    /**
     * @brief Format a string_enum using std::format
     *
     * Delegates to std::formatter<std::string_view> to handle format specifications.
     *
     * @param e The string_enum to format
     * @param ctx The format context
     * @return Iterator to the end of the formatted output
     *
     * @details Supports all std::string_view format specifications:
     * @code
     * using MyEnum = fxt::string_enum<"Short", "VeryLongValue">;
     * MyEnum e{"Short"};
     *
     * std::format("{}", e);           // "Short"
     * std::format("{:>15}", e);       // "          Short" (right-aligned)
     * std::format("{:<15}", e);       // "Short          " (left-aligned)
     * std::format("{:^15}", e);       // "     Short     " (centered)
     * std::format("{:.3}", e);        // "Sho" (truncated)
     * @endcode
     */
    auto format(const fxt::string_enum<Strings...>& e, std::format_context& ctx) const
    {
        return std::formatter<std::string_view>::format(e.value(), ctx);
    }
};

#endif // __cpp_lib_format

/**
 * @brief Standard library hash specialization for string_enum
 *
 * Enables use of string_enum in standard library unordered containers such as
 * std::unordered_map, std::unordered_set, and std::unordered_multimap.
 *
 * The hash is computed based on the string value (not the index), ensuring
 * consistent and collision-resistant hashing. Two string_enum instances with
 * the same string value will have the same hash, even if they come from
 * different enum types.
 *
 * @tparam Strings The compile-time strings defining the enum
 *
 * @details Example usage:
 * @code
 * using HttpMethod = fxt::string_enum<"GET", "POST", "PUT", "DELETE">;
 * using StatusCode = fxt::string_enum<"200", "404", "500">;
 *
 * // Use in unordered_map
 * std::unordered_map<HttpMethod, std::function<void()>> handlers;
 * handlers[HttpMethod{"GET"}] = []{ std::cout << "GET handler\n"; };
 * handlers[HttpMethod{"POST"}] = []{ std::cout << "POST handler\n"; };
 *
 * // Use in unordered_set
 * std::unordered_set<StatusCode> error_codes;
 * error_codes.insert(StatusCode{"404"});
 * error_codes.insert(StatusCode{"500"});
 *
 * // Check containment
 * if (error_codes.contains(StatusCode{"404"})) {
 *     std::cout << "404 is an error code\n";
 * }
 * @endcode
 *
 * @note This specialization is explicitly allowed by the C++ standard for
 * user-defined types (C++11 §17.6.3.3).
 */
template<fixstr::basic_fixed_string... Strings>
struct std::hash<fxt::string_enum<Strings...>>
{
    /**
     * @brief Compute hash value for a string_enum instance
     *
     * @param e The string_enum instance to hash
     * @return Hash value computed from the current string value
     */
    constexpr std::size_t operator()(const fxt::string_enum<Strings...>& e) const noexcept
    {
        return std::hash<std::string_view>{}(e.value());
    }
};
