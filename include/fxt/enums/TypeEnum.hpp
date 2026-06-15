//
// Created by kenne on 02-01-2026.
//

/**
 * @file TypeEnum.hpp
 * @brief Type-based enum with compile-time type safety
 *
 * This file defines TypeEnum, an enum-like type where each element is a distinct
 * C++ type. It uses std::variant internally but provides a cleaner interface for
 * type-safe operations including switch statements and visitation patterns.
 */

#pragma once

#include "EnumBase.hpp"
#include <cstddef>
#include <type_traits>
#include <variant>

namespace fxt
{

    /**
     * @brief Helper to find the index of a type in a parameter pack
     *
     * Provides compile-time lookup of type position in a pack. Uses template
     * specialization to recursively search through the parameter pack.
     *
     * @tparam T The type to search for
     * @tparam Ts The parameter pack of types to search in
     */
    template<typename T, typename... Ts>
    struct TypeIndex;

    /// Specialization: Type found at first position (base case)
    template<typename T, typename... Ts>
    struct TypeIndex<T, T, Ts...> : std::integral_constant<std::size_t, 0>
    {
    };

    /// Specialization: Recursive case - continue searching in remaining types
    template<typename T, typename U, typename... Ts>
    struct TypeIndex<T, U, Ts...> : std::integral_constant<std::size_t, 1 + TypeIndex<T, Ts...>::value>
    {
    };

    /// Variable template for convenient access to TypeIndex value
    template<typename T, typename... Ts>
    inline constexpr std::size_t TypeIndex_v = TypeIndex<T, Ts...>::value;

    /**
     * @brief Type-based enum with compile-time type safety
     *
     * TypeEnum behaves like a traditional enum but uses C++ types as elements.
     * It wraps std::variant but provides a more intuitive interface with:
     * - Type-safe visitation with std::visit or overloaded lambdas
     * - Switch statement support via index conversion
     * - Type checking and retrieval methods
     *
     * @tparam Types The C++ types that define the enum elements
     *
     * @details Example usage:
     * @code
     * struct TypeA { int value; };
     * struct TypeB { std::string name; };
     *
     * using MyEnum = TypeEnum<TypeA, TypeB>;
     *
     * MyEnum e = TypeA{42};
     *
     * // Switch statement
     * switch (e.index()) {
     *     case MyEnum::IndexOf<TypeA>():
     *         // Handle TypeA
     *         break;
     *     case MyEnum::IndexOf<TypeB>():
     *         // Handle TypeB
     *         break;
     * }
     *
     * // Type-safe visitation
     * e.visit([](auto& value) {
     *     if constexpr (std::is_same_v<decltype(value), TypeA&>) {
     *         std::cout << "TypeA: " << value.value << '\n';
     *     } else {
     *         std::cout << "TypeB: " << value.name << '\n';
     *     }
     * });
     *
     * // Type checking
     * if (e.is<TypeA>()) {
     *     auto& a = e.get<TypeA>();
     *     // Use TypeA
     * }
     * @endcode
     */
    template<typename... Types>
    class type_enum : public detail::enum_base<type_enum<Types...>, sizeof...(Types)>
    {
        friend class detail::enum_base<type_enum, sizeof...(Types)>;

    private:
        /// Internal variant storage for the enum value
        std::variant<Types...> variant_;

    public:
        /**
         * @brief Default constructor - initializes to the first type
         *
         * Creates a TypeEnum with the first type in the parameter pack default-constructed.
         */
        type_enum() : variant_(std::in_place_index<0>) {}

        /**
         * @brief Constructor from any of the enum types
         *
         * Constructs the TypeEnum with a value of one of the template parameter types.
         *
         * @tparam T The type of the value (must be one of Types...)
         * @param value The value to initialize with
         */
        template<typename T, typename = std::enable_if_t<(std::is_same_v<T, Types> || ...)>>
        type_enum(const T& value) : variant_(value)
        {}    // NOLINT

        /**
         * @brief Assignment from any of the enum types
         *
         * Assigns a new value to the TypeEnum, changing its active type.
         *
         * @tparam T The type of the value (must be one of Types...)
         * @param value The value to assign
         * @return Reference to this object
         */
        template<typename T, typename = std::enable_if_t<(std::is_same_v<T, Types> || ...)>>
        type_enum& operator=(const T& value)
        {
            variant_ = value;
            return *this;
        }

        /**
         * @brief Get the current index
         *
         * Returns the zero-based index of the currently active type.
         *
         * @return The index of the active type in the parameter pack
         */
        [[nodiscard]] std::size_t index() const { return variant_.index(); }

        /**
         * @brief Static method to get the compile-time index of a specific type
         *
         * Returns the zero-based position of a type in the template parameter pack.
         *
         * @tparam T The type to get the index for
         * @return The compile-time index of the type
         *
         * @details Example:
         * @code
         * using MyEnum = TypeEnum<TypeA, TypeB, TypeC>;
         * constexpr auto idx = MyEnum::IndexOf<TypeB>();  // Returns 1
         * @endcode
         */
        template<typename T>
        static constexpr std::size_t IndexOf()
        {
            static_assert((std::is_same_v<T, Types> || ...), "Type not in TypeEnum");
            return TypeIndex_v<T, Types...>;
        }

