//
// Created by kenne on 02-01-2026.
//

/**
 * @file TypedString.hpp
 * @brief Compile-time string with content-based type identity
 *
 * This file defines typed_string, a wrapper around fixed_string that makes each
 * unique string content a distinct type. Unlike fixed_string which only distinguishes
 * types by character type and length, typed_string creates unique types based on
 * the actual string content.
 */

#pragma once

#include <fixed_string.hpp>
#include <string_view>
#include <ostream>
#include <type_traits>

/**
 * @namespace fixstr
 * @brief Namespace for fixed string utilities
 */
namespace fixstr {

/**
 * @brief Compile-time string with content-based type identity
 *
 * typed_string wraps a compile-time string and makes each unique string content
 * a distinct type. This enables type-safe string handling at compile time and
 * allows string literals to be used as template parameters while maintaining
 * type uniqueness based on content.
 *
 * @tparam Str The compile-time string value
 *
 * @details Key differences from fixed_string:
 * - fixed_string: "four" and "five" are the same type (both char[4])
 * - typed_string: "four" and "five" are different types
 *
 * This enables:
 * - Function overloading based on string content
 * - Type-safe string-based dispatch
 * - Compile-time string validation
 *
 * Example usage:
 * @code
 * using FourType = typed_string<"four">;
 * using FiveType = typed_string<"five">;
 *
 * static_assert(!std::is_same_v<FourType, FiveType>);
 *
 * FourType str;
 * std::cout << str;  // Prints "four"
 *
 * if (str == "four") {
 *     // Compile-time and runtime comparison
 * }
 * @endcode
 */
template<basic_fixed_string Str>
class typed_string {
public:
    /// Character type of the string
    using char_type = typename decltype(Str)::value_type;

    /// Character traits type for the string
    using traits_type = typename decltype(Str)::traits_type;

    /// String view type for this character type
    using string_view_type = std::basic_string_view<char_type, traits_type>;

    /// Compile-time constant for the string size
    static constexpr auto size_value = Str.size();

    /**
     * @brief Default constructor
     *
     * Constructs a typed_string. Since the string value is compile-time,
     * no runtime initialization is needed.
     */
    constexpr typed_string() noexcept = default;

    /**
     * @brief Copy constructor
     *
     * @param _ The typed_string to copy from
     */
    constexpr typed_string(const typed_string& _) noexcept = default;

    /**
     * @brief Copy assignment operator
     *
     * @param _ The typed_string to copy from
     * @return Reference to this object
     */
    constexpr typed_string& operator=(const typed_string& _) noexcept = default;

    /**
     * @brief Get the compile-time string value
     *
     * @return The fixed_string value
     */
    [[nodiscard]] static constexpr auto value() noexcept {
        return Str;
    }

    /**
     * @brief Get the size of the string
     *
     * @return The number of characters in the string (excluding null terminator)
     */
    [[nodiscard]] static constexpr std::size_t size() noexcept {
        return Str.size();
    }

    /**
     * @brief Convert to string_view
     *
     * Creates a string_view that views the compile-time string data.
     *
     * @return A string_view of the string contents
     */
    [[nodiscard]] static constexpr string_view_type view() noexcept {
        return string_view_type(Str.data(), Str.size());
    }

    /**
     * @brief Implicit conversion to string_view
     *
     * Allows typed_string to be used wherever string_view is expected.
     *
     * @return A string_view of the string contents
     */
    [[nodiscard]] constexpr operator string_view_type() const noexcept {
        return view();
    }

    /**
     * @brief Get raw data pointer
     *
     * Returns a pointer to the underlying character array.
     *
     * @return Pointer to the first character of the string
     */
    [[nodiscard]] static constexpr const char_type* data() noexcept {
        return Str.data();
    }

    /**
     * @brief Get C-style string
     *
     * Returns a null-terminated C-string pointer.
     *
     * @return Null-terminated C-string pointer
     */
    [[nodiscard]] static constexpr const char_type* c_str() noexcept {
        return Str.data();
    }

    /**
     * @brief Check if string is empty
     *
     * @return true if the string has zero length, false otherwise
     */
    [[nodiscard]] static constexpr bool empty() noexcept {
        return Str.size() == 0;
    }

    /**
     * @brief Compile-time equality comparison with another string
     *
     * Compares this typed_string with another fixed_string at compile time.
     *
     * @tparam Other The fixed_string to compare against
     * @return true if the strings are equal, false otherwise
     *
     * @details Example:
     * @code
     * using MyString = typed_string<"hello">;
     * static_assert(MyString::equals<"hello">());
     * static_assert(!MyString::equals<"world">());
     * @endcode
     */
    template<basic_fixed_string Other>
    [[nodiscard]] static constexpr bool equals() noexcept {
        return Str == Other;
    }

