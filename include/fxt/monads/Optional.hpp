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
#include "../concepts/IsOptional.hpp"

#ifdef FXT_USE_TL_OPTIONAL
#    include <tl/optional.hpp>
namespace fxt
{
  template<typename T>
  using optional = tl::optional<T>;
    inline constexpr auto nullopt = tl::nullopt;
}
#else
#    include <optional>

namespace fxt
{
  template<typename T>
  using optional = std::optional<T>;
    inline constexpr auto nullopt = std::nullopt;

}

#endif

namespace fxt
{
    /**
     * @brief Pipe operator overload for const lvalue references to fxt::optional.
     *
     * Enables piping a const fxt::optional object into a callable function.
     * The function is invoked with a const reference to the optional object.
     *
     * @tparam ValueType The value type of the optional object
     * @tparam Callable The callable type that accepts const fxt::optional<ValueType>&
     *
     * @param optional The const lvalue reference to the optional object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the optional object
     *
     * @requires Callable must be invocable with const fxt::optional<ValueType>&
     *
     * Example:
     * @code
     * const fxt::optional<int> value = 42;
     * auto result = value | transform_fn | validate_fn;
     * @endcode
     */
    // template<typename ValueType, typename Callable>
    //     requires std::invocable<Callable, const fxt::optional<ValueType>&>
    // constexpr auto operator|(const fxt::optional<ValueType>& optional, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), optional))
    // {
    //     return std::invoke(std::forward<Callable>(function), optional);
    // }

    /**
     * @brief Pipe operator overload for non-const lvalue references to fxt::optional.
     *
     * Enables piping a non-const fxt::optional object into a callable function.
     * The function is invoked with a non-const lvalue reference to the optional object.
     *
     * @tparam ValueType The value type of the optional object
     * @tparam Callable The callable type that accepts fxt::optional<ValueType>&
     *
     * @param optional The non-const lvalue reference to the optional object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the optional object
     *
     * @requires Callable must be invocable with fxt::optional<ValueType>&
     *
     * Example:
     * @code
     * fxt::optional<int> value = 42;
     * auto result = value | mutating_fn | other_fn;
     * @endcode
     */
    // template<typename ValueType, typename Callable>
    //     requires std::invocable<Callable, fxt::optional<ValueType>&>
    // constexpr auto operator|(fxt::optional<ValueType>& optional, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), optional))
    // {
    //     return std::invoke(std::forward<Callable>(function), optional);
    // }

    /**
     * @brief Pipe operator overload for rvalue references to fxt::optional.
     *
     * Enables piping an rvalue fxt::optional object into a callable function.
     * The function is invoked with an rvalue reference (moved) to the optional object.
     * This overload allows for move semantics and optimal resource handling.
     *
     * @tparam ValueType The value type of the optional object
     * @tparam Callable The callable type that accepts fxt::optional<ValueType>&&
     *
     * @param optional The rvalue reference to the optional object
     * @param function The callable function to invoke
     *
     * @return The result of invoking the function with the moved optional object
     *
     * @requires Callable must be invocable with fxt::optional<ValueType>&&
     *
     * Example:
     * @code
     * auto result = make_optional() | transform_fn | validate_fn;
     * @endcode
     */
    // template<typename ValueType, typename Callable>
    //     requires std::invocable<Callable, fxt::optional<ValueType>&&>
    // constexpr auto operator|(fxt::optional<ValueType>&& optional, Callable&& function)
    //     -> decltype(std::invoke(std::forward<Callable>(function), std::move(optional)))
    // {
    //     return std::invoke(std::forward<Callable>(function), std::move(optional));
    // }


    template<typename TOptional, typename Callable>
        requires std::invocable<Callable, TOptional&&> && optional_like<std::remove_cvref_t<TOptional>>
    constexpr auto operator|(TOptional&& expected, Callable&& function)
    -> decltype(std::invoke(std::forward<Callable>(function), std::forward<TOptional>(expected)))
    {
        return std::invoke(std::forward<Callable>(function), std::forward<TOptional>(expected));
    }
}    // namespace fxt
