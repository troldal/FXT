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
 * @brief Tuple element extraction: tuple_take, tuple_take_last and their monadic lifts
 *
 * ## Core operations
 *
 * **tuple_take<X>(tuple)** — first X elements
 * - Direct call: `fxt::tuple_take<2>(tpl)`
 * - Curried:     `tpl | fxt::tuple_take<2>()`
 *
 * **tuple_take_last<X>(tuple)** — last X elements
 * - Direct call: `fxt::tuple_take_last<2>(tpl)`
 * - Curried:     `tpl | fxt::tuple_take_last<2>()`
 *
 * ## Monadic lifts (fxt::expected / fxt::optional)
 *
 * **mtuple_take<X>(container)** / **mtuple_take_last<X>(container)**
 * - Curried pipe form: `container | fxt::mtuple_take<2>()`
 * - Errors/nullopt propagate without applying the operation.
 *
 * All operations work with both fxt::tuple and fxt::flat_tuple, preserving the tuple kind.
 */

#pragma once

#include <tuple>
#include <utility>
#include "FlatTuple.hpp"
#include "TupleAppend.hpp"

namespace fxt
{
    // ========================================================================
    // fxt::tuple_take — first X elements
    // ========================================================================

    /**
     * @brief Return a new tuple containing the first @p X elements.
     *
     * @tparam X  Number of elements to keep (must be ≤ tuple size)
     * @tparam Tuple  fxt::tuple or fxt::flat_tuple (deduced)
     * @param  tpl    Source tuple (any value category)
     *
     * @code
     * auto t  = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto r  = fxt::tuple_take<2>(t);   // fxt::tuple<int, double>{1, 2.0}
     *
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto r2 = fxt::tuple_take<2>(ft);  // fxt::flat_tuple<double, double>{1.0, 2.0}
     * @endcode
     */
    template<std::size_t X, typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto tuple_take(Tuple&& tpl)
    {
        constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= N, "Cannot take more elements than the tuple size");
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return impl::make_tuple_like<Tuple>(fxt::get<Is>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<X>{});
    }

    /** @brief Curried `tuple_take` for pipeline usage: `tpl | fxt::tuple_take<2>()` */
    template<std::size_t X>
    constexpr auto tuple_take()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::tuple_take<X>(std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::tuple_take_last — last X elements
    // ========================================================================

    /**
     * @brief Return a new tuple containing the last @p X elements.
     *
     * @tparam X  Number of elements to keep (must be ≤ tuple size)
     * @tparam Tuple  fxt::tuple or fxt::flat_tuple (deduced)
     * @param  tpl    Source tuple (any value category)
     *
     * @code
     * auto t  = fxt::make_tuple(1, 2.0, "three", 'f');
     * auto r  = fxt::tuple_take_last<2>(t);   // fxt::tuple<const char*, char>{"three", 'f'}
     *
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto r2 = fxt::tuple_take_last<2>(ft);  // fxt::flat_tuple<double, double>{3.0, 4.0}
     * @endcode
     */
    template<std::size_t X, typename Tuple>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    constexpr auto tuple_take_last(Tuple&& tpl)
    {
        constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= N, "Cannot take more elements than the tuple size");
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return impl::make_tuple_like<Tuple>(fxt::get<Is + (N - X)>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<X>{});
    }

    /** @brief Curried `tuple_take_last` for pipeline usage: `tpl | fxt::tuple_take_last<2>()` */
    template<std::size_t X>
    constexpr auto tuple_take_last()
    {
        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::tuple_take_last<X>(std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::mtuple_take — monadic lift of tuple_take
    // ========================================================================

    /**
     * @brief Apply `tuple_take` to a tuple inside a monadic container (direct call).
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int,int,int,int>, Error>{fxt::make_tuple(1,2,3,4)};
     * auto r   = fxt::mtuple_take<2>(exp);
     * // fxt::expected<fxt::tuple<int,int>, Error>{1, 2}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mtuple_take(Container&& container)
    {
        return std::forward<Container>(container).transform([]<typename TValue>(TValue&& tpl) {
            constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<TValue>>;
            static_assert(X <= N, "Cannot take more elements than the tuple size");
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return impl::make_tuple_like<TValue>(fxt::get<Is>(std::forward<TValue>(tpl))...);
            }(std::make_index_sequence<X>{});
        });
    }

    /**
     * @brief Curried `mtuple_take` for pipeline usage.
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int,int,int,int>, Error>{fxt::make_tuple(1,2,3,4)};
     * auto r   = exp | fxt::mtuple_take<2>();
     * // fxt::expected<fxt::tuple<int,int>, Error>{1, 2}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mtuple_take()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mtuple_take<X>(std::forward<Container>(container));
        };
    }

    // ========================================================================
    // fxt::mtuple_take_last — monadic lift of tuple_take_last
    // ========================================================================

    /**
     * @brief Apply `tuple_take_last` to a tuple inside a monadic container (direct call).
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int,int,int,int>, Error>{fxt::make_tuple(1,2,3,4)};
     * auto r   = fxt::mtuple_take_last<2>(exp);
     * // fxt::expected<fxt::tuple<int,int>, Error>{3, 4}
     * @endcode
     */
    template<std::size_t X, typename Container>
    constexpr auto mtuple_take_last(Container&& container)
    {
        return std::forward<Container>(container).transform([]<typename TValue>(TValue&& tpl) {
            constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<TValue>>;
            static_assert(X <= N, "Cannot take more elements than the tuple size");
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
                return impl::make_tuple_like<TValue>(fxt::get<Is + (N - X)>(std::forward<TValue>(tpl))...);
            }(std::make_index_sequence<X>{});
        });
    }

    /**
     * @brief Curried `mtuple_take_last` for pipeline usage.
     *
     * @code
     * auto exp = fxt::expected<fxt::tuple<int,int,int,int>, Error>{fxt::make_tuple(1,2,3,4)};
     * auto r   = exp | fxt::mtuple_take_last<2>();
     * // fxt::expected<fxt::tuple<int,int>, Error>{3, 4}
     * @endcode
     */
    template<std::size_t X>
    constexpr auto mtuple_take_last()
    {
        return []<typename Container>(Container&& container) {
            return fxt::mtuple_take_last<X>(std::forward<Container>(container));
        };
    }

}    // namespace fxt
