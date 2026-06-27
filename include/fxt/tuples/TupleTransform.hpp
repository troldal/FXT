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
 * @file TupleTransform.hpp
 * @brief Tuple transformation operations for fxt::tuple and fxt::flat_tuple
 *
 * This file provides functions for transforming tuple elements by applying callables to each element,
 * in both plain and monadic contexts. All operations support both fxt::tuple and fxt::flat_tuple,
 * preserving the tuple type through the operations.
 *
 * ## Main Functions
 *
 * **tuple_transform(callable, tuple)** - Transforms each element of a tuple by applying a callable
 * - Direct call: `tuple_transform(callable, tuple)` returns transformed tuple
 * - Curried: `tuple | tuple_transform(callable)` enables pipeline usage
 * - Can change element types (e.g., int to string)
 *
 * **mtuple_transform(callable, monad<tuple>)** - Transforms tuple elements inside fxt::expected or fxt::optional
 * - Direct call: `mtuple_transform(callable, monad)` returns monad with transformed tuple
 * - Curried: `monad | mtuple_transform(callable)` enables pipeline usage
 * - Automatically propagates errors/nullopt without applying transformation
 *
 * ## Key Features
 * - Type-preserving: Maintains tuple type (fxt::tuple or fxt::flat_tuple)
 * - Type-transforming: Can change element types during transformation
 * - Pipeline-friendly: Curried versions enable fluent chaining with operator|
 * - Monadic: Handles error/nullopt propagation automatically
 * - Perfect forwarding: Preserves value categories through transformations
 *
 * ## Examples
 *
 * @code
 * // Basic transformation
 * auto t = fxt::tuple{1, 2, 3};
 * auto result = fxt::tuple_transform([](auto x) { return x * 2; }, t);
 * // result is fxt::tuple<int, int, int>{2, 4, 6}
 *
 * // Pipeline usage with type change
 * auto result2 = fxt::tuple{1, 2, 3}
 *     | fxt::tuple_transform([](auto x) { return std::to_string(x); })
 *     | fxt::tuple_take<2>();
 * // result2 is fxt::tuple<std::string, std::string>{"1", "2"}
 *
 * // Monadic transformation with error propagation
 * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::tuple{1, 2, 3}};
 * auto result3 = exp | fxt::mtuple_transform([](auto x) { return x * 2; });
 * // result3 is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
 *
 * // Error is propagated without applying transformation
 * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
 * auto result4 = exp_err | fxt::mtuple_transform([](auto x) { return x * 2; });
 * // result4 contains the error, transformation not applied
 *
 * // Chaining operations
 * auto result5 = fxt::tuple{1, 2, 3, 4}
 *     | fxt::tuple_transform([](auto x) { return x * 2; })
 *     | fxt::tuple_drop<1>()
 *     | fxt::tuple_transform([](auto x) { return x + 1; });
 * // result5 is fxt::tuple<int, int, int>{5, 7, 9}
 * @endcode
 *
 * @see fxt::tuple
 * @see fxt::flat_tuple
 * @see fxt::expected
 * @see fxt::optional
 */


#pragma once

#include "FlatTuple.hpp"
#include "../concepts/IsTuple.hpp"
#include <utility>
#include <type_traits>

