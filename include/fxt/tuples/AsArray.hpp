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

#include "../concepts/IsMonad.hpp"
#include "../concepts/IsTuple.hpp"
#include <array>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // Single forwarding-ref impl replaces the four typed overloads (const tuple&,
        // tuple&&, const flat_tuple&, flat_tuple&&). The if constexpr selects the
        // right accessor: std::get for fxt::tuple (= std::tuple), fxt::get for flat_tuple.
        template<typename T, typename TTuple, std::size_t... Is>
        constexpr auto as_array_impl(TTuple&& t, std::index_sequence<Is...>)
        { return std::array<T, sizeof...(Is)> { static_cast<T>(fxt::get<Is>(std::forward<TTuple>(t)))... }; }

        template<typename TTuple>
        consteval std::size_t as_array_size()
        { return std::tuple_size_v<std::remove_cvref_t<TTuple>>; }

        // Preserves the original per-overload `std::constructible_from` contract:
        // checks T is constructible from each element accessed with TTuple's value
        // category (const& for lvalues, && for rvalues).
        template<typename T, typename TTuple, std::size_t... Is>
        consteval bool as_array_constructible(std::index_sequence<Is...>)
        { return (std::constructible_from<T, decltype(fxt::get<Is>(std::declval<TTuple>()))> && ...); }
    }    // namespace impl

    // ========================================================================
    // fxt::tuple_as_array — convert a tuple to std::array
    // ========================================================================

    /**
     * @brief Convert a tuple-like object to a std::array
     *
     * Takes a tuple-like object (either fxt::tuple or fxt::flat_tuple) and converts
     * it to a std::array where all elements are cast to @p T.
     *
     * @tparam T The element type of the resulting array (must be constructible from all tuple elements)
     * @tparam Ts Tuple element types (deduced)
     * @param t The tuple to convert
     * @return std::array<T, N> where N is the number of elements in the tuple
     *
     * @code
     * auto tpl = fxt::make_tuple(1, 2.5, 3);
     * auto arr = fxt::tuple_as_array<double>(tpl);  // std::array<double, 3>{1.0, 2.5, 3.0}
     * @endcode
     */
    // Single forwarding-reference overload replacing the four typed overloads
    // (const tuple&, tuple&&, const flat_tuple&, flat_tuple&&). Constrained to
    // fxt::tuple / fxt::flat_tuple via is_tuple_like_v, and preserves the original
    // element-wise std::constructible_from contract. The accessor (std::get vs
    // fxt::get) and element count are selected inside the impl helpers.
    template<typename T, typename TTuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<TTuple>> &&
                 (impl::as_array_constructible<T, TTuple>(std::make_index_sequence<impl::as_array_size<TTuple>()> {}))
    constexpr auto tuple_as_array(TTuple&& t)
    { return impl::as_array_impl<T>(std::forward<TTuple>(t), std::make_index_sequence<impl::as_array_size<TTuple>()> {}); }

    // Deprecated two-argument forwarder covering all old as_array(t) call sites.
    template<typename T, typename TTuple>
    [[deprecated("Use fxt::tuple_as_array")]]
    constexpr auto as_array(TTuple&& t) -> decltype(tuple_as_array<T>(std::forward<TTuple>(t)))
    { return tuple_as_array<T>(std::forward<TTuple>(t)); }

    // ========================================================================
    // fxt::tuple_as_array (curried) — pipe-operator form
    // ========================================================================

    /**
     * @brief Curried tuple_as_array for pipe-operator usage.
     *
     * @tparam T The element type of the resulting array
     * @return A lambda that takes a tuple and returns std::array<T, N>
     *
     * @code
     * auto tpl = fxt::make_tuple(1, 2.5, 3);
     * auto arr = tpl | fxt::tuple_as_array<double>();  // std::array<double, 3>{1.0, 2.5, 3.0}
     * @endcode
     */
    template<typename T>
    constexpr auto tuple_as_array()
    {
        return []<typename TTuple>(TTuple&& t)
            requires tuple_like<std::remove_cvref_t<TTuple>>
        { return tuple_as_array<T>(std::forward<TTuple>(t)); };
    }

    template<typename T>
    [[deprecated("Use fxt::tuple_as_array")]]
    constexpr auto as_array()
    { return tuple_as_array<T>(); }

    // ========================================================================
    // fxt::mtuple_as_array — monadic as_array
    // ========================================================================

    namespace impl
    {
        template<typename T, typename MonadT>
        struct transformed_monad_type;

        template<typename T, typename MonadT>
            requires monad_like<MonadT>
        struct transformed_monad_type<T, MonadT>
        {
            using type = decltype(std::declval<MonadT>().transform([](auto&& val) { return static_cast<T>(val); }));
        };

        template<typename T, typename MonadT>
        using transformed_monad_type_t = transformed_monad_type<T, MonadT>::type;

        // Single forwarding-ref impl replaces four typed overloads. Dispatches on
        // is_flat_tuple_v to pick the right accessor (fxt::get vs std::get).
        template<typename T, typename TTuple, std::size_t... Is>
        constexpr auto mas_array_impl_monadic_elements(TTuple&& t, std::index_sequence<Is...>)
        {
            using CleanTuple        = std::remove_cvref_t<TTuple>;
            using elem_type         = std::tuple_element_t<0, CleanTuple>;
            using result_monad_type = transformed_monad_type_t<T, elem_type>;
            return std::array<result_monad_type, sizeof...(Is)> { fxt::get<Is>(std::forward<TTuple>(t)).transform([](auto&& val) {
                return static_cast<T>(val);
            })... };
        }

        template<typename... Ts>
        constexpr bool all_monadic_v = (sizeof...(Ts) > 0) && (monad_like<Ts> && ...);

        template<typename T>
        struct tuple_elements_are_monadic : std::false_type
        {
        };

        template<typename... Ts>
        struct tuple_elements_are_monadic<tuple<Ts...>> : std::bool_constant<all_monadic_v<Ts...>>
        {
        };

        template<typename... Ts>
        struct tuple_elements_are_monadic<flat_tuple<Ts...>> : std::bool_constant<all_monadic_v<Ts...>>
        {
        };
    }    // namespace impl

    /**
     * @brief Monadic version of tuple_as_array
     *
     * Works with monadic containers (optional/expected) holding tuples.
     * Two dispatch cases:
     *   1. Tuple elements are **not** monadic → converts tuple to `std::array<T, N>` inside the monad.
     *   2. Tuple elements **are** monadic → produces `std::array<Monad<T>, N>` where each element
     *      is individually transformed.
     *
     * @tparam T Target value type (or target inner-value type for monadic elements)
     * @return A callable that applies to a monadic container holding a tuple
     *
     * @code
     * // Case 1: optional<tuple<int, double, int>>
     * std::optional<fxt::tuple<int, double, int>> opt = fxt::make_tuple(1, 2.5, 3);
     * auto r1 = opt | fxt::mtuple_as_array<double>();
     * // r1: std::optional<std::array<double, 3>>
     *
     * // Case 2: expected<tuple<Exp, Exp, Exp>, E>
     * using Exp = fxt::expected<int, std::string>;
     * fxt::expected<fxt::tuple<Exp, Exp, Exp>, std::string> exp = ...;
     * auto r2 = exp | fxt::mtuple_as_array<double>();
     * // r2: fxt::expected<std::array<fxt::expected<double, std::string>, 3>, std::string>
     * @endcode
     */
    template<typename T>
    constexpr auto mtuple_as_array()
    {
        return []<typename TMonad>(TMonad&& monad) {
            return std::forward<TMonad>(monad).transform([]<typename TTuple>(TTuple&& tuple_val) {
                using tuple_type = std::remove_cvref_t<TTuple>;

                if constexpr (tuple_like<tuple_type>) {
                    if constexpr (impl::tuple_elements_are_monadic<tuple_type>::value) {
                        return impl::mas_array_impl_monadic_elements<T>(std::forward<TTuple>(tuple_val),
                                                                        std::make_index_sequence<std::tuple_size_v<tuple_type>> {});
                    }
                    else {
                        return tuple_as_array<T>(std::forward<TTuple>(tuple_val));
                    }
                }
                else {
                    static_assert(tuple_like<tuple_type>, "mtuple_as_array requires a tuple-like type inside the monad");
                }
            });
        };
    }

    template<typename T>
    [[deprecated("Use fxt::mtuple_as_array")]]
    constexpr auto mas_array()
    { return mtuple_as_array<T>(); }

}    // namespace fxt
