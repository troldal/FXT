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

#include "../concepts/IsTuple.hpp"
#include "../concepts/IsMonad.hpp"
#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include <array>
#include <utility>

namespace fxt
{
    /**
     * @brief Implementation details for as_array
     */
    namespace impl
    {
        template<typename T, typename... Ts, std::size_t... Is>
        constexpr std::array<T, sizeof...(Ts)> as_array_impl(const tuple<Ts...>& t, std::index_sequence<Is...>)
        {
            return {static_cast<T>(std::get<Is>(t))...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr std::array<T, sizeof...(Ts)> as_array_impl(tuple<Ts...>&& t, std::index_sequence<Is...>)
        {
            return {static_cast<T>(std::get<Is>(std::move(t)))...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr std::array<T, sizeof...(Ts)> as_array_impl(const flat_tuple<Ts...>& t, std::index_sequence<Is...>)
        {
            return {static_cast<T>(fxt::get<Is>(t))...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr std::array<T, sizeof...(Ts)> as_array_impl(flat_tuple<Ts...>&& t, std::index_sequence<Is...>)
        {
            return {static_cast<T>(fxt::get<Is>(std::move(t)))...};
        }
    }

    /**
     * @brief Convert a tuple-like object to a std::array
     *
     * Takes a tuple-like object (either fxt::tuple or fxt::flat_tuple) and converts
     * it to a std::array where all elements are converted to the specified type T.
     *
     * @tparam T The element type of the resulting array (must be constructible from all tuple elements)
     * @tparam TTuple The tuple type (deduced)
     * @param t The tuple to convert
     * @return std::array<T, N> where N is the number of elements in the tuple
     *
     * @section Usage
     * @code
     * auto tpl = fxt::make_tuple(1, 2.5, 3);
     * auto arr = fxt::as_array<double>(tpl);  // std::array<double, 3>{1.0, 2.5, 3.0}
     * @endcode
     */
    template<typename T, typename... Ts>
    constexpr std::array<T, sizeof...(Ts)> as_array(const tuple<Ts...>& t)
    requires (std::constructible_from<T, const Ts&> && ...)
    {
        return impl::as_array_impl<T>(t, std::index_sequence_for<Ts...>{});
    }

    template<typename T, typename... Ts>
    constexpr std::array<T, sizeof...(Ts)> as_array(tuple<Ts...>&& t)
    requires (std::constructible_from<T, Ts> && ...)
    {
        return impl::as_array_impl<T>(std::move(t), std::index_sequence_for<Ts...>{});
    }

    template<typename T, typename... Ts>
    constexpr std::array<T, sizeof...(Ts)> as_array(const flat_tuple<Ts...>& t)
    requires (std::constructible_from<T, const Ts&> && ...)
    {
        return impl::as_array_impl<T>(t, std::index_sequence_for<Ts...>{});
    }

    template<typename T, typename... Ts>
    constexpr std::array<T, sizeof...(Ts)> as_array(flat_tuple<Ts...>&& t)
    requires (std::constructible_from<T, Ts> && ...)
    {
        return impl::as_array_impl<T>(std::move(t), std::index_sequence_for<Ts...>{});
    }

    // TODO: NAMING — per the fxt/tuples convention in Tuple.hpp, rename the
    //       fxt-original ops to carry the `tuple_` prefix: as_array -> tuple_as_array
    //       and mas_array -> mtuple_as_array (keep old names as [[deprecated]]
    //       aliases for one release).
    /**
     * @brief Curried version of as_array
     *
     * Returns a lambda that converts a tuple-like object to a std::array.
     * This is useful for piping operations.
     *
     * @tparam T The element type of the resulting array
     * @return A lambda that takes a tuple and returns std::array<T, N>
     *
     * @section Usage
     * @code
     * auto tpl = fxt::make_tuple(1, 2.5, 3);
     * auto arr = tpl | fxt::as_array<double>();  // std::array<double, 3>{1.0, 2.5, 3.0}
     * @endcode
     */
    template<typename T>
    constexpr auto as_array()
    {
        return []<typename TTuple>(TTuple&& t)
        requires tuple_like<TTuple>
        {
            return as_array<T>(std::forward<TTuple>(t));
        };
    }

    // ========================================================================
    // Monadic as_array - mas_array
    // ========================================================================

    namespace impl
    {
        // Helper type to get the result monad type when transforming a monadic element
        template<typename T, typename MonadT>
        struct transformed_monad_type;

        template<typename T, typename MonadT>
        requires monad_like<MonadT>
        struct transformed_monad_type<T, MonadT>
        {
            using type = decltype(std::declval<MonadT>().transform([](auto&& val) { return static_cast<T>(val); }));
        };

        template<typename T, typename MonadT>
        using transformed_monad_type_t = typename transformed_monad_type<T, MonadT>::type;

        // Helper to transform monadic elements in tuple to array
        template<typename T, typename... Ts, std::size_t... Is>
        constexpr auto mas_array_impl_monadic_elements(const tuple<Ts...>& t, std::index_sequence<Is...>)
        requires (sizeof...(Ts) > 0) && (monad_like<Ts> && ...)
        {
            using result_monad_type = transformed_monad_type_t<T, std::tuple_element_t<0, tuple<Ts...>>>;
            return std::array<result_monad_type, sizeof...(Ts)>{std::get<Is>(t).transform([](auto&& val) { return static_cast<T>(val); })...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr auto mas_array_impl_monadic_elements(tuple<Ts...>&& t, std::index_sequence<Is...>)
        requires (sizeof...(Ts) > 0) && (monad_like<Ts> && ...)
        {
            using result_monad_type = transformed_monad_type_t<T, std::tuple_element_t<0, tuple<Ts...>>>;
            return std::array<result_monad_type, sizeof...(Ts)>{std::get<Is>(std::move(t)).transform([](auto&& val) { return static_cast<T>(val); })...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr auto mas_array_impl_monadic_elements(const flat_tuple<Ts...>& t, std::index_sequence<Is...>)
        requires (sizeof...(Ts) > 0) && (monad_like<Ts> && ...)
        {
            using result_monad_type = transformed_monad_type_t<T, std::tuple_element_t<0, flat_tuple<Ts...>>>;
            return std::array<result_monad_type, sizeof...(Ts)>{fxt::get<Is>(t).transform([](auto&& val) { return static_cast<T>(val); })...};
        }

        template<typename T, typename... Ts, std::size_t... Is>
        constexpr auto mas_array_impl_monadic_elements(flat_tuple<Ts...>&& t, std::index_sequence<Is...>)
        requires (sizeof...(Ts) > 0) && (monad_like<Ts> && ...)
        {
            using result_monad_type = transformed_monad_type_t<T, std::tuple_element_t<0, flat_tuple<Ts...>>>;
            return std::array<result_monad_type, sizeof...(Ts)>{fxt::get<Is>(std::move(t)).transform([](auto&& val) { return static_cast<T>(val); })...};
        }
    }

    namespace impl
    {
        // Helper to check if all types in a tuple are monadic (returns false for empty tuples)
        template<typename... Ts>
        constexpr bool all_monadic_v = (sizeof...(Ts) > 0) && (monad_like<Ts> && ...);

        // Specialization for tuple
        template<typename T>
        struct tuple_elements_are_monadic : std::false_type {};

        template<typename... Ts>
        struct tuple_elements_are_monadic<tuple<Ts...>> : std::bool_constant<all_monadic_v<Ts...>> {};

        template<typename... Ts>
        struct tuple_elements_are_monadic<flat_tuple<Ts...>> : std::bool_constant<all_monadic_v<Ts...>> {};
    }

    /**
     * @brief Monadic version of as_array (mas_array)
     *
     * This function works in monadic contexts (optional/expected containing tuples).
     * It has two modes of operation:
     * 1. If the tuple elements are NOT monadic: converts tuple to std::array<T, N>
     * 2. If the tuple elements ARE monadic: converts to std::array<Monad<T>, N> where each monad is transformed
     *
     * @tparam T The target element type (for non-monadic elements) or the target value type (for monadic elements)
     * @return A lambda that works with monadic containers containing tuples
     *
     * @section Usage
     * @code
     * // Case 1: Optional containing a tuple of non-monadic values
     * std::optional<fxt::tuple<int, double, int>> opt = fxt::make_tuple(1, 2.5, 3);
     * auto result1 = opt | fxt::mas_array<double>();
     * // result1: std::optional<std::array<double, 3>>
     *
     * // Case 2: Expected containing a tuple of monadic values
     * using Exp = fxt::expected<int, std::string>;
     * fxt::expected<fxt::tuple<Exp, Exp, Exp>, std::string> exp = ...;
     * auto result2 = exp | fxt::mas_array<double>();
     * // result2: fxt::expected<std::array<fxt::expected<double, std::string>, 3>, std::string>
     * @endcode
     */
    template<typename T>
    constexpr auto mas_array()
    {
        return []<typename TMonad>(TMonad&& monad)
        {
            return std::forward<TMonad>(monad).transform([](auto&& tuple_val) {
                using tuple_type = std::remove_cvref_t<decltype(tuple_val)>;

                if constexpr (tuple_like<tuple_type>) {
                    // Check if tuple contains monadic elements
                    if constexpr (impl::tuple_elements_are_monadic<tuple_type>::value) {
                        // Tuple contains monadic elements - transform each
                        if constexpr (impl::is_fxt_tuple_v<tuple_type>) {
                            return impl::mas_array_impl_monadic_elements<T>(
                                std::forward<decltype(tuple_val)>(tuple_val),
                                std::make_index_sequence<std::tuple_size_v<tuple_type>>{}
                            );
                        } else {
                            return impl::mas_array_impl_monadic_elements<T>(
                                std::forward<decltype(tuple_val)>(tuple_val),
                                std::make_index_sequence<tuple_type::size()>{}
                            );
                        }
                    } else {
                        // Tuple contains non-monadic elements - convert directly
                        return as_array<T>(std::forward<decltype(tuple_val)>(tuple_val));
                    }
                } else {
                    // Not a tuple, just forward
                    static_assert(tuple_like<tuple_type>, "mas_array requires a tuple-like type inside the monad");
                }
            });
        };
    }

} // namespace fxt

