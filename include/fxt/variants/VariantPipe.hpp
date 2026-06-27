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

#include "../concepts/IsVariant.hpp"
#include <functional>

/**
 * @brief Pipe operator for fxt::variant with callable
 *
 * Allows piping a variant to a callable function, enabling functional-style composition.
 * The callable receives the variant and returns the result.
 *
 * @tparam TVariant Type of the variant (deduced)
 * @tparam Callable Type of the callable
 * @param variant The variant to pipe
 * @param function The callable to apply to the variant
 * @return The result of invoking the callable with the variant
 *
 * @code
 * fxt::variant<int, double, std::string> v{42};
 * auto result = v | [](auto&& var) { return std::visit([](auto&& x) { return x; }, var); };
 * @endcode
 */
template<typename TVariant, typename Callable>
    requires fxt::variant_like<std::remove_cvref_t<TVariant>>
        && requires(TVariant&& v, Callable&& c) {
                 std::invoke(std::forward<Callable>(c), std::forward<TVariant>(v));
             }
constexpr auto operator|(TVariant&& variant, Callable&& function)
-> decltype(std::invoke(std::forward<Callable>(function), std::forward<TVariant>(variant)))
{
    return std::invoke(std::forward<Callable>(function), std::forward<TVariant>(variant));
}
