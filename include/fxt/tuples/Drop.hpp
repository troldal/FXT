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

#include <tuple>
#include <utility>
#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include "TupleSize.hpp"
#include "TupleAppend.hpp"

namespace fxt
{
    /**
     * @brief Drops the first X elements from a tuple or flat_tuple (direct call)
     *
     * This function drops the first X elements from the input tuple and returns a new tuple
     * of the same type. Perfect forwarding is preserved for the tuple elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing all but the first X elements
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::drop<2>(t);
     * // result is fxt::tuple<const char*, char>{"three", 'f'}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::drop<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{3.0, 4.0}
     * @endcode
     */
    template<std::size_t X, typename Tuple>
        requires tuple_like<std::remove_cvref_t<Tuple>>
    constexpr auto drop(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                return fxt::make_tuple(fxt::get<Indices + X>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                return fxt::make_flat_tuple(fxt::get<Indices + X>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<tupleSize - X>{});
    }

    /**
     * @brief Drops the first X elements from a tuple or flat_tuple (curried version for pipeline)
     *
     * Returns a lambda that drops the first X elements from a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @return A lambda that takes a tuple and returns a new tuple with the first X elements dropped
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4);
     * auto result = t | fxt::drop<2>();
     * // result is fxt::tuple<int, int>{3, 4}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = ft | fxt::drop<2>();
     * // result2 is fxt::flat_tuple<double, double>{3.0, 4.0}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto drop()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::drop<X>(std::forward<Tuple>(tpl));
        };
    }

    /**
     * @brief Drops the last X elements from a tuple or flat_tuple (direct call)
     *
     * This function drops the last X elements from the input tuple and returns a new tuple
     * of the same type. Perfect forwarding is preserved for the tuple elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing all but the last X elements
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::drop_last<2>(t);
     * // result is fxt::tuple<int, double>{1, 2.0}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::drop_last<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{1.0, 2.0}
     * @endcode
     */
    template<std::size_t X, typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto drop_last(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                return fxt::make_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                return fxt::make_flat_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<tupleSize - X>{});
    }

    /**
     * @brief Drops the last X elements from a tuple or flat_tuple (curried version for pipeline)
     *
     * Returns a lambda that drops the last X elements from a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @return A lambda that takes a tuple and returns a new tuple with the last X elements dropped
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4);
     * auto result = t | fxt::drop_last<2>();
     * // result is fxt::tuple<int, int>{1, 2}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = ft | fxt::drop_last<2>();
     * // result2 is fxt::flat_tuple<double, double>{1.0, 2.0}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto drop_last()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::drop_last<X>(std::forward<Tuple>(tpl));
        };
    }

    /**
     * @brief Drops the first X elements from a tuple or flat_tuple (for monadic use)
     *
     * This function object creates a new tuple that contains all elements of the input tuple
     * except for the first X elements. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing all but the first X elements
     *
     * @section Usage
     * @code
     * // With fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::mdrop<2>(t);
     * // result is fxt::tuple<const char*, char>{"three", 'f'}
     *
     * // With fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::mdrop<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{3.0, 4.0}
     *
     * // In a pipeline
     * auto result3 = fxt::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::mdrop<2>(std::move(t)); };
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mdrop = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                // Return fxt::tuple
                return fxt::make_tuple(fxt::get<Indices + X>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                // Return fxt::flat_tuple
                return fxt::make_flat_tuple(fxt::get<Indices + X>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<tupleSize - X>{});
    };

    /**
     * @brief Drops the last X elements from a tuple or flat_tuple (for monadic use)
     *
     * This function object creates a new tuple that contains all elements of the input tuple
     * except for the last X elements. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing all but the last X elements
     *
     * @section Usage
     * @code
     * // With fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::mdrop_last<2>(t);
     * // result is fxt::tuple<int, double>{1, 2.0}
     *
     * // With fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::mdrop_last<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{1.0, 2.0}
     *
     * // In a pipeline
     * auto result3 = fxt::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::mdrop_last<2>(std::move(t)); };
     * // result3 is fxt::tuple<int, int>{1, 2}
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mdrop_last = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                // Return fxt::tuple
                return fxt::make_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                // Return fxt::flat_tuple
                return fxt::make_flat_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<tupleSize - X>{});
    };
}    // namespace fxt