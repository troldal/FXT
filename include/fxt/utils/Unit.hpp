//
// Created by kenne on 23-05-2025.
//

#pragma once

#include <variant>

namespace fxt
{
    /**
     * @brief Unit type representing an empty value
     *
     * Unit is an alias for std::monostate, which is the standard way to represent
     * an empty alternative in variant-like types. It's commonly used in functional
     * programming to represent functions that don't return meaningful values.
     *
     * Using std::monostate provides:
     * - Standard library integration and compatibility
     * - Well-defined comparison operators (all instances are equal)
     * - Hash support for use in unordered containers
     * - Compatibility with std::variant
     *
     * @example
     * @code
     * fxt::expected<fxt::Unit, std::string> perform_action() {
     *     // Do something...
     *     return fxt::Unit{};  // Success with no value
     * }
     * @endcode
     */
    using Unit = std::monostate;
}
