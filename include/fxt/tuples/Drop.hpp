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
 * @file Drop.hpp
 * @brief Tuple element removal operations for discarding subsets from tuples
 *
 * This file provides operations for removing a specified number of elements from the beginning
 * or end of tuple-like types. All operations support both fxt::tuple and fxt::flat_tuple, preserving
 * the input tuple type. Each operation has both direct-call and curried (pipeline-compatible) versions.
 *
 * ## Core Operations
 *
 * **drop<X>(tuple)** - Removes the first X elements from a tuple
 * - Direct call: `fxt::drop<2>(tpl)` returns new tuple without first 2 elements
 * - Curried: `tpl | fxt::drop<2>()` enables pipeline usage
 *
 * **drop_last<X>(tuple)** - Removes the last X elements from a tuple
 * - Direct call: `fxt::drop_last<2>(tpl)` returns new tuple without last 2 elements
 * - Curried: `tpl | fxt::drop_last<2>()` enables pipeline usage
 *
 * ## Monadic Operations
 *
 * **mdrop<X>(container)** - Applies drop to a tuple inside fxt::expected or fxt::optional
 * - Automatically propagates errors/nullopt through the operation
 * - Curried version: `container | fxt::mdrop<2>()`
 *
 * **mdrop_last<X>(container)** - Applies drop_last to a tuple inside a monadic container
 * - Automatically propagates errors/nullopt through the operation
 * - Curried version: `container | fxt::mdrop_last<2>()`
 *
 * All operations use perfect forwarding and are constexpr-compatible for compile-time evaluation.
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
     * @brief Drops the first X elements from a tuple inside a monad (direct call)
     *
     * Applies the drop operation to a tuple contained within an fxt::expected or fxt::optional object.
     * Returns a monad of the same type containing the tuple with the first X elements removed.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @tparam Container The monad type containing a tuple (deduced)
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the tuple without first X elements
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = fxt::mdrop<2>(exp);
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {3, 4}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = fxt::mdrop<1>(opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mdrop(Container&& container)
    {
        return std::forward<Container>(container).transform([](auto&& tpl) {
            constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<decltype(tpl)>>;
            static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_tuple(fxt::get<Indices + X>(std::forward<decltype(tpl)>(tpl))...);
                } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_flat_tuple(fxt::get<Indices + X>(std::forward<decltype(tpl)>(tpl))...);
                }
            }(std::make_index_sequence<tupleSize - X>{});
        });
    }

    /**
     * @brief Drops the first X elements from a tuple inside a monad (curried version for pipeline)
     *
     * Returns a lambda that drops the first X elements from a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_without_first_X_elements>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = exp | fxt::mdrop<2>();
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {3, 4}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = opt | fxt::mdrop<1>();
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result3 = exp_err | fxt::mdrop<1>();
     * // result3 contains the error
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4, 5)};
     * auto result4 = exp2
     *     | fxt::mdrop<2>()
     *     | fxt::mtransform_tuple([](auto x) { return x * 2; });
     * // result4 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {6, 8, 10}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mdrop()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mdrop<X>(std::forward<Container>(container));
        };
    }

    /**
     * @brief Drops the last X elements from a tuple inside a monad (direct call)
     *
     * Applies the drop_last operation to a tuple contained within an fxt::expected or fxt::optional object.
     * Returns a monad of the same type containing the tuple with the last X elements removed.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @tparam Container The monad type containing a tuple (deduced)
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the tuple without last X elements
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = fxt::mdrop_last<2>(exp);
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {1, 2}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = fxt::mdrop_last<1>(opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {1.0, 2.0}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mdrop_last(Container&& container)
    {
        return std::forward<Container>(container).transform([](auto&& tpl) {
            constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<decltype(tpl)>>;
            static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_tuple(fxt::get<Indices>(std::forward<decltype(tpl)>(tpl))...);
                } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_flat_tuple(fxt::get<Indices>(std::forward<decltype(tpl)>(tpl))...);
                }
            }(std::make_index_sequence<tupleSize - X>{});
        });
    }

    /**
     * @brief Drops the last X elements from a tuple inside a monad (curried version for pipeline)
     *
     * Returns a lambda that drops the last X elements from a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_without_last_X_elements>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = exp | fxt::mdrop_last<2>();
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {1, 2}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = opt | fxt::mdrop_last<1>();
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {1.0, 2.0}
     *
     * // Nullopt propagation
     * auto opt_null = fxt::optional<fxt::tuple<int, int, int>>{std::nullopt};
     * auto result3 = opt_null | fxt::mdrop_last<1>();
     * // result3 is nullopt
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4, 5)};
     * auto result4 = exp2
     *     | fxt::mdrop_last<2>()
     *     | fxt::mtuple_reverse();
     * // result4 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 2, 1}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mdrop_last()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mdrop_last<X>(std::forward<Container>(container));
        };
    }

}    // namespace fxt