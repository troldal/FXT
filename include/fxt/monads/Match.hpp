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

#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include <type_traits>
#include <utility>

namespace fxt
{
    namespace detail
    {
        /**
         * @brief Helper to match on expected types with proper value category handling
         */
        template<typename OnValue, typename OnError>
        struct match_expected_t
        {
            OnValue on_value;
            OnError on_error;

            // exp's value category decides both whether *exp/exp.error() are moved
            // from, and whether on_value/on_error themselves are moved-from when
            // invoked (consuming match vs. by-reference match).
            template<typename TExp>
                requires expected_like<std::remove_cvref_t<TExp>>
            constexpr auto operator()(TExp&& exp) -> decltype(auto)
            {
                if constexpr (std::is_rvalue_reference_v<TExp&&>) {
                    if (exp.has_value()) {
                        return std::move(on_value)(std::move(*exp));
                    } else {
                        return std::move(on_error)(std::move(exp.error()));
                    }
                } else {
                    if (exp.has_value()) {
                        return on_value(*exp);
                    } else {
                        return on_error(exp.error());
                    }
                }
            }
        };

        /**
         * @brief Helper to match on optional types with proper value category handling
         */
        template<typename OnValue, typename OnNone>
        struct match_optional_t
        {
            OnValue on_value;
            OnNone on_none;

            // opt's value category decides both whether *opt is moved from, and
            // whether on_value/on_none themselves are moved-from when invoked
            // (consuming match vs. by-reference match).
            template<typename TOpt>
                requires optional_like<std::remove_cvref_t<TOpt>>
            constexpr auto operator()(TOpt&& opt) -> decltype(auto)
            {
                if constexpr (std::is_rvalue_reference_v<TOpt&&>) {
                    if (opt.has_value()) {
                        return std::move(on_value)(std::move(*opt));
                    } else {
                        return std::move(on_none)();
                    }
                } else {
                    if (opt.has_value()) {
                        return on_value(*opt);
                    } else {
                        return on_none();
                    }
                }
            }
        };
    }

    /**
     * @brief Matcher returned by fxt::match
     *
     * Dispatches on the *monad's* value category via detail::match_expected_t /
     * detail::match_optional_t, and on its OWN value category to decide whether
     * the handlers are moved or copied:
     * - Invoking an rvalue matcher (e.g. the temporary returned directly by
     *   fxt::match(...), or one explicitly std::move'd) moves the handlers,
     *   so move-only handlers work for one-shot use.
     * - Invoking a stored matcher (lvalue, const or not) copies the handlers,
     *   so the matcher remains valid for repeated use.
     *
     * @note Deliberately declared in namespace `fxt` (not `fxt::detail`): this is
     * the type returned to callers and piped via `operator|`, and ADL for
     * `fxt::operator|` (Expected.hpp/Optional.hpp) needs `fxt` among this type's
     * associated namespaces to find it.
     */
    template<typename OnValue, typename OnError>
    struct matcher_t
    {
        OnValue on_value;
        OnError on_error;

        // Forwards on_value/on_error with *this's value category via forward_like:
        // an rvalue matcher moves the handlers, an lvalue (const or not) copies them.
        template<typename Self, typename TMonad>
        constexpr auto operator()(this Self&& self, TMonad&& monad) -> decltype(auto)
        {
            return dispatch(std::forward<TMonad>(monad),
                             std::forward_like<Self>(self.on_value),
                             std::forward_like<Self>(self.on_error));
        }

    private:
        template<typename TMonad, typename OV, typename OE>
        static constexpr auto dispatch(TMonad&& monad, OV&& value_handler, OE&& error_handler) -> decltype(auto)
        {
            using TDecayed = std::remove_cvref_t<TMonad>;

            // Check if it's an expected-like type
            if constexpr (requires {
                typename TDecayed::value_type;
                typename TDecayed::error_type;
                monad.has_value();
                monad.error();
            })
            {
                using match_t = detail::match_expected_t<std::decay_t<OV>, std::decay_t<OE>>;
                return match_t{std::forward<OV>(value_handler), std::forward<OE>(error_handler)}(std::forward<TMonad>(monad));
            }
            // Check if it's an optional-like type
            else if constexpr (requires {
                typename TDecayed::value_type;
                monad.has_value();
            })
            {
                // Handle optional (on_error acts as on_none)
                using match_t = detail::match_optional_t<std::decay_t<OV>, std::decay_t<OE>>;
                return match_t{std::forward<OV>(value_handler), std::forward<OE>(error_handler)}(std::forward<TMonad>(monad));
            }
            else
            {
                static_assert(sizeof(TMonad) == 0, "match requires an expected-like or optional-like type");
            }
        }
    };

    /**
     * @brief Pattern matching for expected and optional types
     *
     * Provides functional pattern matching that exhaustively handles all cases.
     * For expected types, provides handlers for both success and error cases.
     * For optional types, provides handlers for value present and empty cases.
     *
     * Both handlers must return the same type (or convertible types).
     * The function properly forwards value categories (lvalue/rvalue references).
     *
     * @section Usage with expected
     * @code
     * auto result = safe_divide(10, 2)
     *     | fxt::match(
     *         [](int value) { return "Success: " + std::to_string(value); },
     *         [](const std::string& err) { return "Error: " + err; }
     *     );
     *
     * // Or with function call syntax
     * auto result2 = fxt::match(
     *     safe_divide(10, 0),
     *     [](int value) { return value * 2; },
     *     [](const std::string& err) { return -1; }
     * );
     * @endcode
     *
     * @section Usage with optional
     * @code
     * auto result = parse_int("42")
     *     | fxt::match(
     *         [](int value) { return value * 2; },
     *         []() { return 0; }  // Called when empty
     *     );
     * @endcode
     */
    inline constexpr auto match = []<typename OnValue, typename OnError>(OnValue&& on_value, OnError&& on_error)
    {
        return matcher_t<std::decay_t<OnValue>, std::decay_t<OnError>>{
            std::forward<OnValue>(on_value), std::forward<OnError>(on_error)
        };
    };

}    // namespace fxt