    /**
     * @brief Stream output operator
     *
     * Outputs the string to an output stream.
     *
     * @param os The output stream
     * @param str The typed_string to output (parameter unused as value is static)
     * @return Reference to the output stream
     */
    friend std::basic_ostream<char_type, traits_type>&
    operator<<(std::basic_ostream<char_type, traits_type>& os, typed_string) {
        return os << view();
    }
};

/**
 * @brief Equality comparison between two typed_strings
 *
 * Compares two typed_strings for equality at compile time.
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if strings are equal, false otherwise
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator==(typed_string<Str1>, typed_string<Str2>) noexcept {
    return Str1 == Str2;
}

/**
 * @brief Inequality comparison between two typed_strings
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if strings are not equal, false otherwise
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator!=(typed_string<Str1>, typed_string<Str2>) noexcept {
    return Str1 != Str2;
}

/**
 * @brief Less-than comparison between two typed_strings
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if first string is lexicographically less than second
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator<(typed_string<Str1>, typed_string<Str2>) noexcept {
    return typed_string<Str1>::view() < typed_string<Str2>::view();
}

/**
 * @brief Less-than-or-equal comparison between two typed_strings
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if first string is lexicographically less than or equal to second
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator<=(typed_string<Str1>, typed_string<Str2>) noexcept {
    return typed_string<Str1>::view() <= typed_string<Str2>::view();
}

/**
 * @brief Greater-than comparison between two typed_strings
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if first string is lexicographically greater than second
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator>(typed_string<Str1>, typed_string<Str2>) noexcept {
    return typed_string<Str1>::view() > typed_string<Str2>::view();
}

/**
 * @brief Greater-than-or-equal comparison between two typed_strings
 *
 * @tparam Str1 First string value
 * @tparam Str2 Second string value
 * @return true if first string is lexicographically greater than or equal to second
 */
template<basic_fixed_string Str1, basic_fixed_string Str2>
[[nodiscard]] constexpr bool operator>=(typed_string<Str1>, typed_string<Str2>) noexcept {
    return typed_string<Str1>::view() >= typed_string<Str2>::view();
}

/**
 * @brief Equality comparison between typed_string and string_view
 *
 * Compares a typed_string with a string_view at runtime.
 *
 * @tparam Str The compile-time string value
 * @param sv The string_view to compare against
 * @return true if the strings are equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator==(typed_string<Str>, typename typed_string<Str>::string_view_type sv) noexcept {
    return typed_string<Str>::view() == sv;
}

/**
 * @brief Equality comparison between string_view and typed_string
 *
 * Compares a string_view with a typed_string at runtime.
 *
 * @tparam Str The compile-time string value
 * @param sv The string_view to compare
 * @return true if the strings are equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator==(typename typed_string<Str>::string_view_type sv, typed_string<Str>) noexcept {
    return sv == typed_string<Str>::view();
}

/**
 * @brief Inequality comparison between typed_string and string_view
 *
 * @tparam Str The compile-time string value
 * @param sv The string_view to compare against
 * @return true if the strings are not equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator!=(typed_string<Str>, typename typed_string<Str>::string_view_type sv) noexcept {
    return typed_string<Str>::view() != sv;
}

/**
 * @brief Inequality comparison between string_view and typed_string
 *
 * @tparam Str The compile-time string value
 * @param sv The string_view to compare
 * @return true if the strings are not equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator!=(typename typed_string<Str>::string_view_type sv, typed_string<Str>) noexcept {
    return sv != typed_string<Str>::view();
}

/**
 * @brief Equality comparison between typed_string and C-string
 *
 * Compares a typed_string with a null-terminated C-string at runtime.
 *
 * @tparam Str The compile-time string value
 * @param str The C-string to compare against
 * @return true if the strings are equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator==(typed_string<Str>, const typename typed_string<Str>::char_type* str) noexcept {
    return typed_string<Str>::view() == std::basic_string_view(str);
}

/**
 * @brief Equality comparison between C-string and typed_string
 *
 * Compares a null-terminated C-string with a typed_string at runtime.
 *
 * @tparam Str The compile-time string value
 * @param str The C-string to compare
 * @return true if the strings are equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator==(const typename typed_string<Str>::char_type* str, typed_string<Str>) noexcept {
    return std::basic_string_view(str) == typed_string<Str>::view();
}

/**
 * @brief Inequality comparison between typed_string and C-string
 *
 * @tparam Str The compile-time string value
 * @param str The C-string to compare against
 * @return true if the strings are not equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator!=(typed_string<Str>, const typename typed_string<Str>::char_type* str) noexcept {
    return typed_string<Str>::view() != std::basic_string_view(str);
}

/**
 * @brief Inequality comparison between C-string and typed_string
 *
 * @tparam Str The compile-time string value
 * @param str The C-string to compare
 * @return true if the strings are not equal, false otherwise
 */
template<basic_fixed_string Str>
[[nodiscard]] constexpr bool operator!=(const typename typed_string<Str>::char_type* str, typed_string<Str>) noexcept {
    return std::basic_string_view(str) != typed_string<Str>::view();
}

} // namespace fixstr

