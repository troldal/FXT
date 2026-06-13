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

#include <concepts>
#include <utility>
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"

namespace fxt
{
    namespace impl
    {
        /**
         * @brief True for expected<expected<T, E>, E> — both layers must share the
         *        same error type E.
         */
        template<typename T>
        concept joinable_expected =
            expected_like<T> &&
            expected_like<typename T::value_type> &&
            std::same_as<typename T::error_type, typename T::value_type::error_type>;

        /**
         * @brief True for optional<optional<T>>.
         */
        template<typename T>
        concept joinable_optional =
            optional_like<T> &&
            optional_like<typename T::value_type>;

        /**
         * @brief True for any doubly-wrapped monad that fxt::join can flatten.
         */
        template<typename T>
        concept joinable = joinable_expected<T> || joinable_optional<T>;
    }    // namespace impl

    /**
     * @brief Monadic join (flatten) operation for nested optional/expected types
     *
     * This is the canonical monadic join: it collapses a doubly-wrapped monad of
     * the same family into a single layer:
     *   - expected<expected<T, E>, E> -> expected<T, E>
     *   - optional<optional<T>>       -> optional<T>
     *
     * For expected, if the outer expected holds an error, that error is
     * propagated; otherwise the inner expected is returned as-is. For optional,
     * if the outer optional is empty, the result is empty; otherwise the inner
     * optional is returned as-is. Either way, the two layers collapse into one.
     *
     * It is implemented as a nullary adaptor factory (consistent with fxt::value,
     * fxt::to_optional, etc.): calling fxt::join() yields the pipe adaptor, which
     * forwards to the container's and_then() member function with an identity
     * callable so the inner monad is returned without re-wrapping.
     *
     * @tparam TContainer The (deduced) doubly-wrapped monad type
     * @param container The nested optional/expected to flatten
     * @return The flattened optional<T> or expected<T, E>
     *
     * @section Concepts
     * The callable is constrained via a requires clause (impl::joinable) so it only
     * participates in overload resolution when its argument is a doubly-wrapped monad
     * of a single family:
     * - expected<expected<T, E1>, E2> additionally requires same_as<E1, E2> — a
     *   mismatched inner/outer error type is rejected at the call site rather than
     *   failing deep inside and_then().
     * - optional<optional<T>> is always joinable.
     *
     * Calling join on anything else (a single layer, or mismatched monad families)
     * produces a clean compile error rather than a confusing template instantiation
     * failure.
     *
     * @section Usage
     * @code
     * // Flatten the result of a curry | with() pipeline whose final callable
     * // itself returns fxt::expected<R, E>:
     * auto result = fxt::curry(compute)
     *             | fxt::with(validate_x(raw_x))   // expected<double, E>
     *             | fxt::with(validate_y(raw_y))   // expected<double, E>
     *             | fxt::join();                   // expected<expected<R, E>, E> -> expected<R, E>
     *
     * // Function call syntax
     * auto flat = fxt::join()(nested);   // nested : expected<expected<T, E>, E>
     *
     * // Nested optional
     * fxt::optional<fxt::optional<int>> nested_opt{fxt::optional<int>{42}};
     * auto flat_opt = nested_opt | fxt::join();   // optional<int>{42}
     * @endcode
     */
    inline constexpr auto join = []() {
        return []<typename TContainer>(TContainer&& container)
            requires impl::joinable<std::remove_cvref_t<TContainer>>
        {
            return std::forward<TContainer>(container).and_then(
                []<typename TInner>(TInner&& inner) -> decltype(auto) {
                    return std::forward<TInner>(inner);
                });
        };
    };
}    // namespace fxt