        /**
         * @brief Check if currently holding a specific type
         *
         * Tests whether the active type matches the template parameter type.
         *
         * @tparam T The type to check for
         * @return true if the enum currently holds type T, false otherwise
         *
         * @details Example:
         * @code
         * MyEnum e = TypeA{};
         * if (e.is<TypeA>()) {
         *     // Handle TypeA
         * }
         * @endcode
         */
        template<typename T>
        [[nodiscard]] bool is() const
        { return variant_.index() == IndexOf<T>(); }

        /**
         * @brief Get the value as a specific type (non-const version)
         *
         * Returns a reference to the stored value, cast to the specified type.
         *
         * @tparam T The type to retrieve
         * @return Reference to the stored value of type T
         * @throws std::bad_variant_access if the active type is not T
         *
         * @details Example:
         * @code
         * MyEnum e = TypeA{42};
         * TypeA& a = e.get<TypeA>();  // OK
         * TypeB& b = e.get<TypeB>();  // Throws std::bad_variant_access
         * @endcode
         */
        template<typename T>
        T& get()
        { return std::get<T>(variant_); }

        /**
         * @brief Get the value as a specific type (const version)
         *
         * Returns a const reference to the stored value, cast to the specified type.
         *
         * @tparam T The type to retrieve
         * @return Const reference to the stored value of type T
         * @throws std::bad_variant_access if the active type is not T
         */
        template<typename T>
        const T& get() const
        { return std::get<T>(variant_); }

        /**
         * @brief Get pointer to value if it's the specified type (non-const version)
         *
         * Returns a pointer to the stored value if the active type matches T,
         * otherwise returns nullptr. This is a safe alternative to get() that
         * doesn't throw exceptions.
         *
         * @tparam T The type to retrieve
         * @return Pointer to the value if active type is T, nullptr otherwise
         *
         * @details Example:
         * @code
         * MyEnum e = TypeA{42};
         * if (auto* a = e.get_if<TypeA>()) {
         *     // Use *a
         * }
         * @endcode
         */
        template<typename T>
        T* get_if()
        { return std::get_if<T>(&variant_); }

        /**
         * @brief Get pointer to value if it's the specified type (const version)
         *
         * Returns a const pointer to the stored value if the active type matches T,
         * otherwise returns nullptr.
         *
         * @tparam T The type to retrieve
         * @return Const pointer to the value if active type is T, nullptr otherwise
         */
        template<typename T>
        const T* get_if() const
        { return std::get_if<T>(&variant_); }

        /**
         * @brief Access the underlying variant (non-const version)
         *
         * Provides direct access to the internal std::variant storage.
         *
         * @return Reference to the internal std::variant
         */
        std::variant<Types...>& variant() { return variant_; }

        /**
         * @brief Access the underlying variant (const version)
         *
         * Provides direct access to the internal std::variant storage.
         *
         * @return Const reference to the internal std::variant
         */
        const std::variant<Types...>& variant() const { return variant_; }

        /**
         * @brief Visit with a callable that accepts the types in the variant (const version)
         *
         * Invokes the visitor with the currently active type using std::visit.
         * The visitor should be able to handle all types in the TypeEnum.
         *
         * @tparam Visitor A callable type that can handle all Types...
         * @param visitor The visitor callable to invoke
         * @return The result of invoking the visitor with the active type
         *
         * @details Example:
         * @code
         * MyEnum e = TypeA{42};
         * e.visit([](auto& value) {
         *     std::cout << "Visited with type\n";
         * });
         *
         * // Or with overloaded lambdas
         * e.visit(overloaded{
         *     [](TypeA& a) { std::cout << "TypeA\n"; },
         *     [](TypeB& b) { std::cout << "TypeB\n"; }
         * });
         * @endcode
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor) const
        { return std::visit(std::forward<Visitor>(visitor), variant_); }

        /**
         * @brief Visit with a callable that accepts the types in the variant (non-const version)
         *
         * Invokes the visitor with the currently active type using std::visit.
         * The visitor should be able to handle all types in the TypeEnum.
         *
         * @tparam Visitor A callable type that can handle all Types...
         * @param visitor The visitor callable to invoke
         * @return The result of invoking the visitor with the active type
         */
        template<typename Visitor>
        constexpr decltype(auto) visit(Visitor&& visitor)
        { return std::visit(std::forward<Visitor>(visitor), variant_); }

    private:
        /**
         * @brief Called by base class during visitation (const version)
         *
         * This is part of the CRTP pattern. The base class calls this method with
         * a compile-time index to invoke the visitor with the correct type.
         *
         * @tparam I The compile-time index of the type to visit
         * @tparam Visitor The visitor type
         * @param visitor The visitor to invoke
         * @return The result of invoking the visitor
         */
        template<std::size_t I, typename Visitor>
        constexpr decltype(auto) call_visitor_at(Visitor&& visitor) const
        { return std::forward<Visitor>(visitor)(std::get<I>(variant_)); }

        /**
         * @brief Called by base class during visitation (non-const version)
         *
         * This is part of the CRTP pattern. The base class calls this method with
         * a compile-time index to invoke the visitor with the correct type.
         *
         * @tparam I The compile-time index of the type to visit
         * @tparam Visitor The visitor type
         * @param visitor The visitor to invoke
         * @return The result of invoking the visitor
         */
        template<std::size_t I, typename Visitor>
        constexpr decltype(auto) call_visitor_at(Visitor&& visitor)
        { return std::forward<Visitor>(visitor)(std::get<I>(variant_)); }
    };

}    // namespace fxt
