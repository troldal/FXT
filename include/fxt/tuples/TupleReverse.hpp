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
 * @file TupleReverse.hpp
 * @brief Tuple reverse operations for fxt::tuple and fxt::flat_tuple
 *
 * This file provides functions for reversing the order of elements in tuples in both plain and monadic contexts.
 * All functions work seamlessly with both fxt::tuple (std::tuple) and fxt::flat_tuple, preserving
 * the tuple type through the operations.
 *
 * ## Main Functions
 *
 * ### fxt::tuple_reverse
 * Reverses the order of elements in a tuple. Available in two forms:
 * - **Direct call**: `tuple_reverse(tuple)` - Takes a tuple and returns it with reversed element order
 * - **Curried form**: `tuple_reverse()` - Returns a lambda for pipeline usage with operator|
 *
 * ### fxt::mtuple_reverse
 * Reverses the order of elements in a tuple inside a monad (fxt::expected or fxt::optional).
 * This is a lambda object that can be used directly with monads via the transform method or pipe operator.
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple, maintaining the tuple type
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_reverse handles error/nullopt propagation automatically
 * - Perfect forwarding: Preserves value categories of tuple elements
 *
 * ## Examples
 *
 * ### Basic tuple reverse
 * @code
 * auto t = fxt::tuple{1, 2, 3};
 * auto result = fxt::tuple_reverse(t); // fxt::tuple{3, 2, 1}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{1, 2, 3, 4}
 *     | fxt::tuple_reverse();  // fxt::tuple{4, 3, 2, 1}
 * @endcode
 *
 * ### Monadic reverse
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::tuple{1, 2, 3}};
 * auto result = exp | fxt::mtuple_reverse;
 * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 2, 1}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double, double>{1.0, 2.0, 3.0};
 * auto result = ft | fxt::tuple_reverse();
 * // result is fxt::flat_tuple<double, double, double>{3.0, 2.0, 1.0}
 * @endcode
 *
 * ### Chaining operations
 * @code
 * auto t = fxt::tuple{1, 2, 3, 4, 5};
 * auto result = t
 *     | fxt::take<3>()
 *     | fxt::tuple_reverse();
 * // result is fxt::tuple<int, int, int>{3, 2, 1}
 * @endcode
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::expected
 * @see fxt::optional
 */

#pragma once

#include "Get.hpp"
#include "FlatTuple.hpp"
#include "../concepts/IsTuple.hpp"
#include <utility>

namespace fxt
{
    /**
     * @brief Reverse the order of elements in a tuple (direct call)
     *
     * Takes a tuple and returns a new tuple of the same type with elements in reversed order.
     * Perfect forwarding is preserved for the tuple elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type with elements in reversed order
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three");
     * auto result = fxt::tuple_reverse(t);
     * // result is fxt::tuple<const char*, double, int>{"three", 2.0, 1}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = fxt::tuple_reverse(ft);
     * // result2 is fxt::flat_tuple<double, double, double>{3.0, 2.0, 1.0}
     *
     * // With rvalue reference
     * auto result3 = fxt::tuple_reverse(fxt::make_tuple(1, 2, 3));
     * // result3 is fxt::tuple<int, int, int>{3, 2, 1}
     * @endcode
     */
    template<typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto tuple_reverse(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>>) {
                return fxt::make_tuple(fxt::get<tupleSize - 1 - Indices>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<Tuple>>) {
                return fxt::make_flat_tuple(fxt::get<tupleSize - 1 - Indices>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<tupleSize>{});
    }

    /**
     * @brief Reverse the order of elements in a tuple (curried version for pipeline)
     *
     * Returns a lambda that reverses the order of elements in a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @return A lambda that takes a tuple and returns a new tuple with elements in reversed order
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4);
     * auto result = t | fxt::tuple_reverse();
     * // result is fxt::tuple<int, int, int, int>{4, 3, 2, 1}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = ft | fxt::tuple_reverse();
     * // result2 is fxt::flat_tuple<double, double, double>{3.0, 2.0, 1.0}
     *
     * // Chaining with other operations
     * auto t2 = fxt::make_tuple(1, 2, 3, 4, 5);
     * auto result3 = t2
     *     | fxt::take<3>()
     *     | fxt::tuple_reverse();
     * // result3 is fxt::tuple<int, int, int>{3, 2, 1}
     * @endcode
     */
    constexpr auto tuple_reverse()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::tuple_reverse(std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::mtuple_reverse - Monadic tuple reverse
    // ========================================================================

    /**
     * @brief Reverse the order of elements in a tuple inside a monad (direct call)
     *
     * Reverses the order of elements in a tuple contained within an fxt::expected
     * or fxt::optional object. Returns a monad of the same type containing the reversed tuple.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container The monad type containing a tuple (deduced)
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the reversed tuple
     *
     * @section Usage
     * @code
     * // With fxt::expected and fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::tuple{1, 2, 3}};
     * auto result = fxt::mtuple_reverse(exp);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 2, 1}
     *
     * // With fxt::optional and fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::flat_tuple{1.0, 2.0, 3.0}};
     * auto result2 = fxt::mtuple_reverse(opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double, double>> containing {3.0, 2.0, 1.0}
     *
     * // Error propagation with fxt::expected
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result3 = fxt::mtuple_reverse(exp_err);
     * // result3 contains the error
     * @endcode
     */
    template<typename Container>
    constexpr auto mtuple_reverse(Container&& container)
    {
        return std::forward<Container>(container).transform([]<typename TValue>(TValue&& tpl) {
            constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<TValue>>;

            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<TValue>>) {
                    return fxt::make_tuple(fxt::get<tupleSize - 1 - Indices>(std::forward<TValue>(tpl))...);
                } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<TValue>>) {
                    return fxt::make_flat_tuple(fxt::get<tupleSize - 1 - Indices>(std::forward<TValue>(tpl))...);
                }
            }(std::make_index_sequence<tupleSize>{});
        });
    }

    /**
     * @brief Reverse the order of elements in a tuple inside a monad (curried version for pipeline)
     *
     * Returns a lambda that reverses the order of elements in a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @return A lambda that takes a monad<tuple> and returns a monad<reversed_tuple>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected and fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::tuple{1, 2, 3}};
     * auto result = exp | fxt::mtuple_reverse();
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 2, 1}
     *
     * // Pipe operator with fxt::optional and fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::flat_tuple{1.0, 2.0, 3.0}};
     * auto result2 = opt | fxt::mtuple_reverse();
     * // result2 is fxt::optional<fxt::flat_tuple<double, double, double>> containing {3.0, 2.0, 1.0}
     *
     * // Nullopt propagation with fxt::optional
     * auto opt_null = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
     * auto result4 = opt_null | fxt::mtuple_reverse();
     * // result4 is std::nullopt
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::tuple{1, 2, 3}};
     * auto result5 = exp2
     *     | fxt::mtuple_transform([](auto x) { return x * 2; })
     *     | fxt::mtuple_reverse();
     * // result5 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {6, 4, 2}
     * @endcode
     */
    constexpr auto mtuple_reverse()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mtuple_reverse(std::forward<Container>(container));
        };
    }

}    // namespace fxt
