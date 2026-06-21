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

/**
 * @file FlatTuple.hpp
 * @brief Flat tuple type with contiguous memory layout for the FXT library
 *
 * This file provides the flat_tuple type, an alternative tuple implementation with a contiguous
 * memory layout. Unlike std::tuple which may have padding between elements, flat_tuple stores
 * all elements in a single contiguous array, potentially offering better cache locality and
 * memory efficiency for certain use cases.
 *
 * ## Main Types
 *
 * ### fxt::flat_tuple
 * A tuple-like container that stores elements in a flat, contiguous memory layout.
 * It provides similar semantics to std::tuple and fxt::tuple, but with different
 * internal representation using std::array and std::variant for element storage.
 *
 * **Key Characteristics:**
 * - Contiguous memory layout (all elements stored in a single array)
 * - Potentially better cache locality than std::tuple
 * - Compatible with all FXT tuple operations (transform, reverse, append, etc.)
 * - Supports empty tuples (zero elements)
 * - Works seamlessly with FXT's pipeline operators
 *
 * **Implementation Details:**
 * - Uses `std::array<std::variant<...>, N>` for storage
 * - Each element is wrapped in an indexed variant for type safety
 * - Provides compile-time size information via `size()` method
 * - Friend declarations enable `fxt::get` access to internal storage
 *
 * ## Main Functions
 *
 * ### fxt::make_flat_tuple
 * Factory function to create a flat_tuple with automatic type deduction from arguments.
 * Similar to `std::make_tuple` but creates a `flat_tuple` instead. Uses `std::decay_t`
 * to remove references and cv-qualifiers from deduced types.
 *
 * **Type Deduction:**
 * - Removes references (T& → T)
 * - Removes cv-qualifiers (const T → T)
 * - Decays arrays and functions to pointers
 *
 * ## Pipe Operators
 *
 * This file defines pipe operators (`operator|`) for `fxt::flat_tuple` that enable
 * functional-style composition, identical to those provided for `fxt::tuple`.
 *
 * ### Pipe Operator Overloads
 * - **Lvalue reference**: `flat_tuple& | callable` - Pipes a mutable lvalue flat_tuple
 * - **Const lvalue reference**: `const flat_tuple& | callable` - Pipes a const lvalue flat_tuple
 * - **Rvalue reference**: `flat_tuple&& | callable` - Pipes an rvalue flat_tuple (move semantics)
 * - **Const rvalue reference**: `const flat_tuple&& | callable` - Pipes a const rvalue flat_tuple
 *
 * The pipe operators use `std::invoke` to call the callable with the flat_tuple, supporting
 * function pointers, function objects, lambdas, and member function pointers.
 *
 * ## Key Features
 * - Contiguous memory layout for better cache performance
 * - Compatible with all FXT tuple operations
 * - Perfect forwarding support for construction
 * - Pipeline-friendly design with pipe operator support
 * - Universal value category support (lvalue, rvalue, const)
 * - Type-safe element access via friend `fxt::get` functions
 * - Zero-overhead for empty tuples
 *
 * ## Performance Considerations
 * - Better cache locality than std::tuple in many scenarios
 * - Contiguous storage may improve iteration performance
 * - Similar compile-time overhead to std::tuple
 * - Variant-based storage adds small runtime overhead per element access
 * - Best suited for tuples with multiple elements of similar size
 *
 * ## Examples
 *
 * ### Creating flat_tuples
 * @code
 * // Direct construction (requires explicit types)
 * fxt::flat_tuple<int, double, float> ft1{42, 3.14, 2.71f};
 *
 * // Using make_flat_tuple with type deduction
 * auto ft2 = fxt::make_flat_tuple(42, 3.14, 2.71f);
 * // Type: fxt::flat_tuple<int, double, float>
 *
 * // Empty flat_tuple
 * fxt::flat_tuple<> empty;
 * @endcode
 *
 * ### Accessing elements
 * @code
 * auto ft = fxt::make_flat_tuple(1, 2.5, 3.0f);
 *
 * // Access by index using fxt::get
 * int first = fxt::get<0>(ft);      // 1
 * double second = fxt::get<1>(ft);  // 2.5
 * float third = fxt::get<2>(ft);    // 3.0f
 * @endcode
 *
 * ### Using with FXT operations
 * @code
 * auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5);
 *
 * // Transform elements
 * auto doubled = ft | fxt::tuple_transform([](auto x) { return x * 2; });
 * // Result: fxt::flat_tuple<int, int, int, int, int>{2, 4, 6, 8, 10}
 *
 * // Reverse order
 * auto reversed = ft | fxt::tuple_reverse();
 * // Result: fxt::flat_tuple<int, int, int, int, int>{5, 4, 3, 2, 1}
 *
 * // Chain operations
 * auto result = ft
 *     | fxt::drop<2>()
 *     | fxt::tuple_reverse()
 *     | fxt::take<2>();
 * // Result: fxt::flat_tuple<int, int>{5, 4}
 * @endcode
 *
 * ### Using the pipe operator
 * @code
 * auto ft = fxt::make_flat_tuple(10, 20, 30);
 *
 * // Pipe to transformation
 * auto result = ft | fxt::tuple_transform([](auto x) { return x + 1; });
 * // Result: fxt::flat_tuple<int, int, int>{11, 21, 31}
 *
 * // Pipe with rvalue
 * auto result2 = fxt::make_flat_tuple(5, 10, 15)
 *     | fxt::tuple_reverse();
 * // Result: fxt::flat_tuple<int, int, int>{15, 10, 5}
 * @endcode
 *
 * ### Interoperability with fxt::tuple
 * @code
 * // Operations work the same way on both types
 * auto t = fxt::make_tuple(1, 2, 3);
 * auto ft = fxt::make_flat_tuple(1, 2, 3);
 *
 * // Both support the same operations
 * auto t_reversed = t | fxt::tuple_reverse();    // fxt::tuple<int, int, int>
 * auto ft_reversed = ft | fxt::tuple_reverse();  // fxt::flat_tuple<int, int, int>
 *
 * // Type is preserved through operations
 * static_assert(fxt::impl::is_fxt_tuple_v<decltype(t_reversed)>);
 * static_assert(fxt::impl::is_flat_tuple_v<decltype(ft_reversed)>);
 * @endcode
 *
 * ### Working with monads
 * @code
 * auto ft = fxt::make_flat_tuple(1, 2, 3);
 * auto exp = fxt::expected<fxt::flat_tuple<int, int, int>, Error>{ft};
 *
 * // Monadic operations preserve flat_tuple type
 * auto result = exp
 *     | fxt::mtuple_reverse()
 *     | fxt::mtransform_tuple([](auto x) { return x * 2; });
 * // Result: fxt::expected<fxt::flat_tuple<int, int, int>, Error>
 * @endcode
 *
 * ## Type Summary
 *
 * | Type | Description |
 * |------|-------------|
 * | `fxt::flat_tuple<Ts...>` | Tuple with contiguous memory layout, alternative to std::tuple |
 *
 * ## Function Summary
 *
 * | Function | Description |
 * |----------|-------------|
 * | `fxt::make_flat_tuple(args...)` | Create a flat_tuple with type deduction from arguments |
 * | `flat_tuple<Ts...>::size()` | Static method returning the number of elements (constexpr) |
 *
 * ## Operator Summary
 *
 * | Operator | Description |
 * |----------|-------------|
 * | `flat_tuple& \| callable` | Pipe lvalue flat_tuple to callable |
 * | `const flat_tuple& \| callable` | Pipe const lvalue flat_tuple to callable |
 * | `flat_tuple&& \| callable` | Pipe rvalue flat_tuple to callable |
 * | `const flat_tuple&& \| callable` | Pipe const rvalue flat_tuple to callable |
 *
 * @see fxt::tuple
 * @see fxt::tuple_transform
 * @see fxt::tuple_reverse
 * @see fxt::tuple_append
 * @see fxt::tuple_size
 * @see fxt::get
 */

