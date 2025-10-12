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

#include "TupleSize.hpp"
#include "../concepts/IsTuple.hpp"

namespace fxt::impl
{
    // Generic append implementation that works for both tuple types - single value
    template<class Tuple, class U, std::size_t... I>
    auto append_impl(Tuple&& t, U&& u, std::index_sequence<I...>)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>..., std::decay_t<U>>;
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<U>(u));
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) -> flat_tuple<std::decay_t<Ts>..., std::decay_t<U>> {
                return std::declval<flat_tuple<std::decay_t<Ts>..., std::decay_t<U>>>();
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<U>(u));
        }
    }

    // Generic append implementation that works for both tuple types - multiple values
    template<class Tuple, class... Us, std::size_t... I>
    auto append_impl_variadic(Tuple&& t, std::index_sequence<I...>, Us&&... us)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>..., std::decay_t<Us>...>;
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<Us>(us)...);
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) -> flat_tuple<std::decay_t<Ts>..., std::decay_t<Us>...> {
                return std::declval<flat_tuple<std::decay_t<Ts>..., std::decay_t<Us>...>>();
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<Us>(us)...);
        }
    }

}    // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Append one or more elements to a tuple
     *
     * Generic overload that works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Uses fxt::tuple_size_v to determine the tuple size, which works for both types.
     * Accepts at least one element to append, but can append multiple elements at once.
     *
     * @tparam Tuple The tuple type (fxt::tuple or fxt::flat_tuple)
     * @tparam U The type of the first element to append
     * @tparam Us The types of additional elements to append
     * @param t The tuple to append to
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A new tuple with all elements appended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2};
     * auto result = fxt::tuple_append(t, 3); // fxt::tuple{1, 2, 3}
     * auto result2 = fxt::tuple_append(t, 3, 4, 5); // fxt::tuple{1, 2, 3, 4, 5}
     * @endcode
     */
    template<class Tuple, class U, class... Us>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    auto tuple_append(Tuple&& t, U&& u, Us&&... us)
    {
        constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        if constexpr (sizeof...(Us) == 0) {
            // Single element append
            return impl::append_impl(std::forward<Tuple>(t), std::forward<U>(u), std::make_index_sequence<N>{});
        } else {
            // Multiple elements append
            return impl::append_impl_variadic(std::forward<Tuple>(t), std::make_index_sequence<N>{}, std::forward<U>(u), std::forward<Us>(us)...);
        }
    }

    /**
     * @brief Append one or more elements to a tuple (curried version for pipeline usage)
     *
     * Returns a lambda that appends the given elements to a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Accepts at least one element to append, but can append multiple elements at once.
     *
     * @tparam U The type of the first element to append
     * @tparam Us The types of additional elements to append
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A lambda that takes a tuple and returns a new tuple with all elements appended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2, 3};
     * auto result = t | fxt::tuple_append(4); // fxt::tuple{1, 2, 3, 4}
     * auto result2 = t | fxt::tuple_append(4, 5, 6); // fxt::tuple{1, 2, 3, 4, 5, 6}
     *
     * auto ft = fxt::flat_tuple<double, double>{1.0, 2.0};
     * auto result3 = ft | fxt::tuple_append(3.0, 4.0); // fxt::flat_tuple{1.0, 2.0, 3.0, 4.0}
     * @endcode
     */
    template<class U, class... Us>
        requires (!impl::is_tuple_like_v<std::remove_cvref_t<U>>)
    auto tuple_append(U&& u, Us&&... us)
    {
        if constexpr (sizeof...(Us) == 0) {
            // Single element version
            return [value = std::forward<U>(u)]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                return impl::append_impl(std::forward<Tuple>(t), std::forward<U>(value), std::make_index_sequence<N>{});
            };
        } else {
            // Multiple elements version - capture values in a tuple and unpack them
            return [values = fxt::tuple<U, Us...>{std::forward<U>(u), std::forward<Us>(us)...}]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                constexpr std::size_t M = sizeof...(Us) + 1;

                // Unpack the captured tuple and pass to append_impl_variadic
                return [&t, &values, N]<std::size_t... I>(std::index_sequence<I...>) {
                    return impl::append_impl_variadic(
                        std::forward<Tuple>(t),
                        std::make_index_sequence<N>{},
                        std::move(fxt::get<I>(values))...
                    );
                }(std::make_index_sequence<M>{});
            };
        }
    }

}    // namespace fxt
