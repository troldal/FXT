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
 * @file TupleAppend.hpp
 * @brief Tuple append operations for fxt::tuple and fxt::flat_tuple
 *
 * This file provides functions for appending elements to tuples in both plain and monadic contexts.
 * All functions work seamlessly with both fxt::tuple (std::tuple) and fxt::flat_tuple, preserving
 * the tuple type through the operations.
 *
 * ## Main Functions
 *
 * ### fxt::tuple_append
 * Appends one or more elements to a tuple. Available in two forms:
 * - **Direct call**: `tuple_append(tuple, values...)` - Takes a tuple and values to append
 * - **Curried form**: `tuple_append(values...)` - Returns a lambda for pipeline usage with operator|
 *
 * ### fxt::mtuple_append
 * Appends elements to a tuple inside a monad (fxt::expected or fxt::optional). Available in multiple forms:
 * - **Plain values**: `mtuple_append(monad<tuple>, values...)` - Appends plain values to monadic tuple
 * - **Monadic values**: `mtuple_append(monad<tuple>, monad<value>...)` - Combines monadic values
 * - **Curried form**: `mtuple_append(values...)` - Returns a lambda for pipeline usage with operator|
 *
 * ## Key Features
 * - Type-preserving: Works with both fxt::tuple and fxt::flat_tuple, maintaining the tuple type
 * - Variadic: Supports appending single or multiple values at once
 * - Pipeline-friendly: Curried versions enable fluent chaining with the pipe operator
 * - Monadic: mtuple_append handles error/nullopt propagation automatically
 * - Mixed operations: Can combine plain values and monadic values in a single call
 *
 * ## Examples
 *
 * ### Basic tuple append
 * @code
 * auto t = fxt::tuple{1, 2};
 * auto result = fxt::tuple_append(t, 3, 4); // fxt::tuple{1, 2, 3, 4}
 * @endcode
 *
 * ### Pipeline usage
 * @code
 * auto result = fxt::tuple{1, 2}
 *     | fxt::tuple_append(3)
 *     | fxt::tuple_append(4, 5);  // fxt::tuple{1, 2, 3, 4, 5}
 * @endcode
 *
 * ### Monadic append with plain values
 * @code
 * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::tuple{1, 2}};
 * auto result = exp | fxt::mtuple_append(3, 4);
 * // result is fxt::expected<fxt::tuple<int, int, int, int>, Error>{1, 2, 3, 4}
 * @endcode
 *
 * ### Monadic append with monadic values
 * @code
 * auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::tuple{1}};
 * auto exp_val1 = fxt::expected<int, Error>{2};
 * auto exp_val2 = fxt::expected<int, Error>{3};
 * auto result = exp_tuple | fxt::mtuple_append(exp_val1) | fxt::mtuple_append(exp_val2);
 * // result is fxt::expected<fxt::tuple<int, int, int>, Error>{1, 2, 3}
 * @endcode
 *
 * ### Works with flat_tuple
 * @code
 * auto ft = fxt::flat_tuple<double, double>{1.0, 2.0};
 * auto result = ft | fxt::tuple_append(3.0, 4.0);
 * // result is fxt::flat_tuple<double, double, double, double>{1.0, 2.0, 3.0, 4.0}
 * @endcode
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::expected
 * @see fxt::optional
 */

#pragma once

