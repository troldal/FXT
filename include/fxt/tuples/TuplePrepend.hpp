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
 * @file TuplePrepend.hpp
 * @brief Tuple prepend operations for fxt::tuple and fxt::flat_tuple
 *
 * This file provides functions for prepending elements to tuples in both plain and monadic contexts.
 * All functions work seamlessly with both fxt::tuple (std::tuple) and fxt::flat_tuple, preserving
 * the tuple type through the operations.
 *
 * ## Main Functions
 *
 * ### fxt::tuple_prepend
 * Prepends one or more elements to a tuple. Available in two forms:
 * - **Direct call**: `tuple_prepend(tuple, values...)` - Takes a tuple and values to prepend
 * - **Curried form**: `tuple_prepend(values...)` - Returns a lambda for pipeline usage with operator|
 *
 * ### fxt::mtuple_prepend
 * Prepends elements to a tuple inside a monad (fxt::expected or fxt::optional). Available in multiple forms:
 * - **Plain values**: `mtuple_prepend(monad<tuple>, values...)` - Prepends plain values to monadic tuple
 * - **Monadic values**: `mtuple_prepend(monad<tuple>, monad<value>...)` - Combines monadic values
 * - **Curried form**: `mtuple_prepend(values...)` - Returns a lambda for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple, maintaining the tuple type
 * - Variadic: Supports prepending single or multiple values at once
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_prepend handles error/nullopt propagation automatically
 * - Mixed operations: Can combine plain values and monadic values in a single call
 *
 * ## Examples
 *
 * ### Basic tuple prepend
 * @code
 * auto t = fxt::tuple{1, 2};
 * auto result = fxt::tuple_prepend(t, 3, 4); // fxt::tuple{3, 4, 1, 2}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{1, 2}
 *     | fxt::tuple_prepend(3)
 *     | fxt::tuple_prepend(4, 5);  // fxt::tuple{4, 5, 3, 1, 2}
 * @endcode
 *
 * ### Monadic prepend with plain values
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{1, 2}};
 * auto result = exp | fxt::mtuple_prepend(3, 4);
 * // result is fxt::expected<fxt::tuple<int, int, int, int>, Error>{3, 4, 1, 2}
 * @endcode
 *
 * ### Monadic prepend with monadic values
 * @code
 * auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::tuple{1}};
 * auto exp_val1 = fxt::expected<int, Error>{2};
 * auto exp_val2 = fxt::expected<int, Error>{3};
 * auto result = exp_tuple | fxt::mtuple_prepend(exp_val1) | fxt::mtuple_prepend(exp_val2);
 * // result is fxt::expected<fxt::tuple<int, int, int>, Error>{3, 2, 1}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{1.0, 2.0};
 * auto result = ft | fxt::tuple_prepend(3.0, 4.0);
 * // result is fxt::flat_tuple<double, double, double, double>{3.0, 4.0, 1.0, 2.0}
 * @endcode
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::tuple_append
 * @see fxt::expected
 * @see fxt::optional
 */

#pragma once

#include "TupleSize.hpp"
#include "../concepts/IsTuple.hpp"
#include "../concepts/IsMonad.hpp"

