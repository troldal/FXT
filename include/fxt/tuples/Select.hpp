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
#include "FlatTuple.hpp"
#include "Tuple.hpp"
#include "TupleAppend.hpp"
#include <cstddef>
#include <tuple>

namespace fxt
{

    /**
     * @brief Extract multiple elements from a tuple inside a monadic container by index
     *
     * Creates a function that extracts multiple elements at the specified indices from a tuple
     * contained within an fxt::expected or fxt::optional object, or directly from fxt::tuple
     * or fxt::flat_tuple. The extracted elements are combined into a new tuple of the same type,
     * preserving the container type (expected/optional) or tuple type (fxt::tuple/fxt::flat_tuple).
     *
     * @tparam Is The indices of the elements to extract from the tuple
     * @return A function that transforms a monadic container<tuple> or tuple to extract elements at specified indices
     *
     * @throws static_assert If no indices are provided
     *
     * @example
     *   // With expected containing fxt::tuple
     *   auto exp = fxt::expected<fxt::tuple<int, double, std::string>, Error>{fxt::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::mselect<0, 2>();  // result contains fxt::tuple<int, std::string>{1, "three"}
     *
     *   // With expected containing fxt::flat_tuple
     *   auto exp2 = fxt::expected<fxt::flat_tuple<int, double, std::string>, Error>{fxt::make_flat_tuple(1, 2.0, "three")};
     *   auto result2 = exp2 | fxt::mselect<0, 2>();  // result contains fxt::flat_tuple<int, std::string>{1, "three"}
     *
     *   // With optional containing fxt::tuple
     *   auto opt = fxt::optional<fxt::tuple<int, double, std::string>>{fxt::make_tuple(1, 2.0, "three")};
     *   auto result3 = opt | fxt::mselect<0, 2>();  // result contains fxt::tuple<int, std::string>{1, "three"}
     *
     *   // With optional containing fxt::flat_tuple
     *   auto opt2 = fxt::optional<fxt::flat_tuple<int, double, std::string>>{fxt::make_flat_tuple(1, 2.0, "three")};
     *   auto result4 = opt2 | fxt::mselect<0, 2>();  // result contains fxt::flat_tuple<int, std::string>{1, "three"}
     *
     *   // Direct with fxt::tuple
     *   auto t = fxt::make_tuple(1, 2.0, "three");
     *   auto result5 = t | fxt::mselect<0, 2>();  // result is fxt::tuple<int, std::string>{1, "three"}
     *
     *   // Direct with fxt::flat_tuple
     *   auto ft = fxt::make_flat_tuple(1, 2.0, "three");
     *   auto result6 = ft | fxt::mselect<0, 2>();  // result is fxt::flat_tuple<int, std::string>{1, "three"}
     */
    template<size_t... Is>
    auto mselect()
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");