#include "TupleSize.hpp"
#include "Get.hpp"
#include "../concepts/IsTuple.hpp"
#include "../concepts/IsMonad.hpp"

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
            // Copy-constructible: std::as_const forces a copy on every call → reusable adaptor.
            // Move-only: std::move is unavoidable → single-use by necessity (the type itself
            // prevents reuse; the caller must re-construct the adaptor for a second pipeline).
            return [value = std::forward<U>(u)]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                if constexpr (std::is_copy_constructible_v<std::decay_t<U>>) {
                    return impl::append_impl(std::forward<Tuple>(t), std::as_const(value), std::make_index_sequence<N>{});
                } else {
                    return impl::append_impl(std::forward<Tuple>(t), std::move(value), std::make_index_sequence<N>{});
                }
            };
        } else {
            return [values = fxt::tuple<std::decay_t<U>, std::decay_t<Us>...>{
                        std::forward<U>(u), std::forward<Us>(us)...}]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                constexpr std::size_t M = sizeof...(Us) + 1;
                return [&t, &values]<std::size_t... I>(std::index_sequence<I...>) {
                    if constexpr (std::is_copy_constructible_v<std::decay_t<U>> &&
                                  (std::is_copy_constructible_v<std::decay_t<Us>> && ...)) {
                        return impl::append_impl_variadic(
                            std::forward<Tuple>(t), std::make_index_sequence<N>{},
                            std::as_const(fxt::get<I>(values))...);
                    } else {
                        return impl::append_impl_variadic(
                            std::forward<Tuple>(t), std::make_index_sequence<N>{},
                            std::move(fxt::get<I>(values))...);
                    }
                }(std::make_index_sequence<M>{});
            };
        }
    }

    // ========================================================================
    // fxt::mtuple_append - Monadic tuple append
    // ========================================================================

    /**
     * @brief Append one or more elements to a tuple inside a monad (direct call)
     *
     * Takes a monad (fxt::expected or fxt::optional) containing a tuple and one or more values
     * to append. The values can be plain values or values inside monads.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container The monad type containing the tuple
     * @tparam U The type of the first element to append
     * @tparam Us The types of additional elements to append
     * @param container The monad containing the tuple
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A monad containing the tuple with elements appended
     *
     * @code
     * // Append plain value to monad<tuple>
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result = fxt::mtuple_append(exp, 3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {1, 2, 3}
     *
     * // Append multiple values
     * auto result2 = fxt::mtuple_append(exp, 3, 4, 5);
     * // result2 is fxt::expected<fxt::tuple<int, int, int, int, int>, Error> containing {1, 2, 3, 4, 5}
     *
     * // Works with flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result3 = fxt::mtuple_append(opt, 3.0);
     * // result3 is fxt::optional<fxt::flat_tuple<double, double, double>> containing {1.0, 2.0, 3.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result4 = fxt::mtuple_append(exp_err, 3);
     * // result4 contains the error
     * @endcode
     */
    template<typename Container, typename U, typename... Us>
    requires fxt::monad_like<std::remove_cvref_t<Container>> && (!fxt::monad_like<std::remove_cvref_t<U>>)
    constexpr auto mtuple_append(Container&& container, U&& u, Us&&... us)
    {
        if constexpr (sizeof...(Us) == 0) {
            // Single value append
            return std::forward<Container>(container).transform([value = std::forward<U>(u)]<typename TTuple>(TTuple&& t) mutable {
                return fxt::tuple_append(std::forward<TTuple>(t), std::forward<U>(value));
            });
        } else {
            // Multiple values append
            return std::forward<Container>(container).transform([values = fxt::tuple<U, Us...>{std::forward<U>(u), std::forward<Us>(us)...}]<typename TTuple>(TTuple&& t) mutable {
                constexpr std::size_t M = sizeof...(Us) + 1;
                return [&t, &values]<std::size_t... I>(std::index_sequence<I...>) {
                    return impl::append_impl_variadic(
                        std::forward<TTuple>(t),
                        std::make_index_sequence<fxt::tuple_size_v<std::remove_reference_t<TTuple>>>{},
                        std::move(fxt::get<I>(values))...
                    );
                }(std::make_index_sequence<M>{});
            });
        }
    }

    /**
     * @brief Append a value inside a monad to a tuple inside a monad (direct call)
     *
     * Takes a monad containing a tuple and a monad containing a value, and appends
     * the value to the tuple if both monads contain values.
     *
     * @tparam Container1 The monad type containing the tuple
     * @tparam Container2 The monad type containing the value to append
     * @param container1 The monad containing the tuple
     * @param container2 The monad containing the value to append
     * @return A monad containing the tuple with the value appended if both inputs are valid
     *
     * @code
     * auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto exp2 = fxt::expected<int, Error>{3};
     * auto result = fxt::mtuple_append(exp1, exp2);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {1, 2, 3}
     * @endcode
     */
    template<typename Container1, typename Container2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::monad_like<std::remove_cvref_t<Container2>>
    constexpr auto mtuple_append(Container1&& container1, Container2&& container2)
    {
        return std::forward<Container1>(container1).and_then([container2 = std::forward<Container2>(container2)]<typename TTuple>(TTuple&& t) mutable {
            return std::forward<Container2>(container2).transform([t = std::forward<TTuple>(t)]<typename TValue>(TValue&& _value) mutable {
                return fxt::tuple_append(std::move(t), std::forward<TValue>(_value));
            });
        });
    }

    /**
     * @brief Append values inside monads to a tuple inside a monad (direct call, variadic)
     *
     * Takes a monad containing a tuple and multiple monads containing values, and appends
     * all values to the tuple if all monads contain values.
     *
     * @tparam Container1 The monad type containing the tuple
     * @tparam Container2 The monad type containing the first value
     * @tparam Containers The monad types containing additional values
     * @param container1 The monad containing the tuple
     * @param container2 The monad containing the first value to append
     * @param containers The monads containing additional values to append
     * @return A monad containing the tuple with all values appended if all inputs are valid
     *
     * @code
     * auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
     * auto exp2 = fxt::expected<int, Error>{2};
     * auto exp3 = fxt::expected<int, Error>{3};
     * auto result = fxt::mtuple_append(exp_tuple, exp2, exp3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {1, 2, 3}
     * @endcode
     */
    template<typename Container1, typename Container2, typename... Containers>
    requires fxt::monad_like<std::remove_cvref_t<Container1>>
        && fxt::monad_like<std::remove_cvref_t<Container2>>
        && (fxt::monad_like<std::remove_cvref_t<Containers>> && ...)
    constexpr auto mtuple_append(Container1&& container1, Container2&& container2, Containers&&... containers)
    {
        // First append container2 to container1
        auto result = mtuple_append(std::forward<Container1>(container1), std::forward<Container2>(container2));

        // Then recursively append remaining containers
        if constexpr (sizeof...(Containers) > 0) {
            return mtuple_append(std::move(result), std::forward<Containers>(containers)...);
        } else {
            return result;
        }
    }

    /**
     * @brief Curried overload: append from an expected-like container (lvalue)
     *
     * Returns a pipeline adaptor that appends the value held by @p value to a tuple
     * inside another expected-like container. Supports error-type coercion: @c TError
     * need only be convertible to the container's error type @c TError2, enabling
     * composition across differently-typed expected chains.
     *
     * The appended @p value is checked first. When @p value holds an error its error
     * (converted to @c TError2) propagates immediately, before the container is
     * inspected. This is the preferred behaviour for monadic append pipelines.
     *
     * @tparam TExpected  The expected-like template (e.g. @c std::expected).
     * @tparam TValue     The value type held by @p value.
     * @tparam TError     The error type of @p value.
     * @param  value      The expected-like container holding the value to append (copied).
     * @return A pipeline adaptor that appends @c *value to the tuple inside its argument.
     *
     * @code
     * // Same-error-type append
     * auto exp_tuple = fxt::expected<fxt::tuple<int>, std::string>{fxt::make_tuple(1)};
     * auto exp_val   = fxt::expected<int, std::string>{2};
     * auto result    = exp_tuple | fxt::mtuple_append(exp_val);
     * // result: fxt::expected<fxt::tuple<int, int>, std::string> containing {1, 2}
     *
     * // Different-error-type (MyError convertible to std::string)
     * auto exp_val2  = fxt::expected<int, MyError>{3};
     * auto result2   = exp_tuple | fxt::mtuple_append(exp_val2);
     * // result2: fxt::expected<fxt::tuple<int, int>, std::string> containing {1, 3}
     * @endcode
     */
    template<template<typename, typename> class TExpected, typename TValue, typename TError>
        requires expected_like<TExpected<TValue, TError>>
    constexpr auto mtuple_append(const TExpected<TValue, TError>& value)
    {
        return [value]<template<typename, typename> class TExp2, typename TTuple, typename TError2>(
                   const TExp2<TTuple, TError2>& tupleContainer)
            requires expected_constructible_like<TExp2<TTuple, TError2>>
                  && std::convertible_to<TError, TError2>
        {
            return value
                ? tupleContainer.transform([&value](const TTuple& t) { return fxt::tuple_append(t, *value); })
                : typename TExp2<TTuple, TError2>::unexpected_type(value.error());
        };
    }

    /**
     * @brief Curried overload: append from an expected-like container (rvalue, move-optimised)
     *
     * Move-optimised version for use with temporary expected values or move-only wrapped
     * types (e.g. @c std::unique_ptr). Behaves identically to the lvalue overload but
     * moves instead of copying.
     *
     * @tparam TExpected  The expected-like template.
     * @tparam TValue     The value type (may be move-only).
     * @tparam TError     The error type of @p value.
     * @param  value      The expected-like container to move from.
     * @return A pipeline adaptor (mutable lambda) that appends @c std::move(*value).
     *
     * @code
     * auto exp_tuple = fxt::expected<fxt::tuple<int>, std::string>{fxt::make_tuple(1)};
     * auto result = std::move(exp_tuple)
     *     | fxt::mtuple_append(fxt::expected<std::unique_ptr<int>, std::string>{
     *                               std::make_unique<int>(42)});
     * // result: fxt::expected<fxt::tuple<int, std::unique_ptr<int>>, std::string>
     * @endcode
     */
    template<template<typename, typename> class TExpected, typename TValue, typename TError>
        requires expected_like<TExpected<TValue, TError>>
    constexpr auto mtuple_append(TExpected<TValue, TError>&& value)
    {
        return [value = std::move(value)]<template<typename, typename> class TExp2, typename TTuple, typename TError2>(
                   const TExp2<TTuple, TError2>& tupleContainer) mutable
            requires expected_constructible_like<TExp2<TTuple, TError2>>
                  && std::convertible_to<TError, TError2>
        {
            return value
                ? tupleContainer.transform([value = std::move(value)](const TTuple& t) mutable {
                    return fxt::tuple_append(t, std::move(*value)); })
                : typename TExp2<TTuple, TError2>::unexpected_type(std::move(value.error()));
        };
    }

    /**
     * @brief Curried version of mtuple_append for pipelining (plain values and optional-like)
     *
     * Returns a lambda that appends values to a tuple inside a monad.
     * This enables usage with the pipe operator for monadic tuple append operations.
     * For expected-like arguments prefer the overloads above, which support error-type
     * coercion and have cleaner value-first error semantics.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     * The values to append can be plain values or optional-like monadic values.
     *
     * @tparam U The type of the first element to append (plain value or optional-like)
     * @tparam Us The types of additional elements to append
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A lambda that takes a monad<tuple> and returns a monad<tuple_with_appended_elements>
     *
     * @code
     * // Pipe operator with monad<tuple> and plain value
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result = exp | fxt::mtuple_append(3);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {1, 2, 3}
     *
     * // Pipe operator with multiple values
     * auto result2 = exp | fxt::mtuple_append(3, 4, 5);
     * // result2 is fxt::expected<fxt::tuple<int, int, int, int, int>, Error> containing {1, 2, 3, 4, 5}
     *
     * // Pipe operator with optional<value>
     * auto opt_tuple = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(1)};
     * auto opt_val = fxt::optional<int>{2};
     * auto result3 = opt_tuple | fxt::mtuple_append(opt_val);
     * // result3 is fxt::optional<fxt::tuple<int, int>> containing {1, 2}
     *
     * // Chaining with other monadic operations
     * auto result4 = exp
     *     | fxt::mtuple_transform([](auto x) { return x * 2; })
     *     | fxt::mtuple_append(5, 6);
     * // result4 is fxt::expected<fxt::tuple<int, int, int, int>, Error> containing {2, 4, 5, 6}
     *
     * // Works with optional and flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result5 = opt | fxt::mtuple_append(3.0, 4.0);
     * // result5 is fxt::optional<fxt::flat_tuple<double, double, double, double>> containing {1.0, 2.0, 3.0, 4.0}
     * @endcode
     */
    template<typename U, typename... Us>
        requires (!expected_like<std::remove_cvref_t<U>>)
    constexpr auto mtuple_append(U&& u, Us&&... us)
    {
        // Copy-constructible types: std::as_const forces a copy → reusable adaptor.
        // Move-only types: std::move unavoidable → single-use by necessity.
        return [u = std::forward<U>(u), ... us = std::forward<Us>(us)]<typename TMonad>(TMonad&& container) mutable {
            if constexpr (std::is_copy_constructible_v<std::decay_t<U>> &&
                          (std::is_copy_constructible_v<std::decay_t<Us>> && ...)) {
                return fxt::mtuple_append(std::forward<TMonad>(container), std::as_const(u), std::as_const(us)...);
            } else {
                return fxt::mtuple_append(std::forward<TMonad>(container), std::move(u), std::move(us)...);
            }
        };
    }

}    // namespace fxt
