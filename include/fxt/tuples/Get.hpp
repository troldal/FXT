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

#include "../monads/Expected.hpp"
#include "../monads/Optional.hpp"
#include "../utils/Overload.hpp"
#include <tuple>

namespace fxt
{

    //template<class... Ts> void get(std::tuple<Ts...>&) = delete;
    //template<class... Ts> void get(const std::tuple<Ts...>&) = delete;
    //template<class... Ts> void get(std::tuple<Ts...>&&) = delete;



    /**
     * @brief Get element from tuple by index
     *
     * Forwards to std::get for std::tuple. Provides a consistent fxt::get
     * interface that works with both fxt::tuple and fxt::flat_tuple.
     *
     * @tparam I Index of the element to get
     * @param t Tuple to get element from
     * @return Reference to the element at index I
     */
    // template<std::size_t I, typename... Ts>
    // constexpr auto& get(tuple<Ts...>& t) noexcept
    // {
    //     return std::get<I>(t);
    // }
    //
    // template<std::size_t I, typename... Ts>
    // constexpr const auto& get(const tuple<Ts...>& t) noexcept
    // {
    //     return std::get<I>(t);
    // }
    //
    // template<std::size_t I, typename... Ts>
    // constexpr auto&& get(tuple<Ts...>&& t) noexcept
    // {
    //     return std::get<I>(std::move(t));
    // }
    //
    // template<std::size_t I, typename... Ts>
    // constexpr const auto&& get(const tuple<Ts...>&& t) noexcept
    // {
    //     return std::get<I>(std::move(t));
    // }
    //
    // // Free functions for element access in fxt namespace (similar to std::get for std::tuple)
    // template<size_t I, class... Ts>
    // auto& get(flat_tuple<Ts...>& tuple) {
    //     return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    // }
    //
    // template<size_t I, class... Ts>
    // const auto& get(const flat_tuple<Ts...>& tuple) {
    //     return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    // }
    //
    // template<size_t I, class... Ts>
    // auto&& get(flat_tuple<Ts...>&& tuple) {
    //     return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    // }
    //
    // template<size_t I, class... Ts>
    // const auto&& get(const flat_tuple<Ts...>&& tuple) {
    //     return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    // }

    using std::get;

    // Free functions for element access on fxt::flat_tuple (not covered by std::get)
    template<size_t I, class... Ts>
    auto& get(flat_tuple<Ts...>& tuple) {
        return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    }

    template<size_t I, class... Ts>
    const auto& get(const flat_tuple<Ts...>& tuple) {
        return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    }

    template<size_t I, class... Ts>
    auto&& get(flat_tuple<Ts...>&& tuple) {
        return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    }

    template<size_t I, class... Ts>
    const auto&& get(const flat_tuple<Ts...>&& tuple) {
        return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    }

    /**
     * @brief Get element at the specified index from a tuple inside a monadic container
     *
     * Creates a function that extracts the element at the Ith position of a tuple contained
     * within an fxt::expected or fxt::optional object. The result is wrapped in a new container
     * of the same type, preserving the error handling or empty state semantics.
     *
     * @tparam I The index of the element to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract the Ith element
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::get<0>();  // result contains 1
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::get<0>();  // result contains 1
     */
    template<size_t I>
    auto mget()
    {
        return overload { // Handle expected-like containers
                          []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                              return tuple.transform([](const TTuple& t) { return fxt::get<I>(t); });
                          },
                          // Handle optional-like containers
                          []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                              return tuple.transform([](const TTuple& t) { return fxt::get<I>(t); });
                          }
        };
    }

    /**
     * @brief Get element of the specified type from a tuple inside a monadic container
     *
     * Creates a function that extracts the element of type T from a tuple contained
     * within an fxt::expected or fxt::optional object. The result is wrapped in a new
     * container of the same type, preserving the error handling or empty state semantics.
     *
     * @tparam T The type of the element to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract the element of type T
     *
     * @note The tuple must contain exactly one element of type T, otherwise std::get will fail
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::get<std::string>();  // result contains "three"
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::get<std::string>();  // result contains "three"
     */
    template<typename T>
    auto mget()
    {
        return overload { // Handle expected-like containers
                          []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                              return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
                          },
                          // Handle optional-like containers
                          []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                              return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
                          }
        };
    }

}    // namespace fxt
