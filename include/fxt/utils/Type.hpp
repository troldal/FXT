//
// Created by kenne on 27/02/2025.
//

#pragma once

#include <utility>
#include <type_traits>

namespace fxt
{
    /**
     * @brief A type wrapper utility for tag dispatch, type manipulation, and value association
     *
     * Type is a versatile template that serves two primary purposes through template specialization:
     *
     * 1. **Primary Template** - A lightweight, zero-overhead type tag for compile-time type
     *    manipulation and tag dispatch patterns. It wraps a type without storing any value,
     *    making it useful for overload resolution and type-based dispatching.
     *
     * 2. **Partial Specialization** - Associates a compile-time type tag with a runtime value,
     *    enabling patterns where you need to carry both a type identity and a value of a
     *    potentially different type.
     *
     * @tparam TType The wrapped type that serves as the type identity
     * @tparam TArgs Additional template parameters (when present, enables value storage)
     *
     * @section primary_usage Primary Template Usage (Type Tag)
     * @code
     * // Tag dispatch pattern - selecting function overloads by type
     * void process(fxt::Type<int>) {
     *     std::cout << "Processing integer\n";
     * }
     *
     * void process(fxt::Type<std::string>) {
     *     std::cout << "Processing string\n";
     * }
     *
     * // Can be called with type tag or implicit conversion
     * process(fxt::Type<int>{});
     * process(42);  // Implicit conversion from int
     *
     * // Type manipulation
     * using IntTag = fxt::Type<int>;
     * using WrappedType = typename IntTag::type;  // int
     * @endcode
     *
     * @section specialization_usage Specialization Usage (Type with Value)
     * @code
     * // Store a runtime value with a compile-time type tag
     * fxt::Type<std::string, double> typeValue(3.14);
     *
     * // Access the type tag
     * using TagType = typename decltype(typeValue)::type;  // std::string
     *
     * // Access the value type
     * using ValueType = typename decltype(typeValue)::value_type;  // double
     *
     * // Retrieve the stored value
     * double pi = typeValue.value();  // 3.14
     * double& ref = typeValue.get();  // Reference access
     * @endcode
     *
     * @section helper_functions Helper Functions
     * @code
     * // Create a pure type tag
     * auto intTag = fxt::make_type<int>();
     *
     * // Create a type with value
     * auto typeVal = fxt::make_type<std::string>(42);
     *
     * // Check if something is a Type
     * static_assert(fxt::is_type_v<decltype(intTag)>);
     *
     * // Extract type from Type wrapper
     * using T = fxt::type_t<decltype(intTag)>;  // int
     * @endcode
     *
     * @note The primary template has implicit conversion constructors to enable
     *       convenient tag dispatch, while the specialization uses explicit constructors
     *       to prevent accidental conversions.
     * @note Both versions are fully constexpr-enabled for compile-time computation.
     */
    template<typename TType, typename... TArgs>
    struct Type
    {
        /**
         * @brief Type alias for the wrapped type
         *
         * Provides access to the type identity of this Type wrapper.
         * Useful for extracting the type in template metaprogramming contexts.
         */
        using type = TType;

        /**
         * @brief Default constructor for tag dispatch usage
         *
         * Creates an empty type tag that can be used for overload resolution.
         * This constructor is noexcept and constexpr, making it suitable for
         * compile-time contexts and guaranteeing no exceptions.
         */
        constexpr Type() noexcept = default;

        /**
         * @brief Implicit conversion constructor from const lvalue reference
         *
         * Enables convenient tag dispatch by allowing implicit conversion from
         * values of the wrapped type. The value is discarded as this is a pure
         * type tag with no storage.
         *
         * @param TType const lvalue reference (value is discarded)
         *
         * @example
         * @code
         * void foo(fxt::Type<int>);
         * foo(42);  // Implicitly converts 42 to Type<int>
         * @endcode
         */
        constexpr Type(const TType&) noexcept {}

        /**
         * @brief Implicit conversion constructor from rvalue reference
         *
         * Enables convenient tag dispatch by allowing implicit conversion from
         * rvalue references of the wrapped type. The value is discarded.
         *
         * @param TType rvalue reference (value is discarded)
         */
        constexpr Type(TType&&) noexcept {}

        // Allow copy and move operations
        constexpr Type(const Type&) noexcept = default;
        constexpr Type(Type&&) noexcept = default;
        constexpr Type& operator=(const Type&) noexcept = default;
        constexpr Type& operator=(Type&&) noexcept = default;
    };

