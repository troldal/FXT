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
 * @brief fxt::flat_tuple — a tuple designed for fast compilation
 *
 * ## Design goal
 *
 * `fxt::flat_tuple` targets compilation speed, not a particular runtime memory
 * layout.  `std::tuple`'s most common implementation uses a recursive class
 * hierarchy that grows O(N) deep in template instantiation; `flat_tuple` uses
 * non-recursive **multiple inheritance from leaf base classes** expanded in a
 * single pack-expansion step, giving O(1) instantiation depth for construction
 * and element access regardless of the element count.
 *
 * ## Storage model
 *
 * Each element `Ts[I]` is stored in a private base class `impl::flat_leaf<I, Ts[I]>`.
 * An intermediate `impl::flat_storage_base<index_sequence<Is...>, Ts...>` expands
 * all leaves simultaneously:
 *
 * @code
 * flat_tuple<int, double>
 *   : flat_storage_base<{0,1}, int, double>
 *       : flat_leaf<0, int>    { int value; }
 *       : flat_leaf<1, double> { double value; }
 * @endcode
 *
 * The memory footprint is the same as a plain `struct { int e0; double e1; }` —
 * identical to `std::tuple` (and much smaller than the old `std::array<std::variant<…>>` design).
 * Element access is a single `static_cast` to the appropriate base, with no
 * variant tag check or runtime indirection.
 *
 * ## Comparison with fxt::tuple (std::tuple)
 *
 * | Property | fxt::tuple (std::tuple) | fxt::flat_tuple |
 * |---|---|---|
 * | Instantiation depth | O(N) recursive | O(1) non-recursive |
 * | Element size | size of element | size of element |
 * | Memory layout | similar to struct | similar to struct |
 * | Element access | recursive base cast | direct `static_cast` |
 * | Comparison operators | yes (since C++20) | yes (`operator==` / `<=>`) |
 * | Structured bindings | yes | yes |
 * | `constexpr` usability | yes | yes |
 *
 * ## Main Functions
 *
 * ### fxt::make_flat_tuple
 * Factory function mirroring `std::make_tuple`. Uses `std::decay_t` on deduced
 * argument types.
 *
 * ## Examples
 *
 * ### Creating and accessing flat_tuples
 * @code
 * fxt::flat_tuple<int, double, float> ft(42, 3.14, 2.71f);
 *
 * int   a = fxt::get<0>(ft);          // by index
 * float c = fxt::get<float>(ft);      // by (unique) type
 *
 * auto ft2 = fxt::make_flat_tuple(42, 3.14, 2.71f);
 * @endcode
 *
 * ### Comparison
 * @code
 * auto a = fxt::make_flat_tuple(1, 2, 3);
 * auto b = fxt::make_flat_tuple(1, 2, 4);
 * assert(a == a);
 * assert(a != b);
 * assert(a < b);
 * @endcode
 *
 * ### Interoperability with fxt::tuple
 * @code
 * auto t  = fxt::make_tuple(1, 2, 3);
 * auto ft = fxt::make_flat_tuple(1, 2, 3);
 *
 * auto t_rev  = t  | fxt::tuple_reverse();
 * auto ft_rev = ft | fxt::tuple_reverse();
 *
 * static_assert(fxt::impl::is_fxt_tuple_v<decltype(t_rev)>);
 * static_assert(fxt::impl::is_flat_tuple_v<decltype(ft_rev)>);
 * @endcode
 *
 * @see fxt::tuple
 * @see fxt::get
 * @see fxt::tuple_size
 * @see fxt::tuple_element
 */

#pragma once

