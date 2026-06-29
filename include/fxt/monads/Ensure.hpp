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

namespace fxt
{
    /**
     * @brief Assertion/precondition adaptor for @c expected_like types.
     *
     * Reads as "ensure this holds, or fail with this error."  When used in a
     * pipeline it acts as a gate: a value that satisfies @p pred passes through
     * unchanged; one that does not is replaced by @c unexpected(err).  A
     * pre-existing error is propagated as-is without ever invoking the predicate.
     *
     * @c fxt::ensure differs from @c fxt::filter(pred, err) in intent rather
     * than mechanics: @c filter expresses data selection, @c ensure expresses a
     * precondition or validation rule.  @c ensure intentionally has no
     * single-argument overload for @c optional_like types — a failed assertion
     * always needs a diagnostic reason, so the error argument is mandatory and
     * the result type is always @c expected_like.
     *
     * @tparam TPredicate Callable invocable with the expected's @c value_type
     * @tparam TError     Type of the injected error; must be convertible to the
     *                    expected's @c error_type
     * @param pred  Predicate to evaluate against the contained value
     * @param err   Error value to inject when the predicate is not satisfied
     * @return A callable adaptor compatible with the @c | pipe operator
     *
     * @section Usage
     * @code
     * fxt::expected<int, std::string> value = 42;
     *
     * // Single gate
     * auto r1 = value | fxt::ensure([](int x) { return x > 0; }, std::string{"must be positive"});
     * // r1 == expected{42}
     *
     * auto r2 = value | fxt::ensure([](int x) { return x > 100; }, std::string{"must be > 100"});
     * // r2 == unexpected{"must be > 100"}
     *
     * // Validation pipeline — first failing gate wins
     * auto validated = fxt::expected<int, std::string>{42}
     *                | fxt::ensure([](int x) { return x > 0;   }, std::string{"must be positive"})
     *                | fxt::ensure([](int x) { return x < 100; }, std::string{"must be < 100"})
     *                | fxt::ensure([](int x) { return x % 2 == 0; }, std::string{"must be even"});
     * @endcode
     *
     * @see fxt::filter
     */
    template<typename TPredicate, typename TError>
    constexpr auto ensure(TPredicate&& pred, TError&& err)
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
            return Exp{ typename Exp::unexpected_type{ err } };
        };
    }

}    // namespace fxt