    /**
     * @brief Partial specialization of Type that stores a runtime value
     *
     * This specialization associates a compile-time type tag (TType) with a
     * runtime value of type TValue. This is useful for patterns where you need
     * to carry both a type identity and a value of a different type.
     *
     * The specialization provides:
     * - Full value semantics (copy, move, comparison)
     * - Multiple accessors with appropriate ref-qualifiers
     * - Implicit conversion to the stored value type
     * - Comparison operators (when supported by TValue)
     *
     * @tparam TType The type tag (compile-time type identity)
     * @tparam TValue The type of the stored runtime value
     *
     * @example
     * @code
     * // Create with explicit type and value
     * fxt::Type<std::string, int> tagged(42);
     *
     * // The type tag is std::string, but the value is int
     * using Tag = typename decltype(tagged)::type;        // std::string
     * using Val = typename decltype(tagged)::value_type;  // int
     *
     * int x = tagged.value();  // Extract value: 42
     *
     * // Move semantics
     * auto moved = std::move(tagged).value();  // Moves the value out
     *
     * // Comparison (if TValue supports it)
     * fxt::Type<std::string, int> a(10), b(10);
     * bool equal = (a == b);  // true
     * @endcode
     */
    template<typename TType, typename TValue>
    struct Type<TType, TValue>
    {
        /**
         * @brief Type alias for the type tag
         *
         * The compile-time type identity associated with this wrapper.
         */
        using type = TType;

        /**
         * @brief Type alias for the stored value type
         *
         * The actual runtime type of the stored value.
         */
        using value_type = TValue;

        /**
         * @brief Constructor from const lvalue reference
         *
         * Creates a Type wrapper by copying the provided value.
         * Marked explicit to prevent accidental implicit conversions.
         *
         * @param value The value to store (copied)
         */
        constexpr explicit Type(const TValue& value) : m_value(value) {}

        /**
         * @brief Constructor from rvalue reference
         *
         * Creates a Type wrapper by moving the provided value.
         * Marked explicit to prevent accidental implicit conversions.
         *
         * @param value The value to store (moved)
         */
        constexpr explicit Type(TValue&& value) : m_value(std::move(value)) {}

        // Copy and move constructors
        constexpr Type(const Type&) = default;
        constexpr Type(Type&&) noexcept(std::is_nothrow_move_constructible_v<TValue>) = default;

        // Copy and move assignment
        constexpr Type& operator=(const Type&) = default;
        constexpr Type& operator=(Type&&) noexcept(std::is_nothrow_move_assignable_v<TValue>) = default;

        // Destructor
        ~Type() = default;

        /**
         * @brief Access the stored value (const lvalue overload)
         *
         * Returns a copy of the stored value. Use get() for reference access.
         *
         * @return Copy of the stored value
         */
        [[nodiscard]] constexpr TValue value() const& { return m_value; }

        /**
         * @brief Access the stored value (non-const lvalue overload)
         *
         * Returns a mutable reference to the stored value.
         *
         * @return Mutable reference to the stored value
         */
        [[nodiscard]] constexpr TValue& value() & { return m_value; }

        /**
         * @brief Access the stored value (rvalue overload)
         *
         * Returns an rvalue reference to the stored value, enabling move semantics.
         *
         * @return Rvalue reference to the stored value
         */
        [[nodiscard]] constexpr TValue&& value() && { return std::move(m_value); }

        /**
         * @brief Access the stored value (const rvalue overload)
         *
         * Returns a const rvalue reference to the stored value.
         *
         * @return Const rvalue reference to the stored value
         */
        [[nodiscard]] constexpr const TValue&& value() const&& { return std::move(m_value); }

        /**
         * @brief Get const reference to the stored value
         *
         * More efficient than value() when you don't need a copy.
         *
         * @return Const reference to the stored value
         */
        [[nodiscard]] constexpr const TValue& get() const& noexcept { return m_value; }

        /**
         * @brief Get mutable reference to the stored value
         *
         * More efficient than value() when you don't need a copy.
         *
         * @return Mutable reference to the stored value
         */
        [[nodiscard]] constexpr TValue& get() & noexcept { return m_value; }

        /**
         * @brief Get const rvalue reference to the stored value
         *
         * @return Const rvalue reference to the stored value
         */
        [[nodiscard]] constexpr const TValue&& get() const&& noexcept { return std::move(m_value); }

        /**
         * @brief Get rvalue reference to the stored value
         *
         * Enables moving the value out of the wrapper.
         *
         * @return Rvalue reference to the stored value
         */
        [[nodiscard]] constexpr TValue&& get() && noexcept { return std::move(m_value); }

        /**
         * @brief Implicit conversion to const reference
         *
         * Allows the Type wrapper to be used in contexts expecting TValue.
         *
         * @return Const reference to the stored value
         */
        [[nodiscard]] constexpr operator const TValue&() const& noexcept { return m_value; }

        /**
         * @brief Implicit conversion to mutable reference
         *
         * Allows the Type wrapper to be used in contexts expecting TValue.
         *
         * @return Mutable reference to the stored value
         */
        [[nodiscard]] constexpr operator TValue&() & noexcept { return m_value; }

        /**
         * @brief Implicit conversion to rvalue reference
         *
         * Allows the Type wrapper to be used in contexts expecting TValue.
         *
         * @return Rvalue reference to the stored value
         */
        [[nodiscard]] constexpr operator TValue&&() && noexcept { return std::move(m_value); }