namespace fxt::impl
{
    // Generic prepend implementation that works for both tuple types - single value
    template<class Tuple, class U, std::size_t... I>
    auto prepend_impl(Tuple&& t, U&& u, std::index_sequence<I...>)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<U>, std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>...>;
            return ResultType(std::forward<U>(u), fxt::get<I>(std::forward<Tuple>(t))...);
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) -> flat_tuple<std::decay_t<U>, std::decay_t<Ts>...> {
                return std::declval<flat_tuple<std::decay_t<U>, std::decay_t<Ts>...>>();
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(std::forward<U>(u), fxt::get<I>(std::forward<Tuple>(t))...);
        }
    }

    // Generic prepend implementation that works for both tuple types - multiple values
    template<class Tuple, class... Us, std::size_t... I>
    auto prepend_impl_variadic(Tuple&& t, std::index_sequence<I...>, Us&&... us)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<Us>..., std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>...>;
            return ResultType(std::forward<Us>(us)..., fxt::get<I>(std::forward<Tuple>(t))...);
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) -> flat_tuple<std::decay_t<Us>..., std::decay_t<Ts>...> {
                return std::declval<flat_tuple<std::decay_t<Us>..., std::decay_t<Ts>...>>();
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(std::forward<Us>(us)..., fxt::get<I>(std::forward<Tuple>(t))...);
        }
    }

}    // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Prepend one or more elements to a tuple
     *
     * Generic overload that works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Uses fxt::tuple_size_v to determine the tuple size, which works for both types.
     * Accepts at least one element to prepend, but can prepend multiple elements at once.
     *
     * @tparam Tuple The tuple type (fxt::tuple or fxt::flat_tuple)
     * @tparam U The type of the first element to prepend
     * @tparam Us The types of additional elements to prepend
     * @param t The tuple to prepend to
     * @param u The first element to prepend
     * @param us Additional elements to prepend
     * @return A new tuple with all elements prepended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2};
     * auto result = fxt::tuple_prepend(t, 3); // fxt::tuple{3, 1, 2}
     * auto result2 = fxt::tuple_prepend(t, 3, 4, 5); // fxt::tuple{3, 4, 5, 1, 2}
     * @endcode
     */
    template<class Tuple, class U, class... Us>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    auto tuple_prepend(Tuple&& t, U&& u, Us&&... us)
    {
        constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        if constexpr (sizeof...(Us) == 0) {
            // Single element prepend
            return impl::prepend_impl(std::forward<Tuple>(t), std::forward<U>(u), std::make_index_sequence<N>{});
        } else {
            // Multiple elements prepend
            return impl::prepend_impl_variadic(std::forward<Tuple>(t), std::make_index_sequence<N>{}, std::forward<U>(u), std::forward<Us>(us)...);
        }
    }

    /**
     * @brief Prepend one or more elements to a tuple (curried version for pipeline usage)
     *
     * Returns a lambda that prepends the given elements to a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Accepts at least one element to prepend, but can prepend multiple elements at once.
     *
     * @tparam U The type of the first element to prepend
     * @tparam Us The types of additional elements to prepend
     * @param u The first element to prepend
     * @param us Additional elements to prepend
     * @return A lambda that takes a tuple and returns a new tuple with all elements prepended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2, 3};
     * auto result = t | fxt::tuple_prepend(4); // fxt::tuple{4, 1, 2, 3}
     * auto result2 = t | fxt::tuple_prepend(4, 5, 6); // fxt::tuple{4, 5, 6, 1, 2, 3}
     *
     * auto ft = fxt::flat_tuple<double, double>{1.0, 2.0};
     * auto result3 = ft | fxt::tuple_prepend(3.0, 4.0); // fxt::flat_tuple{3.0, 4.0, 1.0, 2.0}
     * @endcode
     */
    template<class U, class... Us>
        requires (!impl::is_tuple_like_v<std::remove_cvref_t<U>>)
    auto tuple_prepend(U&& u, Us&&... us)
    {
        if constexpr (sizeof...(Us) == 0) {
            // Single element version
            return [value = std::forward<U>(u)]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                return impl::prepend_impl(std::forward<Tuple>(t), std::forward<U>(value), std::make_index_sequence<N>{});
            };
        } else {
            // Multiple elements version - capture values in a tuple and unpack them
            return [values = fxt::tuple<U, Us...>{std::forward<U>(u), std::forward<Us>(us)...}]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                constexpr std::size_t M = sizeof...(Us) + 1;

                // Unpack the captured tuple and pass to prepend_impl_variadic
                return [&t, &values, N]<std::size_t... I>(std::index_sequence<I...>) {
                    return impl::prepend_impl_variadic(
                        std::forward<Tuple>(t),
                        std::make_index_sequence<N>{},
                        std::move(fxt::get<I>(values))...
                    );
                }(std::make_index_sequence<M>{});
            };
        }
    }

    // ========================================================================
    // fxt::mtuple_prepend - Monadic tuple prepend
    // ========================================================================

    /**
     * @brief Prepend one or more elements to a tuple inside a monad (direct call)
     *
     * Takes a monad (fxt::expected or fxt::optional) containing a tuple and one or more values
     * to prepend. The values can be plain values or values inside monads.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container The monad type containing the tuple
     * @tparam U The type of the first element to prepend
     * @tparam Us The types of additional elements to prepend
     * @param container The monad containing the tuple
     * @param u The first element to prepend
     * @param us Additional elements to prepend
     * @return A monad containing the tuple with elements prepended
     *
     * @code
     * // Prepend plain value to monad<tuple>
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result = fxt::mtuple_prepend(exp, 3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 1, 2}
     *
     * // Prepend multiple values
     * auto result2 = fxt::mtuple_prepend(exp, 3, 4, 5);
     * // result2 is fxt::expected<fxt::tuple<int, int, int, int, int>, Error> containing {3, 4, 5, 1, 2}
     *
     * // Works with flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result3 = fxt::mtuple_prepend(opt, 3.0);
     * // result3 is fxt::optional<fxt::flat_tuple<double, double, double>> containing {3.0, 1.0, 2.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result4 = fxt::mtuple_prepend(exp_err, 3);
     * // result4 contains the error
     * @endcode
     */
    template<typename Container, typename U, typename... Us>
    requires fxt::monad_like<std::remove_cvref_t<Container>> && (!fxt::monad_like<std::remove_cvref_t<U>>)
    constexpr auto mtuple_prepend(Container&& container, U&& u, Us&&... us)
    {
        if constexpr (sizeof...(Us) == 0) {
            // Single value prepend
            return std::forward<Container>(container).transform([value = std::forward<U>(u)]<typename TTuple>(TTuple&& t) mutable {
                return fxt::tuple_prepend(std::forward<TTuple>(t), std::forward<U>(value));
            });
        } else {
            // Multiple values prepend
            return std::forward<Container>(container).transform([values = fxt::tuple<U, Us...>{std::forward<U>(u), std::forward<Us>(us)...}]<typename TTuple>(TTuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<TTuple>>;
                constexpr std::size_t M = sizeof...(Us) + 1;
                return [&t, &values]<std::size_t... I>(std::index_sequence<I...>) {
                    return impl::prepend_impl_variadic(
                        std::forward<TTuple>(t),
                        std::make_index_sequence<N>{},
                        std::move(fxt::get<I>(values))...
                    );
                }(std::make_index_sequence<M>{});
            });
        }
    }

    /**
     * @brief Prepend a value inside a monad to a tuple inside a monad (direct call)
     *
     * Takes a monad containing a tuple and a monad containing a value, and prepends
     * the value to the tuple if both monads contain values.
     *
     * @tparam Container1 The monad type containing the tuple
     * @tparam Container2 The monad type containing the value to prepend
     * @param container1 The monad containing the tuple
     * @param container2 The monad containing the value to prepend
     * @return A monad containing the tuple with the value prepended if both inputs are valid
     *
     * @code
     * auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto exp2 = fxt::expected<int, Error>{3};
     * auto result = fxt::mtuple_prepend(exp1, exp2);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 1, 2}
     * @endcode
     */
    template<typename Container1, typename Container2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::monad_like<std::remove_cvref_t<Container2>>
    constexpr auto mtuple_prepend(Container1&& container1, Container2&& container2)
    {
        return std::forward<Container1>(container1).and_then([container2 = std::forward<Container2>(container2)]<typename TTuple>(TTuple&& t) mutable {
            return std::forward<Container2>(container2).transform([t = std::forward<TTuple>(t)]<typename TValue>(TValue&& _value) mutable {
                return fxt::tuple_prepend(std::move(t), std::forward<TValue>(_value));
            });
        });
    }

    /**
     * @brief Prepend values inside monads to a tuple inside a monad (direct call, variadic)
     *
     * Takes a monad containing a tuple and multiple monads containing values, and prepends
     * all values to the tuple if all monads contain values.
     *
     * @tparam Container1 The monad type containing the tuple
     * @tparam Container2 The monad type containing the first value
     * @tparam Containers The monad types containing additional values
     * @param container1 The monad containing the tuple
     * @param container2 The monad containing the first value to prepend
     * @param containers The monads containing additional values to prepend
     * @return A monad containing the tuple with all values prepended if all inputs are valid
     *
     * @code
     * auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
     * auto exp2 = fxt::expected<int, Error>{2};
     * auto exp3 = fxt::expected<int, Error>{3};
     * auto result = fxt::mtuple_prepend(exp_tuple, exp2, exp3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 3, 1}
     * @endcode
     */
    template<typename Container1, typename Container2, typename... Containers>
    requires fxt::monad_like<std::remove_cvref_t<Container1>>
        && fxt::monad_like<std::remove_cvref_t<Container2>>
        && (fxt::monad_like<std::remove_cvref_t<Containers>> && ...)
    constexpr auto mtuple_prepend(Container1&& container1, Container2&& container2, Containers&&... containers)
    {
        if constexpr (sizeof...(Containers) == 0) {
            // Base case: just two containers, prepend second to first
            return mtuple_prepend(std::forward<Container1>(container1), std::forward<Container2>(container2));
        } else {
            // Recursive case: prepend all remaining values first, then prepend container2
            // This ensures correct order: first we add containers..., then we add container2
            auto rest_result = mtuple_prepend(std::forward<Container1>(container1), std::forward<Containers>(containers)...);
            // Now prepend container2 to the front
            return mtuple_prepend(std::move(rest_result), std::forward<Container2>(container2));
        }
    }

    /**
     * @brief Curried version of mtuple_prepend for pipelining
     *
     * Returns a lambda that prepends values to a tuple inside a monad.
     * This enables usage with the pipe operator for monadic tuple prepend operations.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     * The values to prepend can be plain values or values inside monads.
     *
     * @tparam U The type of the first element to prepend
     * @tparam Us The types of additional elements to prepend
     * @param u The first element to prepend
     * @param us Additional elements to prepend
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_with_prepended_elements>
     *
     * @code
     * // Pipe operator with monad<tuple> and plain value
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result = exp | fxt::mtuple_prepend(3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 1, 2}
     *
     * // Pipe operator with multiple values
     * auto result2 = exp | fxt::mtuple_prepend(3, 4, 5);
     * // result2 is fxt::expected<fxt::tuple<int, int, int, int, int>, Error> containing {3, 4, 5, 1, 2}
     *
     * // Pipe operator with monad<value>
     * auto exp_val = fxt::expected<int, Error>{3};
     * auto result3 = exp | fxt::mtuple_prepend(exp_val);
     * // result3 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {3, 1, 2}
     *
     * // Chaining with other monadic operations
     * auto result4 = exp
     *     | fxt::mtransform_tuple([](auto x) { return x * 2; })
     *     | fxt::mtuple_prepend(5, 6);
     * // result4 is fxt::expected<fxt::tuple<int, int, int, int>, Error> containing {5, 6, 2, 4}
     *
     * // Works with optional and flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result5 = opt | fxt::mtuple_prepend(3.0, 4.0);
     * // result5 is fxt::optional<fxt::flat_tuple<double, double, double, double>> containing {3.0, 4.0, 1.0, 2.0}
     * @endcode
     */
    template<typename U, typename... Us>
    constexpr auto mtuple_prepend(U&& u, Us&&... us)
    {
        return [u = std::forward<U>(u), ... us = std::forward<Us>(us)]<typename TMonad>(TMonad&& container) mutable {
            return fxt::mtuple_prepend(std::forward<TMonad>(container), std::move(u), std::move(us)...);
        };
    }

}    // namespace fxt