#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // ====================================================================
        // flat_leaf<I, T> — stores one element at a compile-time index.
        //
        // Using a separate struct per element (rather than a variant slot)
        // eliminates the discriminator byte and the max-sizeof overhead of the
        // old std::array<std::variant<...>> design, and makes element access a
        // plain static_cast with no runtime branching.
        // ====================================================================
        template<std::size_t I, typename T>
        struct flat_leaf
        {
            T value;

            constexpr flat_leaf() = default;

            // Perfect-forwarding constructor: accepts any U convertible to T,
            // fixing the old design which took Ts... by value and forced an
            // extra copy/move even for lvalue arguments.
            template<typename U>
                requires (!std::same_as<std::remove_cvref_t<U>, flat_leaf>)
            constexpr explicit flat_leaf(U&& v)
                noexcept(std::is_nothrow_constructible_v<T, U&&>)
                : value(std::forward<U>(v)) {}
        };

        // ====================================================================
        // flat_storage_base<index_sequence<Is...>, Ts...>
        //
        // Inherits from ALL flat_leaf<Is, Ts>... in a SINGLE non-recursive
        // pack-expansion step. This gives O(1) template instantiation depth
        // regardless of the number of elements — the key compile-speed
        // advantage over std::tuple's recursive chain.
        // ====================================================================
        template<typename IdxSeq, typename... Ts>
        struct flat_storage_base;

        template<std::size_t... Is, typename... Ts>
        struct flat_storage_base<std::index_sequence<Is...>, Ts...>
            : flat_leaf<Is, Ts>...
        {
            constexpr flat_storage_base() = default;

            template<typename... Us>
            constexpr explicit flat_storage_base(Us&&... args)
                noexcept((std::is_nothrow_constructible_v<Ts, Us&&> && ...))
                : flat_leaf<Is, Ts>(std::forward<Us>(args))... {}
        };
    }    // namespace impl

    // ========================================================================
    // fxt::flat_tuple<Ts...>
    // ========================================================================
    template<typename... Ts>
    class flat_tuple
        : private impl::flat_storage_base<std::index_sequence_for<Ts...>, Ts...>
    {
        using Base = impl::flat_storage_base<std::index_sequence_for<Ts...>, Ts...>;

    public:
        constexpr flat_tuple() = default;

        template<typename... Us>
            requires (sizeof...(Us) > 0 && sizeof...(Us) == sizeof...(Ts))
        constexpr explicit flat_tuple(Us&&... args)
            noexcept((std::is_nothrow_constructible_v<Ts, Us&&> && ...))
            : Base(std::forward<Us>(args)...) {}

        static constexpr std::size_t size() noexcept { return sizeof...(Ts); }

        // O(1) element-type alias via base-class overload deduction.
        //
        // Base publicly inherits from flat_leaf<I,T> for each (I,T) pair, so
        // overload resolution on `leaf_type_id<I>(Base*)` deduces the unique T
        // for that I without recursion and without touching std::tuple.
        //
        // The function returns std::type_identity<T> (a defined, trivially
        // default-constructible empty struct) so the IDE sees a complete
        // definition and does not warn about a missing implementation. The body
        // is never actually executed: decltype operates in an unevaluated context.
        template<std::size_t I, typename T>
        static constexpr std::type_identity<T> leaf_type_id(impl::flat_leaf<I, T>*)
        { return {};}

        template<std::size_t I>
        using type_at = decltype(leaf_type_id<I>(static_cast<Base*>(nullptr)))::type;

        // Tag type shared between this class and Get.hpp so both sides of the
        // friend declaration name the same constrained function template without
        // introducing a circular include dependency.
        struct flat_tuple_tag {};

        // Single forwarding-ref friend; the constrained definition is in Get.hpp.
        template<std::size_t I, typename FlatTupleT>
            requires requires { typename std::remove_cvref_t<FlatTupleT>::flat_tuple_tag; }
        friend constexpr decltype(auto) get(FlatTupleT&&) noexcept;

        // ====================================================================
        // Comparison — element-wise, lexicographic, short-circuit
        // ====================================================================

        // operator== is enabled when every element type is equality comparable.
        friend constexpr bool operator==(const flat_tuple& a, const flat_tuple& b)
            requires (std::equality_comparable<Ts> && ...)
        {
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return ((static_cast<const impl::flat_leaf<Is, Ts>&>(a).value ==
                         static_cast<const impl::flat_leaf<Is, Ts>&>(b).value) && ...);
            }(std::index_sequence_for<Ts...>{});
        }

        // operator<=> returns the common comparison category of all element
        // types and short-circuits on the first non-equivalent pair.
        friend constexpr auto operator<=>(const flat_tuple& a, const flat_tuple& b)
            requires (std::three_way_comparable<Ts> && ...)
        {
            using Cat = std::common_comparison_category_t<
                std::compare_three_way_result_t<Ts>...>;
            Cat result = Cat::equivalent;
            [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                // The || fold drives short-circuit evaluation but its bool result
                // is intentionally discarded — we only need the side-effect of
                // setting `result`.  static_cast<void> suppresses -Wunused-value.
                static_cast<void>(
                    ((result = static_cast<const impl::flat_leaf<Is, Ts>&>(a).value <=>
                                static_cast<const impl::flat_leaf<Is, Ts>&>(b).value,
                      result != 0) || ...));
            }(std::index_sequence_for<Ts...>{});
            return result;
        }
    };

    /**
     * @brief Create a flat_tuple with automatic type deduction from arguments.
     *
     * Mirrors `std::make_tuple`. Uses `std::decay_t` to strip references and
     * cv-qualifiers from the deduced types.
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

    // ========================================================================
    // Pipe operators — four value-category overloads
    // ========================================================================

    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(const flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(const flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

}    // namespace fxt
