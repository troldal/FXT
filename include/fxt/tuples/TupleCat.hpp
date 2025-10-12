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
 * @file TupleCat.hpp
 * @brief Tuple concatenation operations for fxt::tuple and fxt::flat_tuple
 *
 * This file provides functions for concatenating tuples in both plain and monadic contexts.
 * All functions work seamlessly with both fxt::tuple (std::tuple) and fxt::flat_tuple,
 * preserving the tuple type through the operations.
 *
 * ## Main Functions
 *
 * ### fxt::tuple_cat
 * Concatenates two tuples into a single tuple containing all elements from both.
 * Available in two forms:
 * - **Direct call**: `tuple_cat(tuple1, tuple2)` - Takes two tuples and concatenates them
 * - **Curried form**: `tuple_cat(tuple2)` - Returns a lambda for pipeline usage with operator|
 *
 * ### fxt::mtuple_cat
 * Concatenates tuples inside monads (fxt::expected or fxt::optional). Available in multiple forms:
 * - **Monad with plain tuple**: `mtuple_cat(monad<tuple1>, tuple2)` - Concatenates with plain tuple
 * - **Two monadic tuples**: `mtuple_cat(monad<tuple1>, monad<tuple2>)` - Combines monadic tuples
 * - **Curried form**: `mtuple_cat(tuple2)` - Returns a lambda for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple, maintaining the tuple type
 * - Move-aware: Provides overloads for all combinations of lvalue/rvalue references
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_cat handles error/nullopt propagation automatically
 * - Efficient: Uses perfect forwarding and move semantics to avoid unnecessary copies
 *
 * ## Examples
 *
 * ### Basic tuple concatenation
 * @code
 * auto t1 = fxt::tuple{1, 2};
 * auto t2 = fxt::tuple{3.0, 4.0};
 * auto result = fxt::tuple_cat(t1, t2);  // fxt::tuple{1, 2, 3.0, 4.0}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{1, 2}
 *     | fxt::tuple_cat(fxt::tuple{3.0})
 *     | fxt::tuple_cat(fxt::tuple{true});  // fxt::tuple{1, 2, 3.0, true}
 * @endcode
 *
 * ### Monadic concatenation with plain tuple
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{1, 2}};
 * auto t2 = fxt::tuple{3.0, 4.0};
 * auto result = exp | fxt::mtuple_cat(t2);
 * // result is fxt::expected<fxt::tuple<int, int, double, double>, Error>{1, 2, 3.0, 4.0}
 * @endcode
 *
 * ### Monadic concatenation with monadic tuple
 * @code
 * auto exp1 = fxt::expected<fxt::tuple<int>, Error>{fxt::tuple{1}};
 * auto exp2 = fxt::expected<fxt::tuple<double>, Error>{fxt::tuple{2.0}};
 * auto result = exp1 | fxt::mtuple_cat(exp2);
 * // result is fxt::expected<fxt::tuple<int, double>, Error>{1, 2.0}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft1 = fxt::flat_tuple<int, int>{1, 2};
 * auto ft2 = fxt::flat_tuple<double, double>{3.0, 4.0};
 * auto result = ft1 | fxt::tuple_cat(ft2);
 * // result is fxt::flat_tuple<int, int, double, double>{1, 2, 3.0, 4.0}
 * @endcode
 *
 * ## Important Notes
 *
 * - Both tuples must be of the same type family (both fxt::tuple or both fxt::flat_tuple)
 * - The resulting tuple contains all elements from the first tuple followed by all elements
 *   from the second tuple
 * - For monadic operations, if any input monad contains an error or nullopt, the result
 *   will also contain that error or nullopt
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::tuple_append
 * @see fxt::expected
 * @see fxt::optional
 */

#pragma once