        /**
         * @brief Equality comparison operator
         *
         * Compares two Type wrappers by comparing their stored values.
         * Only available when TValue supports equality comparison.
         *
         * @param other Another Type wrapper to compare with
         * @return true if the stored values are equal, false otherwise
         */
        [[nodiscard]] constexpr bool operator==(const Type& other) const
            noexcept(noexcept(m_value == other.m_value))
            requires requires(const TValue& a, const TValue& b) { { a == b } -> std::convertible_to<bool>; }
        {
            return m_value == other.m_value;
        }

        /**
         * @brief Three-way comparison operator
         *
         * Provides ordering comparison for Type wrappers by comparing stored values.
         * Only available when TValue supports three-way comparison.
         *
         * @param other Another Type wrapper to compare with
         * @return The result of three-way comparison of the stored values
         */
        [[nodiscard]] constexpr auto operator<=>(const Type& other) const
            noexcept(noexcept(m_value <=> other.m_value))
            requires requires(const TValue& a, const TValue& b) { a <=> b; }
        {
            return m_value <=> other.m_value;
        }

    private:
        TValue m_value;  ///< The stored runtime value
    };

    /**
     * @brief Helper alias for extracting the type from a Type wrapper
     *
     * Provides a convenient way to access the ::type member of a Type wrapper
     * or any other type that provides a ::type member alias.
     *
     * @tparam T A type providing a ::type member alias
     *
     * @example
     * @code
     * using IntType = fxt::type_t<fxt::Type<int>>;  // int
     * @endcode
     */
    template<typename T>
    using type_t = typename T::type;

    /**
     * @brief Helper alias for extracting the value_type from a Type wrapper
     *
     * Provides a convenient way to access the ::value_type member of a Type wrapper
     * with value storage, or any other type that provides a ::value_type member alias.
     *
     * @tparam T A type providing a ::value_type member alias
     *
     * @example
     * @code
     * using ValType = fxt::value_type_t<fxt::Type<std::string, int>>;  // int
     * @endcode
     */
    template<typename T>
    using value_type_t = typename T::value_type;

    /**
     * @brief Helper function to create a Type wrapper with a stored value
     *
     * Creates a Type<TType, TValue> where TType must be explicitly specified
     * and TValue is deduced from the provided argument. The value is perfectly
     * forwarded and decayed to remove references and cv-qualifiers.
     *
     * @tparam TType The type tag (must be explicitly specified)
     * @tparam TValue The value type (automatically deduced)
     * @param value The value to store in the Type wrapper
     * @return A Type<TType, std::decay_t<TValue>> containing the value
     *
     * @example
     * @code
     * auto tagged = fxt::make_type<std::string>(42);
     * // Type is: fxt::Type<std::string, int>
     *
     * auto moved = fxt::make_type<MyClass>(std::move(obj));
     * // Moves obj into the wrapper
     * @endcode
     */
    template<typename TType, typename TValue>
    [[nodiscard]] constexpr auto make_type(TValue&& value) {
        return Type<TType, std::decay_t<TValue>>(std::forward<TValue>(value));
    }

    /**
     * @brief Helper function to create a pure type tag without value storage
     *
     * Creates a Type<TType> that serves as a compile-time type tag with no
     * runtime value storage. Useful for tag dispatch and type-based overloading.
     *
     * @tparam TType The type to wrap (must be explicitly specified)
     * @return A Type<TType> instance
     *
     * @example
     * @code
     * auto intTag = fxt::make_type<int>();
     * auto stringTag = fxt::make_type<std::string>();
     *
     * process(intTag);     // Calls process(Type<int>)
     * process(stringTag);  // Calls process(Type<std::string>)
     * @endcode
     */
    template<typename TType>
    [[nodiscard]] constexpr auto make_type() noexcept {
        return Type<TType>{};
    }

    /**
     * @brief Type trait to detect if a type is a Type wrapper
     *
     * Provides a compile-time boolean constant that is true if T is an
     * instantiation of the Type template, false otherwise.
     *
     * Inherits from std::true_type for Type instantiations and std::false_type
     * for all other types, providing a standard type trait interface.
     *
     * @tparam T The type to check
     *
     * @example
     * @code
     * static_assert(fxt::is_type<fxt::Type<int>>::value);
     * static_assert(!fxt::is_type<int>::value);
     *
     * // Or use the _v helper
     * static_assert(fxt::is_type_v<fxt::Type<std::string, double>>);
     * @endcode
     */
    template<typename T>
    struct is_type : std::false_type {};

    /**
     * @brief Specialization of is_type for Type instantiations
     */
    template<typename TType, typename... TArgs>
    struct is_type<Type<TType, TArgs...>> : std::true_type {};

    /**
     * @brief Variable template helper for is_type
     *
     * Provides convenient access to the is_type trait value.
     *
     * @tparam T The type to check
     */
    template<typename T>
    inline constexpr bool is_type_v = is_type<T>::value;

}    // namespace fxt