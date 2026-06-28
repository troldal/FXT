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
#include <utility>
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"

namespace fxt
{
    /**
     * @brief Filter adaptor for optional_like types
     *
     * Returns the optional unchanged when it holds a value satisfying the
     * predicate. Returns an empty optional otherwise. A disengaged input is
     * propagated as-is without invoking the predicate.
     *
     * The returned callable is defined in namespace @c fxt so that
     * argument-dependent lookup finds @c fxt::operator| for @c optional_like types.
     *
     * @tparam TPredicate Callable type; must be invocable with the optional's value_type
     * @param pred Predicate to test the contained value
     * @return A callable adaptor compatible with the @c | pipe operator
     *
     * @section Usage
     * @code
     * std::optional<int> a = 4;
     * std::optional<int> b = 3;
     * std::optional<int> c = std::nullopt;
     *
     * auto even = fxt::filter([](int x) { return x % 2 == 0; });
     * auto r1 = a | even;  // std::optional<int>{4}
     * auto r2 = b | even;  // std::nullopt  (predicate fails)
     * auto r3 = c | even;  // std::nullopt  (input already empty)
     *
     * // Inline usage
     * auto r4 = a | fxt::filter([](int x) { return x > 0; });  // std::optional<int>{4}
     * @endcode
     */
    template<typename TPredicate>
    constexpr auto filter(TPredicate&& pred)
    {
        return [pred = std::forward<TPredicate>(pred)]<typename TContainer>(TContainer&& container)
            -> std::remove_cvref_t<TContainer>
            requires optional_like<std::remove_cvref_t<TContainer>>
        {
            if (container.has_value() && std::invoke(pred, *container))
                return std::forward<TContainer>(container);
            return {};
        };
    }

    /**
     * @brief Filter adaptor for expected_like types
     *
     * Returns the expected unchanged when it holds a value satisfying the
     * predicate. Returns @c unexpected(err) when the value fails the predicate.
     * An existing error is propagated as-is without invoking the predicate.
     *
     * The returned callable is defined in namespace @c fxt so that
     * argument-dependent lookup finds @c fxt::operator| for @c expected_like types.
     *
     * @tparam TPredicate Callable type; must be invocable with the expected's value_type
     * @tparam TError     Type of the error value injected when the predicate fails;
     *                    must be convertible to the expected's error_type
     * @param pred Predicate to test the contained value
     * @param err  Error value to inject when the predicate is not satisfied
     * @return A callable adaptor compatible with the @c | pipe operator
     *
     * @section Usage
     * @code
     * std::expected<int, std::string> ok  = 4;
     * std::expected<int, std::string> err = std::unexpected<std::string>{"prior error"};
     *
     * auto positive = fxt::filter([](int x) { return x > 0; }, std::string{"not positive"});
     * auto r1 = ok  | positive;   // std::expected<int, std::string>{4}
     * auto r2 = ok  | fxt::filter([](int x) { return x > 10; }, std::string{"too small"});
     *                             // std::unexpected<std::string>{"too small"}
     * auto r3 = err | positive;   // std::unexpected<std::string>{"prior error"} — propagated
     * @endcode
     */
    template<typename TPredicate, typename TError>
    constexpr auto filter(TPredicate&& pred, TError&& err)
    {
        return [pred = std::forward<TPredicate>(pred),
                err  = std::forward<TError>(err)]<typename TContainer>(TContainer&& container)
            -> std::remove_cvref_t<TContainer>
            requires expected_constructible_like<std::remove_cvref_t<TContainer>>
        {
            using Exp = std::remove_cvref_t<TContainer>;
            if (!container.has_value())
                return std::forward<TContainer>(container);
            if (std::invoke(pred, *container))
                return std::forward<TContainer>(container);
            return Exp{typename Exp::unexpected_type{err}};
        };
    }

}    // namespace fxt
