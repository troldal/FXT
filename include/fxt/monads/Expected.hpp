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

#include <functional>
#include "../concepts/IsExpected.hpp"

#ifdef FXT_USE_TL_EXPECTED
#    include <tl/expected.hpp>
namespace fxt
{
    template<typename T, typename E>
    using expected = tl::expected<T, E>;

    template<typename E>
    using unexpected = tl::unexpected<E>;
}    // namespace fxt
#else
#    include <expected>

namespace fxt
{
    template<typename T, typename E>
    using expected = std::expected<T, E>;

    template<typename E>
    using unexpected = std::unexpected<E>;
}    // namespace fxt

#endif

namespace fxt
{
    /**
     * @brief Pipe operator overload for const lvalue references to fxt::expected.
     *
     * Enables piping a const fxt::expected object into a callable function.
     * The function is invoked with a const reference to the expected object.
     *
     * @tparam ValueType The value type of the expected object
     * @tparam ErrorType The error type of the expected object
     * @tparam Callable The callable type that accepts const fxt::expected<ValueType, ErrorType>&
     *
     * @param expected The const lvalue reference to the expected object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the expected object
     *
     * @requires Callable must be invocable with const fxt::expected<ValueType, ErrorType>&
     *
     * Example:
     * @code
     * const fxt::expected<int, Error> value = 42;
     * auto result = value | transform_fn | validate_fn;
     * @endcode
     */
    // template<typename ValueType, typename ErrorType, typename Callable>
    //     requires std::invocable<Callable, const fxt::expected<ValueType, ErrorType>&>
    // constexpr auto operator|(const fxt::expected<ValueType, ErrorType>& expected, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), expected))
    // {
    //     return std::invoke(std::forward<Callable>(function), expected);
    // }

    /**
     * @brief Pipe operator overload for non-const lvalue references to fxt::expected.
     *
     * Enables piping a non-const fxt::expected object into a callable function.
     * The function is invoked with a non-const lvalue reference to the expected object.
     *
     * @tparam ValueType The value type of the expected object
     * @tparam ErrorType The error type of the expected object
     * @tparam Callable The callable type that accepts fxt::expected<ValueType, ErrorType>&
     *
     * @param expected The non-const lvalue reference to the expected object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the expected object
     *
     * @requires Callable must be invocable with fxt::expected<ValueType, ErrorType>&
     *
     * Example:
     * @code
     * fxt::expected<int, Error> value = 42;
     * auto result = value | mutating_fn | other_fn;
     * @endcode
     */
    // template<typename ValueType, typename ErrorType, typename Callable>
    //     requires std::invocable<Callable, fxt::expected<ValueType, ErrorType>&>
    // constexpr auto operator|(fxt::expected<ValueType, ErrorType>& expected, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), expected))
    // {
    //     return std::invoke(std::forward<Callable>(function), expected);
    // }

    /**
     * @brief Pipe operator overload for rvalue references to fxt::expected.
     *
     * Enables piping an rvalue fxt::expected object into a callable function.
     * The function is invoked with an rvalue reference (moved) to the expected object.
     * This overload allows for move semantics and optimal resource handling.
     *
     * @tparam ValueType The value type of the expected object
     * @tparam ErrorType The error type of the expected object
     * @tparam Callable The callable type that accepts fxt::expected<ValueType, ErrorType>&&
     *
     * @param expected The rvalue reference to the expected object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the moved expected object
     *
     * @requires Callable must be invocable with fxt::expected<ValueType, ErrorType>&&
     *
     * Example:
     * @code
     * auto result = make_expected() | transform_fn | validate_fn;
     * @endcode
     */
    // template<typename ValueType, typename ErrorType, typename Callable>
    //     requires std::invocable<Callable, fxt::expected<ValueType, ErrorType>&&>
    // constexpr auto operator|(fxt::expected<ValueType, ErrorType>&& expected, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), std::move(expected)))
    // {
    //     return std::invoke(std::forward<Callable>(function), std::move(expected));
    // }

    template<typename TExpected, typename Callable>
        requires std::invocable<Callable, TExpected> && expected_like<std::remove_cvref_t<TExpected>>
    constexpr auto operator|(TExpected&& expected, Callable&& function)
    -> decltype(std::invoke(std::forward<Callable>(function), std::forward<TExpected>(expected)))
    {
        return std::invoke(std::forward<Callable>(function), std::forward<TExpected>(expected));
    }
}    // namespace fxt