#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include <fxt/concepts/IsTuple.hpp>
#include <fxt/concepts/IsMonad.hpp>
#include <tuple>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // Helper to concatenate two flat_tuples
        template<typename... Ts, typename... Us, std::size_t... Is, std::size_t... Js>
        constexpr auto flat_tuple_cat_impl(
            const flat_tuple<Ts...>& t1,
            const flat_tuple<Us...>& t2,
            std::index_sequence<Is...>,
            std::index_sequence<Js...>)
        {
            return make_flat_tuple(get<Is>(t1)..., get<Js>(t2)...);
        }

        template<typename... Ts, typename... Us, std::size_t... Is, std::size_t... Js>
        constexpr auto flat_tuple_cat_impl(
            flat_tuple<Ts...>&& t1,
            flat_tuple<Us...>&& t2,
            std::index_sequence<Is...>,
            std::index_sequence<Js...>)
        {
            return make_flat_tuple(get<Is>(std::move(t1))..., get<Js>(std::move(t2))...);
        }

        template<typename... Ts, typename... Us, std::size_t... Is, std::size_t... Js>
        constexpr auto flat_tuple_cat_impl(
            const flat_tuple<Ts...>& t1,
            flat_tuple<Us...>&& t2,
            std::index_sequence<Is...>,
            std::index_sequence<Js...>)
        {
            return make_flat_tuple(get<Is>(t1)..., get<Js>(std::move(t2))...);
        }

        template<typename... Ts, typename... Us, std::size_t... Is, std::size_t... Js>
        constexpr auto flat_tuple_cat_impl(
            flat_tuple<Ts...>&& t1,
            const flat_tuple<Us...>& t2,
            std::index_sequence<Is...>,
            std::index_sequence<Js...>)
        {
            return make_flat_tuple(get<Is>(std::move(t1))..., get<Js>(t2)...);
        }
    }

    /**
     * @brief Concatenate two fxt::tuple objects (const lvalue reference overload)
     *
     * For fxt::tuple (std::tuple), this forwards to std::tuple_cat to leverage
     * standard library optimizations. Creates a new tuple containing all elements
     * from both input tuples in order.
     *
     * @tparam Ts Types in the first tuple
     * @tparam Us Types in the second tuple
     * @param t1 First tuple (const lvalue reference)
     * @param t2 Second tuple (const lvalue reference)
     * @return A new tuple containing all elements from both tuples
     *
     * @code
     * auto t1 = fxt::make_tuple(1, 2);
     * auto t2 = fxt::make_tuple(3.0, 4.0);
     * auto result = fxt::tuple_cat(t1, t2);
     * // result is fxt::tuple<int, int, double, double>{1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const tuple<Ts...>& t1, const tuple<Us...>& t2)
    {
        return std::tuple_cat(t1, t2);
    }

    /**
     * @brief Concatenate two fxt::tuple objects (rvalue reference overload)
     *
     * Move-optimized version that takes both tuples as rvalue references,
     * allowing elements to be moved rather than copied when possible.
     *
     * @tparam Ts Types in the first tuple
     * @tparam Us Types in the second tuple
     * @param t1 First tuple (rvalue reference)
     * @param t2 Second tuple (rvalue reference)
     * @return A new tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(tuple<Ts...>&& t1, tuple<Us...>&& t2)
    {
        return std::tuple_cat(std::move(t1), std::move(t2));
    }

    /**
     * @brief Concatenate two fxt::tuple objects (mixed reference overload)
     *
     * Takes the first tuple as const lvalue reference and the second as rvalue reference.
     *
     * @tparam Ts Types in the first tuple
     * @tparam Us Types in the second tuple
     * @param t1 First tuple (const lvalue reference)
     * @param t2 Second tuple (rvalue reference)
     * @return A new tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const tuple<Ts...>& t1, tuple<Us...>&& t2)
    {
        return std::tuple_cat(t1, std::move(t2));
    }

    /**
     * @brief Concatenate two fxt::tuple objects (mixed reference overload)
     *
     * Takes the first tuple as rvalue reference and the second as const lvalue reference.
     *
     * @tparam Ts Types in the first tuple
     * @tparam Us Types in the second tuple
     * @param t1 First tuple (rvalue reference)
     * @param t2 Second tuple (const lvalue reference)
     * @return A new tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(tuple<Ts...>&& t1, const tuple<Us...>& t2)
    {
        return std::tuple_cat(std::move(t1), t2);
    }

    /**
     * @brief Concatenate two fxt::flat_tuple objects (const lvalue reference overload)
     *
     * Creates a new flat_tuple containing all elements from both input tuples in order.
     * Uses a custom implementation that unpacks and repacks elements.
     *
     * @tparam Ts Types in the first flat_tuple
     * @tparam Us Types in the second flat_tuple
     * @param t1 First flat_tuple (const lvalue reference)
     * @param t2 Second flat_tuple (const lvalue reference)
     * @return A new flat_tuple containing all elements from both tuples
     *
     * @code
     * auto ft1 = fxt::make_flat_tuple(1, 2);
     * auto ft2 = fxt::make_flat_tuple(3.0, 4.0);
     * auto result = fxt::tuple_cat(ft1, ft2);
     * // result is fxt::flat_tuple<int, int, double, double>{1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const flat_tuple<Ts...>& t1, const flat_tuple<Us...>& t2)
    {
        return impl::flat_tuple_cat_impl(
            t1, t2,
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

    /**
     * @brief Concatenate two fxt::flat_tuple objects (rvalue reference overload)
     *
     * Move-optimized version that takes both flat_tuples as rvalue references,
     * allowing elements to be moved rather than copied when possible.
     *
     * @tparam Ts Types in the first flat_tuple
     * @tparam Us Types in the second flat_tuple
     * @param t1 First flat_tuple (rvalue reference)
     * @param t2 Second flat_tuple (rvalue reference)
     * @return A new flat_tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(flat_tuple<Ts...>&& t1, flat_tuple<Us...>&& t2)
    {
        return impl::flat_tuple_cat_impl(
            std::move(t1), std::move(t2),
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

    /**
     * @brief Concatenate two fxt::flat_tuple objects (mixed reference overload)
     *
     * Takes the first flat_tuple as const lvalue reference and the second as rvalue reference.
     *
     * @tparam Ts Types in the first flat_tuple
     * @tparam Us Types in the second flat_tuple
     * @param t1 First flat_tuple (const lvalue reference)
     * @param t2 Second flat_tuple (rvalue reference)
     * @return A new flat_tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const flat_tuple<Ts...>& t1, flat_tuple<Us...>&& t2)
    {
        return impl::flat_tuple_cat_impl(
            t1, std::move(t2),
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

    /**
     * @brief Concatenate two fxt::flat_tuple objects (mixed reference overload)
     *
     * Takes the first flat_tuple as rvalue reference and the second as const lvalue reference.
     *
     * @tparam Ts Types in the first flat_tuple
     * @tparam Us Types in the second flat_tuple
     * @param t1 First flat_tuple (rvalue reference)
     * @param t2 Second flat_tuple (const lvalue reference)
     * @return A new flat_tuple containing all elements from both tuples
     */
    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(flat_tuple<Ts...>&& t1, const flat_tuple<Us...>& t2)
    {
        return impl::flat_tuple_cat_impl(
            std::move(t1), t2,
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

    /**
     * @brief Curried version of tuple_cat for pipelining
     *
     * Returns a lambda that concatenates the given tuple with another tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the type. Both tuples
     * must be of the same type family.
     *
     * @tparam Tuple2 Type of the second tuple (to be concatenated)
     * @param t2 The second tuple to concatenate
     * @return A lambda that takes the first tuple and returns the concatenated result
     *
     * @code
     * auto t1 = fxt::make_tuple(1, 2);
     * auto t2 = fxt::make_tuple(3.0, 4.0);
     * auto result = t1 | fxt::tuple_cat(t2);
     * // result is fxt::tuple<int, int, double, double>{1, 2, 3.0, 4.0}
     *
     * // Chain multiple concatenations
     * auto result2 = fxt::tuple{1, 2}
     *     | fxt::tuple_cat(fxt::tuple{3.0})
     *     | fxt::tuple_cat(fxt::tuple{true, false});
     * // result2 is fxt::tuple<int, int, double, bool, bool>{1, 2, 3.0, true, false}
     *
     * // Also works with flat_tuple
     * auto ft1 = fxt::make_flat_tuple(1, 2);
     * auto ft2 = fxt::make_flat_tuple(3.0, 4.0);
     * auto result3 = ft1 | fxt::tuple_cat(ft2);
     * // result3 is fxt::flat_tuple<int, int, double, double>{1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename Tuple2>
    requires fxt::tuple_like<std::remove_cvref_t<Tuple2>>
    constexpr auto tuple_cat(Tuple2&& t2)
    {
        return [t2 = std::forward<Tuple2>(t2)]<typename T0>(T0&& t1) mutable {
            return fxt::tuple_cat(std::forward<T0>(t1), std::move(t2));
        };
    }

    // ========================================================================
    // fxt::mtuple_cat - Monadic tuple concatenation
    // ========================================================================

    /**
     * @brief Concatenate a tuple inside a monad with a plain tuple (direct call)
     *
     * Takes a monad (fxt::expected or fxt::optional) containing a tuple and a plain tuple,
     * and concatenates them if the monad contains a value. This overload is for when the
     * second argument is a plain tuple (not inside a monad).
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container1 The monad type containing the first tuple
     * @tparam Tuple2 The plain tuple type
     * @param container The monad containing the first tuple
     * @param t2 The plain tuple to concatenate
     * @return A monad containing the concatenated tuple
     *
     * @code
     * // Concatenate monad<tuple> with plain tuple
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto t2 = fxt::make_tuple(3.0, 4.0);
     * auto result = fxt::mtuple_cat(exp, t2);
     * // result is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Works with flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
     * auto ft2 = fxt::make_flat_tuple(3.0, 4.0);
     * auto result2 = fxt::mtuple_cat(opt, ft2);
     * // result2 is fxt::optional<fxt::flat_tuple<int, int, double, double>> containing {1, 2, 3.0, 4.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result3 = fxt::mtuple_cat(exp_err, fxt::make_tuple(3.0, 4.0));
     * // result3 contains the error
     * @endcode
     */
    template<typename Container1, typename Tuple2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::tuple_like<std::remove_cvref_t<Tuple2>>
    constexpr auto mtuple_cat(Container1&& container, Tuple2&& t2)
    {
        return std::forward<Container1>(container).transform([t2 = std::forward<Tuple2>(t2)]<typename T0>(T0&& t1) mutable {
            return fxt::tuple_cat(std::forward<T0>(t1), std::move(t2));
        });
    }

    /**
     * @brief Concatenate two tuples inside monads (direct call)
     *
     * Takes two monads (fxt::expected or fxt::optional) each containing a tuple,
     * and concatenates them if both monads contain values. If either monad contains
     * an error or nullopt, the result will contain that error or nullopt.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container1 The first monad type containing a tuple
     * @tparam Container2 The second monad type containing a tuple
     * @param container1 The first monad containing a tuple
     * @param container2 The second monad containing a tuple
     * @return A monad containing the concatenated tuple if both inputs are valid
     *
     * @code
     * auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto exp2 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::make_tuple(3.0, 4.0)};
     * auto result = fxt::mtuple_cat(exp1, exp2);
     * // result is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Error in first monad
     * auto exp_err1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result2 = fxt::mtuple_cat(exp_err1, exp2);
     * // result2 contains the error from exp_err1
     *
     * // Error in second monad
     * auto exp_err2 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::unexpected{Error{}}};
     * auto result3 = fxt::mtuple_cat(exp1, exp_err2);
     * // result3 contains the error from exp_err2
     * @endcode
     */
    template<typename Container1, typename Container2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::monad_like<std::remove_cvref_t<Container2>>
    constexpr auto mtuple_cat(Container1&& container1, Container2&& container2)
    {
        return std::forward<Container1>(container1).and_then([container2 = std::forward<Container2>(container2)]<typename TTuple1>(TTuple1&& t1) mutable {
            return std::forward<Container2>(container2).transform([t1 = std::forward<TTuple1>(t1)]<typename TTuple2>(TTuple2&& t2) mutable {
                return fxt::tuple_cat(std::move(t1), std::forward<TTuple2>(t2));
            });
        });
    }

    /**
     * @brief Curried version of mtuple_cat for pipelining
     *
     * Returns a lambda that concatenates a tuple inside a monad with another tuple.
     * This enables usage with the pipe operator for monadic tuple concatenation operations.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     * The second argument can be a plain tuple or a tuple inside a monad.
     *
     * @tparam Tuple2 Type of the second tuple (or monad containing a tuple)
     * @param t2 The second tuple (or monad containing the second tuple)
     * @return A lambda that takes a monad<tuple> and returns a monad<concatenated_tuple>
     *
     * @code
     * // Pipe operator with monad<tuple> and plain tuple
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto t2 = fxt::make_tuple(3.0, 4.0);
     * auto result = exp | fxt::mtuple_cat(t2);
     * // result is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Pipe operator with two monad<tuple>
     * auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto exp2 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::make_tuple(3.0, 4.0)};
     * auto result2 = exp1 | fxt::mtuple_cat(exp2);
     * // result2 is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Chaining multiple concatenations
     * auto result3 = exp1
     *     | fxt::mtuple_cat(fxt::make_tuple(5.0))
     *     | fxt::mtuple_cat(fxt::make_tuple(true, false));
     * // result3 is fxt::expected<fxt::tuple<int, int, double, bool, bool>, Error>
     *
     * // Works with optional and flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
     * auto result4 = opt | fxt::mtuple_cat(fxt::make_flat_tuple(3.0, 4.0));
     * // result4 is fxt::optional<fxt::flat_tuple<int, int, double, double>> containing {1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename Tuple2>
    constexpr auto mtuple_cat(Tuple2&& t2)
    {
        return [t2 = std::forward<Tuple2>(t2)]<typename TMonad>(TMonad&& container) mutable {
            return fxt::mtuple_cat(std::forward<TMonad>(container), std::move(t2));
        };
    }

} // namespace fxt