namespace fxt
{
    /**
     * @brief Transform each element of a tuple by applying a callable (direct call)
     *
     * Applies the given callable to each element in the input tuple and returns a new tuple
     * of the same type (fxt::tuple or fxt::flat_tuple) containing the transformed results.
     * The callable is invoked with each element, and the return values form the new tuple.
     *
     * @tparam F The callable type (deduced)
     * @tparam Tuple The tuple type (deduced)
     * @param f The callable to apply to each element
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing the transformed elements
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto result = fxt::tuple_transform([](auto x) { return x * 2; }, t);
     * // result is fxt::tuple<int, int, int>{2, 4, 6}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = fxt::tuple_transform([](auto x) { return x + 1.0; }, ft);
     * // result2 is fxt::flat_tuple<double, double, double>{2.0, 3.0, 4.0}
     *
     * // With different return types
     * auto t2 = fxt::make_tuple(1, 2, 3);
     * auto result3 = fxt::tuple_transform([](auto x) { return std::to_string(x); }, t2);
     * // result3 is fxt::tuple<std::string, std::string, std::string>
     * @endcode
     */
    // Argument order follows std::apply(f, t) — function first, then the container.
    // tuple_append / mtuple_append use the opposite order (tuple/container first) because
    // their primary form is the curried adaptor built around the appended values, not
    // around the container; the direct-call mirrors that choice. The two conventions
    // are deliberately kept consistent within their own families rather than forced
    // into a single global rule.
    template<typename F, typename Tuple>
        requires tuple_like<std::remove_cvref_t<Tuple>>
    constexpr auto tuple_transform(F&& f, Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return impl::make_tuple_like<Tuple>(
                std::invoke(std::forward<F>(f), fxt::get<Indices>(std::forward<Tuple>(tpl)))...);
        }(std::make_index_sequence<tupleSize>{});
    }

    /**
     * @brief Transform each element of a tuple by applying a callable (curried version for pipeline)
     *
     * Returns a lambda that applies the given callable to each element of a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam F The callable type (deduced)
     * @param f The callable to apply to each element
     * @return A lambda that takes a tuple and returns a new tuple with transformed elements
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto result = t | fxt::tuple_transform([](auto x) { return x * 2; });
     * // result is fxt::tuple<int, int, int>{2, 4, 6}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = ft | fxt::tuple_transform([](auto x) { return x + 1.0; });
     * // result2 is fxt::flat_tuple<double, double, double>{2.0, 3.0, 4.0}
     *
     * // Chaining with other operations
     * auto t2 = fxt::make_tuple(1, 2, 3, 4);
     * auto result3 = t2
     *     | fxt::tuple_transform([](auto x) { return x * 2; })
     *     | fxt::tuple_take<2>();
     * // result3 is fxt::tuple<int, int>{2, 4}
     * @endcode
     */
    template<typename F>
    constexpr auto tuple_transform(F&& f)
    {
        return [f = std::forward<F>(f)]<typename Tuple>(Tuple&& tpl) {
            return fxt::tuple_transform(f, std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::mtuple_transform - Monadic transform for tuples
    // ========================================================================

    /**
     * @brief Transform each element of a tuple inside a monad by applying a callable (direct call)
     *
     * Applies the given callable to each element of a tuple contained within an fxt::expected
     * or fxt::optional object. Returns a monad of the same type containing the transformed tuple.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam F The callable type (deduced)
     * @tparam Container The monad type containing a tuple (deduced)
     * @param f The callable to apply to each tuple element
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the transformed tuple
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result = fxt::mtuple_transform([](auto x) { return x * 2; }, exp);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result2 = fxt::mtuple_transform([](auto x) { return x + 1.0; }, opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // With different return types
     * auto exp2 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result3 = fxt::mtuple_transform([](auto x) { return std::to_string(x); }, exp2);
     * // result3 is fxt::expected<fxt::tuple<std::string, std::string>, Error>
     * @endcode
     */
    template<typename F, typename Container>
    constexpr auto mtuple_transform(F&& f, Container&& container)
    {
        return std::forward<Container>(container).transform([f = std::forward<F>(f)](auto&& tpl) {
            return fxt::tuple_transform(f, std::forward<decltype(tpl)>(tpl));
        });
    }

    /**
     * @brief Transform each element of a tuple inside a monad by applying a callable (curried version)
     *
     * Returns a lambda that transforms each element of a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with fxt::expected or fxt::optional containing either fxt::tuple or fxt::flat_tuple.
     *
     * @tparam F The callable type (deduced)
     * @param f The callable to apply to each tuple element
     * @return A lambda that takes a monad<tuple> and returns a monad<transformed_tuple>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result = exp | fxt::mtuple_transform([](auto x) { return x * 2; });
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result2 = opt | fxt::mtuple_transform([](auto x) { return x + 1.0; });
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result3 = exp2
     *     | fxt::mtuple_transform([](auto x) { return x * 2; })
     *     | fxt::mselect<0, 2>();
     * // result3 is fxt::expected<fxt::tuple<int, int>, Error> containing {2, 6}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result4 = exp_err | fxt::mtuple_transform([](auto x) { return x * 2; });
     * // result4 is fxt::expected<fxt::tuple<int, int>, Error> containing the error
     *
     * // None propagation
     * auto opt_none = fxt::optional<fxt::tuple<int, int>>{};
     * auto result5 = opt_none | fxt::mtuple_transform([](auto x) { return x * 2; });
     * // result5 is fxt::optional<fxt::tuple<int, int>> containing nullopt
     * @endcode
     */
    template<typename F>
    constexpr auto mtuple_transform(F&& f)
    {
        return [f = std::forward<F>(f)]<typename Container>(Container&& container) {
            return fxt::mtuple_transform(f, std::forward<Container>(container));
        };
    }

} // namespace fxt
