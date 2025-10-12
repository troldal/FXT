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

#include <tuple>
#include <concepts>

namespace fxt
{
    /**
     * @brief Alias for std::tuple
     *
     * Provides a consistent fxt namespace interface for tuple types,
     * allowing users to write fxt::tuple instead of std::tuple.
     */
    template<typename... Ts>
    using tuple = std::tuple<Ts...>;

    /**
     * @brief Create a tuple, deducing the target type from the types of arguments
     *
     * Forwards to std::make_tuple. Provides a consistent fxt::make_tuple
     * interface that matches fxt::tuple.
     *
     * @tparam Ts Types of the elements
     * @param args Values to initialize the tuple with
     * @return A tuple containing the given values
     *
     * @code
     * auto t = fxt::make_tuple(42, 3.14, "hello");
     * // Creates fxt::tuple<int, double, const char*>
     * @endcode
     */
    template<typename... Ts>
    constexpr auto make_tuple(Ts&&... args)
    {
        return std::make_tuple(std::forward<Ts>(args)...);
    }

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
    template<std::size_t I, typename... Ts>
    constexpr auto& get(tuple<Ts...>& t) noexcept
    {
        return std::get<I>(t);
    }

    template<std::size_t I, typename... Ts>
    constexpr const auto& get(const tuple<Ts...>& t) noexcept
    {
        return std::get<I>(t);
    }

    template<std::size_t I, typename... Ts>
    constexpr auto&& get(tuple<Ts...>&& t) noexcept
    {
        return std::get<I>(std::move(t));
    }

    template<std::size_t I, typename... Ts>
    constexpr const auto&& get(const tuple<Ts...>&& t) noexcept
    {
        return std::get<I>(std::move(t));
    }
}    // namespace fxt


    /**
     * @brief Pipe operator for fxt::tuple with callable (lvalue reference)
     *
     * Allows piping a tuple to a callable function, enabling functional-style composition.
     * The callable receives the tuple and returns the result.
     *
     * @tparam Ts Types in the tuple
     * @tparam Callable Type of the callable
     * @param tuple The tuple to pipe
     * @param callable The callable to apply to the tuple
     * @return The result of invoking the callable with the tuple
     *
     * @code
     * fxt::tuple<int, double> t{42, 3.14};
     * auto result = t | fxt::get<0>;  // Returns 42
     * @endcode
     */
    template<typename... Ts, typename Callable>
    requires requires(fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(fxt::tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::tuple with callable (const lvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(const fxt::tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::tuple with callable (rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(fxt::tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

    /**
     * @brief Pipe operator for fxt::tuple with callable (const rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(const fxt::tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }
