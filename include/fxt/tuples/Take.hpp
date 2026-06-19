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
 * @file Take.hpp
 * @brief Tuple element extraction operations for selecting subsets from tuples
 *
 * This file provides operations for extracting a specified number of elements from the beginning
 * or end of tuple-like types. All operations support both fxt::tuple and fxt::flat_tuple, preserving
 * the input tuple type. Each operation has both direct-call and curried (pipeline-compatible) versions.
 *
 * ## Core Operations
 *
 * **take<X>(tuple)** - Extracts the first X elements from a tuple
 * - Direct call: `fxt::take<2>(tpl)` returns new tuple with first 2 elements
 * - Curried: `tpl | fxt::take<2>()` enables pipeline usage
 *
 * **take_last<X>(tuple)** - Extracts the last X elements from a tuple
 * - Direct call: `fxt::take_last<2>(tpl)` returns new tuple with last 2 elements
 * - Curried: `tpl | fxt::take_last<2>()` enables pipeline usage
 *
 * ## Monadic Operations
 *
 * **mtake<X>(container)** - Applies take to a tuple inside fxt::expected or fxt::optional
 * - Automatically propagates errors/nullopt through the operation
 * - Curried version: `container | fxt::mtake<2>()`
 *
 * **mtake_last<X>(container)** - Applies take_last to a tuple inside a monadic container
 * - Automatically propagates errors/nullopt through the operation
 * - Curried version: `container | fxt::mtake_last<2>()`
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
     * @brief Takes the first X elements from a tuple or flat_tuple (direct call)
     *
     * This function takes the first X elements from the input tuple and returns a new tuple
     * of the same type. Perfect forwarding is preserved for the tuple elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing the first X elements
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::take<2>(t);
     * // result is fxt::tuple<int, double>{1, 2.0}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::take<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{1.0, 2.0}
     * @endcode
     */
    // TODO: NAMING — per the fxt/tuples convention in Tuple.hpp, rename the
    //       fxt-original ops to carry the `tuple_` prefix: take -> tuple_take,
    //       take_last -> tuple_take_last, mtake -> mtuple_take, mtake_last ->
    //       mtuple_take_last (keep old names as [[deprecated]] aliases for one release).
    template<std::size_t X, typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto take(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                return fxt::make_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                return fxt::make_flat_tuple(fxt::get<Indices>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<X>{});
    }

    /**
     * @brief Takes the first X elements from a tuple or flat_tuple (curried version for pipeline)
     *
     * Returns a lambda that takes the first X elements from a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @return A lambda that takes a tuple and returns a new tuple with the first X elements
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4);
     * auto result = t | fxt::take<2>();
     * // result is fxt::tuple<int, int>{1, 2}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = ft | fxt::take<2>();
     * // result2 is fxt::flat_tuple<double, double>{1.0, 2.0}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto take()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::take<X>(std::forward<Tuple>(tpl));
        };
    }

    /**
     * @brief Takes the last X elements from a tuple or flat_tuple (direct call)
     *
     * This function takes the last X elements from the input tuple and returns a new tuple
     * of the same type. Perfect forwarding is preserved for the tuple elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to take from the end of the tuple
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing the last X elements
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::take_last<2>(t);
     * // result is fxt::tuple<const char*, char>{"three", 'f'}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::take_last<2>(ft);
     * // result2 is fxt::flat_tuple<double, double>{3.0, 4.0}
     * @endcode
     */
    template<std::size_t X, typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto take_last(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<Tuple>) {
                return fxt::make_tuple(fxt::get<Indices + (tupleSize - X)>(std::forward<Tuple>(tpl))...);
            } else if constexpr (impl::is_flat_tuple_v<Tuple>) {
                return fxt::make_flat_tuple(fxt::get<Indices + (tupleSize - X)>(std::forward<Tuple>(tpl))...);
            }
        }(std::make_index_sequence<X>{});
    }

    /**
     * @brief Takes the last X elements from a tuple or flat_tuple (curried version for pipeline)
     *
     * Returns a lambda that takes the last X elements from a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam X The number of elements to take from the end of the tuple
     * @return A lambda that takes a tuple and returns a new tuple with the last X elements
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4);
     * auto result = t | fxt::take_last<2>();
     * // result is fxt::tuple<int, int>{3, 4}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = ft | fxt::take_last<2>();
     * // result2 is fxt::flat_tuple<double, double>{3.0, 4.0}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto take_last()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::take_last<X>(std::forward<Tuple>(tpl));
        };
    }

    /**
     * @brief Takes the first X elements from a tuple inside a monad (direct call)
     *
     * Applies the take operation to a tuple contained within an fxt::expected or fxt::optional object.
     * Returns a monad of the same type containing the tuple with only the first X elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @tparam Container The monad type containing a tuple (deduced)
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the tuple with first X elements
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = fxt::mtake<2>(exp);
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {1, 2}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = fxt::mtake<2>(opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {1.0, 2.0}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mtake(Container&& container)
    {
        return std::forward<Container>(container).transform([](auto&& tpl) {
            constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<decltype(tpl)>>;
            static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_tuple(fxt::get<Indices>(std::forward<decltype(tpl)>(tpl))...);
                } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_flat_tuple(fxt::get<Indices>(std::forward<decltype(tpl)>(tpl))...);
                }
            }(std::make_index_sequence<X>{});
        });
    }

    /**
     * @brief Takes the first X elements from a tuple inside a monad (curried version for pipeline)
     *
     * Returns a lambda that takes the first X elements from a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_with_first_X_elements>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = exp | fxt::mtake<2>();
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {1, 2}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = opt | fxt::mtake<2>();
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {1.0, 2.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result3 = exp_err | fxt::mtake<2>();
     * // result3 contains the error
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4, 5)};
     * auto result4 = exp2
     *     | fxt::mtransform_tuple([](auto x) { return x * 2; })
     *     | fxt::mtake<3>();
     * // result4 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mtake()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mtake<X>(std::forward<Container>(container));
        };
    }

    /**
     * @brief Takes the last X elements from a tuple inside a monad (direct call)
     *
     * Applies the take_last operation to a tuple contained within an fxt::expected or fxt::optional object.
     * Returns a monad of the same type containing the tuple with only the last X elements.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam X The number of elements to take from the end of the tuple
     * @tparam Container The monad type containing a tuple (deduced)
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the tuple with last X elements
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = fxt::mtake_last<2>(exp);
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {3, 4}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = fxt::mtake_last<2>(opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mtake_last(Container&& container)
    {
        return std::forward<Container>(container).transform([](auto&& tpl) {
            constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<decltype(tpl)>>;
            static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

            return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
                if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_tuple(fxt::get<Indices + (tupleSize - X)>(std::forward<decltype(tpl)>(tpl))...);
                } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<decltype(tpl)>>) {
                    return fxt::make_flat_tuple(fxt::get<Indices + (tupleSize - X)>(std::forward<decltype(tpl)>(tpl))...);
                }
            }(std::make_index_sequence<X>{});
        });
    }

    /**
     * @brief Takes the last X elements from a tuple inside a monad (curried version for pipeline)
     *
     * Returns a lambda that takes the last X elements from a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam X The number of elements to take from the end of the tuple
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_with_last_X_elements>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4)};
     * auto result = exp | fxt::mtake_last<2>();
     * // result is fxt::expected<fxt::tuple<int, int>, Error> containing {3, 4}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double, double>>{fxt::make_flat_tuple(1.0, 2.0, 3.0)};
     * auto result2 = opt | fxt::mtake_last<2>();
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // Nullopt propagation
     * auto opt_null = fxt::optional<fxt::tuple<int, int, int>>{std::nullopt};
     * auto result3 = opt_null | fxt::mtake_last<2>();
     * // result3 is nullopt
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, Error>{fxt::make_tuple(1, 2, 3, 4, 5)};
     * auto result4 = exp2
     *     | fxt::mtuple_reverse()
     *     | fxt::mtake_last<3>();
     * // result4 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 2, 1}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mtake_last()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mtake_last<X>(std::forward<Container>(container));
        };
    }

}    // namespace fxt
