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
     * @brief Concatenate two fxt::tuple objects
     *
     * For fxt::tuple (std::tuple), this is an alias to std::tuple_cat.
     *
     * @tparam Ts Types in the first tuple
     * @tparam Us Types in the second tuple
     * @param t1 First tuple
     * @param t2 Second tuple
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

    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(tuple<Ts...>&& t1, tuple<Us...>&& t2)
    {
        return std::tuple_cat(std::move(t1), std::move(t2));
    }

    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const tuple<Ts...>& t1, tuple<Us...>&& t2)
    {
        return std::tuple_cat(t1, std::move(t2));
    }

    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(tuple<Ts...>&& t1, const tuple<Us...>& t2)
    {
        return std::tuple_cat(std::move(t1), t2);
    }

    /**
     * @brief Concatenate two fxt::flat_tuple objects
     *
     * Creates a new flat_tuple containing all elements from both input tuples.
     *
     * @tparam Ts Types in the first flat_tuple
     * @tparam Us Types in the second flat_tuple
     * @param t1 First flat_tuple
     * @param t2 Second flat_tuple
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

    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(flat_tuple<Ts...>&& t1, flat_tuple<Us...>&& t2)
    {
        return impl::flat_tuple_cat_impl(
            std::move(t1), std::move(t2),
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

    template<typename... Ts, typename... Us>
    constexpr auto tuple_cat(const flat_tuple<Ts...>& t1, flat_tuple<Us...>&& t2)
    {
        return impl::flat_tuple_cat_impl(
            t1, std::move(t2),
            std::index_sequence_for<Ts...>{},
            std::index_sequence_for<Us...>{}
        );
    }

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
     * Returns a lambda that concatenates the given tuple with another tuple.
     * This enables usage with the pipe operator.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the type.
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
     * // Also works with flat_tuple
     * auto ft1 = fxt::make_flat_tuple(1, 2);
     * auto ft2 = fxt::make_flat_tuple(3.0, 4.0);
     * auto result2 = ft1 | fxt::tuple_cat(ft2);
     * // result2 is fxt::flat_tuple<int, int, double, double>{1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename Tuple2>
    requires fxt::tuple_like<std::remove_cvref_t<Tuple2>>
    constexpr auto tuple_cat(Tuple2&& t2)
    {
        return [t2 = std::forward<Tuple2>(t2)](auto&& t1) mutable {
            return fxt::tuple_cat(std::forward<decltype(t1)>(t1), std::move(t2));
        };
    }

    // ========================================================================
    // fxt::mtuple_cat - Monadic tuple concatenation
    // ========================================================================

    /**
     * @brief Concatenate a tuple inside a monad with another tuple (direct call)
     *
     * Takes a monad (fxt::expected or fxt::optional) containing a tuple and another tuple
     * (which may or may not be inside a monad), and concatenates them. If the second argument
     * is also in a monad, both values are extracted and concatenated if both are valid.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     *
     * @tparam Container1 The monad type containing the first tuple
     * @tparam Tuple2 The second tuple (or monad containing a tuple)
     * @param container The monad containing the first tuple
     * @param t2 The second tuple (or monad containing the second tuple)
     * @return A monad containing the concatenated tuple
     *
     * @section Usage
     * @code
     * // Concatenate monad<tuple> with tuple
     * auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto t2 = fxt::make_tuple(3.0, 4.0);
     * auto result = fxt::mtuple_cat(exp, t2);
     * // result is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Concatenate monad<tuple> with monad<tuple>
     * auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto exp2 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::make_tuple(3.0, 4.0)};
     * auto result2 = fxt::mtuple_cat(exp1, exp2);
     * // result2 is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}
     *
     * // Works with flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
     * auto ft2 = fxt::make_flat_tuple(3.0, 4.0);
     * auto result3 = fxt::mtuple_cat(opt, ft2);
     * // result3 is fxt::optional<fxt::flat_tuple<int, int, double, double>> containing {1, 2, 3.0, 4.0}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result4 = fxt::mtuple_cat(exp_err, fxt::make_tuple(3.0, 4.0));
     * // result4 is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing the error
     * @endcode
     */
    template<typename Container1, typename Tuple2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::tuple_like<std::remove_cvref_t<Tuple2>>
    constexpr auto mtuple_cat(Container1&& container, Tuple2&& t2)
    {
        return std::forward<Container1>(container).transform([t2 = std::forward<Tuple2>(t2)](auto&& t1) mutable {
            return fxt::tuple_cat(std::forward<decltype(t1)>(t1), std::move(t2));
        });
    }

    /**
     * @brief Concatenate two tuples inside monads (direct call)
     *
     * Takes two monads (fxt::expected or fxt::optional) each containing a tuple,
     * and concatenates them if both monads contain values.
     *
     * @tparam Container1 The first monad type containing a tuple
     * @tparam Container2 The second monad type containing a tuple
     * @param container1 The first monad containing a tuple
     * @param container2 The second monad containing a tuple
     * @return A monad containing the concatenated tuple if both inputs are valid
     */
    template<typename Container1, typename Container2>
    requires fxt::monad_like<std::remove_cvref_t<Container1>> && fxt::monad_like<std::remove_cvref_t<Container2>>
    constexpr auto mtuple_cat(Container1&& container1, Container2&& container2)
    {
        return std::forward<Container1>(container1).and_then([container2 = std::forward<Container2>(container2)](auto&& t1) mutable {
            return std::forward<Container2>(container2).transform([t1 = std::forward<decltype(t1)>(t1)](auto&& t2) mutable {
                return fxt::tuple_cat(std::move(t1), std::forward<decltype(t2)>(t2));
            });
        });
    }

    /**
     * @brief Curried version of mtuple_cat for pipelining
     *
     * Returns a lambda that concatenates a tuple inside a monad with another tuple.
     * This enables usage with the pipe operator for monadic tuple concatenation.
     *
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the tuple type.
     * The second argument can be a plain tuple or a tuple inside a monad.
     *
     * @tparam Tuple2 Type of the second tuple (or monad containing a tuple)
     * @param t2 The second tuple (or monad containing the second tuple)
     * @return A lambda that takes a monad<tuple> and returns a monad<concatenated_tuple>
     *
     * @section Usage
     * @code
     * // Pipe operator with monad<tuple> and tuple
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
     * // Chaining with other monadic operations
     * auto exp3 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result3 = exp3
     *     | fxt::mtransform_tuple([](auto x) { return x * 2; })
     *     | fxt::mtuple_cat(fxt::make_tuple(5.0, 6.0));
     * // result3 is fxt::expected<fxt::tuple<int, int, double, double>, Error> containing {2, 4, 5.0, 6.0}
     *
     * // Works with optional
     * auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
     * auto result4 = opt | fxt::mtuple_cat(fxt::make_flat_tuple(3.0, 4.0));
     * // result4 is fxt::optional<fxt::flat_tuple<int, int, double, double>> containing {1, 2, 3.0, 4.0}
     * @endcode
     */
    template<typename Tuple2>
    constexpr auto mtuple_cat(Tuple2&& t2)
    {
        return [t2 = std::forward<Tuple2>(t2)](auto&& container) mutable {
            return fxt::mtuple_cat(std::forward<decltype(container)>(container), std::move(t2));
        };
    }

} // namespace fxt