#pragma once

#include <array>
#include <concepts>
#include <type_traits>
#include <utility>
#include <variant>

namespace fxt
{

    // TODO: DOCS/DESIGN — the file comment above oversells flat_tuple: storage is
    //       std::array<std::variant<indexed<Is,Ts>...>, N>, so EVERY slot occupies
    //       max(sizeof(Ts)...) plus a discriminator. For heterogeneous element sizes this is
    //       usually LARGER than std::tuple and the "contiguous memory / better cache
    //       locality" claim only holds for same-sized elements; element access also goes
    //       through a variant. Either document the real trade-offs or reimplement with
    //       aligned byte storage.
    // TODO: COMPLETENESS — flat_tuple lacks std::tuple_size / std::tuple_element
    //       specializations and an ADL get, so structured bindings (`auto [a, b] = ft;`)
    //       and std::apply do not work, unlike fxt::tuple. It also has no operator== /
    //       operator<=>, so two flat_tuples cannot be compared.
    // TODO: SAFETY — the variadic constructor takes `Ts... args` by value via the
    //       index_sequence overload, forcing a copy/move per element even when Args are
    //       lvalues that could bind by reference; consider perfect forwarding to the
    //       indexed wrappers.
    template<class... Ts>
    class flat_tuple {
        template<size_t I, class T>
        struct indexed { T value; };