        return overload{
            // Handle expected-like containers with fxt::tuple
            []<typename... TArgs, typename TError>(const fxt::expected<fxt::tuple<TArgs...>, TError>& container) {
                return container.transform([](const fxt::tuple<TArgs...>& t) {
                    return fxt::make_tuple(fxt::get<Is>(t)...);
                });
            },
            // Handle expected-like containers with fxt::flat_tuple
            []<typename... TArgs, typename TError>(const fxt::expected<fxt::flat_tuple<TArgs...>, TError>& container) {
                return container.transform([](const fxt::flat_tuple<TArgs...>& t) {
                    return fxt::make_flat_tuple(fxt::get<Is>(t)...);
                });
            },
            // Handle optional-like containers with fxt::tuple
            []<typename... TArgs>(const fxt::optional<fxt::tuple<TArgs...>>& container) {
                return container.transform([](const fxt::tuple<TArgs...>& t) {
                    return fxt::make_tuple(fxt::get<Is>(t)...);
                });
            },
            // Handle optional-like containers with fxt::flat_tuple
            []<typename... TArgs>(const fxt::optional<fxt::flat_tuple<TArgs...>>& container) {
                return container.transform([](const fxt::flat_tuple<TArgs...>& t) {
                    return fxt::make_flat_tuple(fxt::get<Is>(t)...);
                });
            },
            // Handle fxt::tuple (lvalue reference)
            []<typename... TArgs>(fxt::tuple<TArgs...>& tuple) {
                return fxt::make_tuple(fxt::get<Is>(tuple)...);
            },
            // Handle fxt::tuple (const lvalue reference)
            []<typename... TArgs>(const fxt::tuple<TArgs...>& tuple) {
                return fxt::make_tuple(fxt::get<Is>(tuple)...);
            },
            // Handle fxt::tuple (rvalue reference)
            []<typename... TArgs>(fxt::tuple<TArgs...>&& tuple) {
                return fxt::make_tuple(fxt::get<Is>(std::move(tuple))...);
            },
            // Handle fxt::flat_tuple (lvalue reference)
            []<typename... TArgs>(fxt::flat_tuple<TArgs...>& tuple) {
                return fxt::make_flat_tuple(fxt::get<Is>(tuple)...);
            },
            // Handle fxt::flat_tuple (const lvalue reference)
            []<typename... TArgs>(const fxt::flat_tuple<TArgs...>& tuple) {
                return fxt::make_flat_tuple(fxt::get<Is>(tuple)...);
            },
            // Handle fxt::flat_tuple (rvalue reference)
            []<typename... TArgs>(fxt::flat_tuple<TArgs...>&& tuple) {
                return fxt::make_flat_tuple(fxt::get<Is>(std::move(tuple))...);
            }
        };
    }

    /**
     * @brief Extract multiple elements from a tuple inside a monadic container by type
     *
     * Creates a function that extracts multiple elements of the specified types from a tuple
     * contained within an fxt::expected or fxt::optional object, or directly from fxt::tuple.
     * The extracted elements are combined into a new tuple of the same type,
     * preserving the container type (expected/optional) or tuple type (fxt::tuple).
     *
     * Note: This function only works with fxt::tuple, not fxt::flat_tuple, because
     * fxt::flat_tuple does not support get by type.
     *
     * @tparam Ts The types of the elements to extract from the tuple
     * @return A function that transforms a monadic container<tuple> or tuple to extract elements of specified types
     *
     * @throws static_assert If no types are provided
     * @note The tuple must contain exactly one element of each specified type, otherwise std::get will fail
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<fxt::tuple<int, double, std::string>, Error>{fxt::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::mselect<int, std::string>();  // result contains fxt::tuple<int, std::string>{1, "three"}
     *
     *   // With optional
     *   auto opt = fxt::optional<fxt::tuple<int, double, std::string>>{fxt::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::mselect<int, std::string>();  // result contains fxt::tuple<int, std::string>{1, "three"}
     *
     *   // With fxt::tuple (note: uses std::get by type)
     *   auto t = fxt::make_tuple(1, 2.0, "three");
     *   auto result = t | fxt::mselect<int, const char*>();  // result is fxt::tuple<int, const char*>
     */
    template<typename... Ts>
    auto mselect()
    {
        static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");

        return overload{
            // Handle expected-like containers with fxt::tuple
            []<typename... TArgs, typename TError>(const fxt::expected<fxt::tuple<TArgs...>, TError>& container) {
                return container.transform([](const fxt::tuple<TArgs...>& t) {
                    return fxt::make_tuple(std::get<Ts>(t)...);
                });
            },
            // Handle optional-like containers with fxt::tuple
            []<typename... TArgs>(const fxt::optional<fxt::tuple<TArgs...>>& container) {
                return container.transform([](const fxt::tuple<TArgs...>& t) {
                    return fxt::make_tuple(std::get<Ts>(t)...);
                });
            },
            // Handle fxt::tuple (lvalue reference) - uses std::get by type
            []<typename... TArgs>(fxt::tuple<TArgs...>& tuple) {
                return fxt::make_tuple(std::get<Ts>(tuple)...);
            },
            // Handle fxt::tuple (const lvalue reference)
            []<typename... TArgs>(const fxt::tuple<TArgs...>& tuple) {
                return fxt::make_tuple(std::get<Ts>(tuple)...);
            },
            // Handle fxt::tuple (rvalue reference)
            []<typename... TArgs>(fxt::tuple<TArgs...>&& tuple) {
                return fxt::make_tuple(std::get<Ts>(std::move(tuple))...);
            }
            // Note: fxt::flat_tuple does not support get by type, only by index
        };
    }

    // ===== Direct select functions (non-monadic) =====

    /**
     * @brief Extract multiple elements from a tuple by index (direct call)
     *
     * This function extracts multiple elements at the specified indices from fxt::tuple
     * or fxt::flat_tuple and returns a new tuple of the same type with the selected elements.
     * Perfect forwarding is preserved for the tuple elements.
     *
     * @tparam Is The indices of the elements to extract from the tuple
     * @tparam Tuple The tuple type (deduced)
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing only the selected elements
     *
     * @throws static_assert If no indices are provided
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2.0, "three", 'f', true);
     * auto result = fxt::select<0, 2, 4>(t);
     * // result is fxt::tuple<int, const char*, bool>{1, "three", true}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = fxt::select<0, 3>(ft);
     * // result2 is fxt::flat_tuple<double, double>{1.0, 4.0}
     * @endcode
     */
    template<size_t... Is, typename Tuple>
        requires (impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>> ||
                  impl::is_flat_tuple_v<std::remove_cvref_t<Tuple>>)
    constexpr auto select(Tuple&& tpl)
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");

        if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>>) {
            return fxt::make_tuple(fxt::get<Is>(std::forward<Tuple>(tpl))...);
        } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<Tuple>>) {
            return fxt::make_flat_tuple(fxt::get<Is>(std::forward<Tuple>(tpl))...);
        }
    }

    /**
     * @brief Extract multiple elements from a tuple by index (curried version for pipeline)
     *
     * Returns a lambda that extracts multiple elements at the specified indices from a tuple.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam Is The indices of the elements to extract from the tuple
     * @return A lambda that takes a tuple and returns a new tuple with the selected elements
     *
     * @throws static_assert If no indices are provided
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3, 4, 5);
     * auto result = t | fxt::select<0, 2, 4>();
     * // result is fxt::tuple<int, int, int>{1, 3, 5}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0);
     * auto result2 = ft | fxt::select<1, 3>();
     * // result2 is fxt::flat_tuple<double, double>{2.0, 4.0}
     *
     * // Chaining with other operations
     * auto result3 = fxt::make_tuple(10, 20, 30, 40, 50)
     *              | fxt::drop<1>()
     *              | fxt::select<0, 2>();
     * // result3 is fxt::tuple<int, int>{20, 40}
     * @endcode
     */
    template<size_t... Is>
    constexpr auto select()
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");

        return []<typename Tuple>(Tuple&& tpl) {
            return fxt::select<Is...>(std::forward<Tuple>(tpl));
        };
    }

    /**
     * @brief Extract multiple elements from an fxt::tuple by type (direct call)
     *
     * This function extracts multiple elements of the specified types from fxt::tuple
     * and returns a new fxt::tuple with the selected elements. Uses std::get by type,
     * so each type must appear exactly once in the tuple.
     *
     * Note: This function only works with fxt::tuple, not fxt::flat_tuple, because
     * fxt::flat_tuple does not support get by type.
     *
     * @tparam Ts The types of the elements to extract from the tuple
     * @tparam Args The tuple element types (deduced)
     * @param tpl The input fxt::tuple (lvalue or rvalue reference)
     * @return A new fxt::tuple containing only the selected elements
     *
     * @throws static_assert If no types are provided
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(42, 3.14, "hello", 'x');
     * auto result = fxt::select<int, char>(t);
     * // result is fxt::tuple<int, char>{42, 'x'}
     *
     * // With rvalue
     * auto result2 = fxt::select<double, const char*>(fxt::make_tuple(1, 2.5, "test"));
     * // result2 is fxt::tuple<double, const char*>{2.5, "test"}
     * @endcode
     */
    template<typename... Ts, typename... Args>
    constexpr auto select(fxt::tuple<Args...>& tpl)
    {
        static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");
        return fxt::make_tuple(std::get<Ts>(tpl)...);
    }

    template<typename... Ts, typename... Args>
    constexpr auto select(const fxt::tuple<Args...>& tpl)
    {
        static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");
        return fxt::make_tuple(std::get<Ts>(tpl)...);
    }

    template<typename... Ts, typename... Args>
    constexpr auto select(fxt::tuple<Args...>&& tpl)
    {
        static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");
        return fxt::make_tuple(std::get<Ts>(std::move(tpl))...);
    }

    /**
     * @brief Extract multiple elements from an fxt::tuple by type (curried version for pipeline)
     *
     * Returns a lambda that extracts multiple elements of the specified types from an fxt::tuple.
     * This overload enables pipeline-style usage with the pipe operator.
     * Uses std::get by type, so each type must appear exactly once in the tuple.
     *
     * Note: This function only works with fxt::tuple, not fxt::flat_tuple, because
     * fxt::flat_tuple does not support get by type.
     *
     * @tparam Ts The types of the elements to extract from the tuple
     * @return A lambda that takes an fxt::tuple and returns a new tuple with the selected elements
     *
     * @throws static_assert If no types are provided
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(42, 3.14, "hello", 'x');
     * auto result = t | fxt::select<int, double>();
     * // result is fxt::tuple<int, double>{42, 3.14}
     *
     * // Chaining with other operations
     * auto result2 = fxt::make_tuple(1, 2.5, "test", true)
     *              | fxt::select<double, bool>();
     * // result2 is fxt::tuple<double, bool>{2.5, true}
     * @endcode
     */
    template<typename... Ts>
        requires (sizeof...(Ts) >= 1)
    constexpr auto select()
    {
        return []<typename... Args>(auto&& tpl) -> fxt::tuple<Ts...> {
            return fxt::make_tuple(std::get<Ts>(std::forward<decltype(tpl)>(tpl))...);
        };
    }

}    // namespace fxt