        template<size_t I, class... Us>
        struct type_at_impl;

        template<size_t I, class U, class... Us>
        struct type_at_impl<I, U, Us...> {
            using type = typename type_at_impl<I - 1, Us...>::type;
        };

        template<class U, class... Us>
        struct type_at_impl<0, U, Us...> {
            using type = U;
        };

        template<size_t I>
        using type_at = typename type_at_impl<I, Ts...>::type;

        template<class Seq>
        struct make_variant;

        template<size_t... Is>
        struct make_variant<std::index_sequence<Is...>> {
            // Use std::monostate if the parameter pack is empty, otherwise use the indexed types
            using type = std::conditional_t<
                sizeof...(Is) == 0,
                std::variant<std::monostate>,
                std::variant<indexed<Is, Ts>...>
            >;
        };

        using variant_t = typename make_variant<std::index_sequence_for<Ts...>>::type;

        // For empty tuple, use std::array with size 0 (which is valid and has special behavior)
        std::array<variant_t, sizeof...(Ts)> values;

    public:
        // Constructor for non-empty tuples. constexpr so a flat_tuple of literal
        // element types can be built and accessed in constant expressions, matching
        // std::tuple's constexpr usability.
        template<size_t... Is>
        constexpr explicit flat_tuple(std::index_sequence<Is...>, Ts... args)
            : values{variant_t(std::in_place_index<Is>, indexed<Is, Ts>{std::move(args)})...} {}

        // Default constructor for empty tuples
        flat_tuple() = default;

        // Constructor that forwards to the index_sequence version (only enabled for non-empty)
        template<typename... Args>
            requires (sizeof...(Args) > 0 && sizeof...(Args) == sizeof...(Ts))
        constexpr explicit flat_tuple(Args&&... args)
            : flat_tuple(std::index_sequence_for<Ts...>{}, std::forward<Args>(args)...) {}

        // template<class F>
        // void visit_all(F&& f) {
        //     [&]<size_t... Is>(std::index_sequence<Is...>) {
        //         (std::forward<F>(f)(std::get<indexed<Is, type_at<Is>>>(values[Is]).value), ...);
        //     }(std::index_sequence_for<Ts...>{});
        // }

        static constexpr size_t size() { return sizeof...(Ts); }

        // Tag type used as a self-contained constraint in the friend declaration and
        // the fxt::get definition so both sides name the same function template
        // without introducing a circular include dependency.
        struct flat_tuple_tag {};

        // Single forwarding-ref friend; definition is the constexpr noexcept
        // overload in Get.hpp.  The requires-clause is identical on both sides,
        // satisfying C++20 constraint-identity rules.
        template<std::size_t I, typename FlatTupleT>
            requires requires { typename std::remove_cvref_t<FlatTupleT>::flat_tuple_tag; }
        friend constexpr decltype(auto) get(FlatTupleT&&) noexcept;
    };

    /**
     * @brief Create a flat_tuple, deducing the target type from the types of arguments
     *
     * Similar to std::make_tuple but creates a flat_tuple instead.
     * Automatically deduces types and uses std::decay to remove references and cv-qualifiers.
     *
     * @tparam Ts Types of the elements (deduced)
     * @param args Values to initialize the flat_tuple with
     * @return A flat_tuple containing the given values
     *
     * @code
     * auto ft = fxt::make_flat_tuple(42, 3.14, 99.9f);
     * // Creates fxt::flat_tuple<int, double, float>
     * @endcode
     */
    template<typename... Ts>
    constexpr auto make_flat_tuple(Ts&&... args)
    {
        return flat_tuple<std::decay_t<Ts>...>(std::forward<Ts>(args)...);
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (lvalue reference)
     *
     * Allows piping a flat_tuple to a callable function, enabling functional-style composition.
     * The callable receives the flat_tuple and returns the result.
     *
     * @tparam Ts Types in the flat_tuple
     * @tparam Callable Type of the callable
     * @param tuple The flat_tuple to pipe
     * @param callable The callable to apply to the flat_tuple
     * @return The result of invoking the callable with the flat_tuple
     *
     * @code
     * fxt::flat_tuple<int, double> t{42, 3.14};
     * auto result = t | fxt::get<0>;  // Returns 42
     * @endcode
     */
    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (const lvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(const flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (const rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(const flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

}