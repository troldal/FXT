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

/**
 * @file Format.hpp
 * @brief std::format and operator<< support for fxt::optional and fxt::expected.
 *
 * Provides two complementary mechanisms:
 *
 * 1. **std::formatter specialisations** (in namespace std) — enable
 *    std::format("{}", opt) and std::format("{}", exp) without any
 *    using-declarations.  The format spec, if supplied, is forwarded to the
 *    value type's own formatter on the success path; the empty/error path
 *    always uses "{}".
 *
 * 2. **operator<< overloads** (in namespace fxt) — enable stream output via
 *    std::cout << opt and std::cout << exp.  Because both std::ostream and
 *    std::optional / std::expected live in namespace std, ADL will not find
 *    these overloads automatically.  Bring them into scope with
 *    @code
 *      using fxt::operator<<;   // or: using namespace fxt;
 *    @endcode
 *
 * Output format:
 * | Type                              | Has value | Output              |
 * |-----------------------------------|-----------|---------------------|
 * | optional<T>                       | yes       | formatted T value   |
 * | optional<T>                       | no        | "nullopt"           |
 * | expected<T, E>  (T != void)       | yes       | formatted T value   |
 * | expected<T, E>  (T != void)       | no        | "unexpected(<E>)"   |
 * | expected<void, E>                 | yes       | "void"              |
 * | expected<void, E>                 | no        | "unexpected(<E>)"   |
 */

#pragma once

#include "Expected.hpp"
#include "Optional.hpp"
#include "../concepts/IsExpected.hpp"
#include "../concepts/IsOptional.hpp"
#include <format>
#include <ostream>
#include <type_traits>

// ============================================================================
// std::formatter specialisations
//
// These must live in namespace std so that std::format can find them via
// ordinary template lookup without any using-declarations at the call site.
// ============================================================================

namespace std
{
// ----------------------------------------------------------------------------
// optional formatter
// ----------------------------------------------------------------------------

#ifndef FXT_USE_TL_OPTIONAL

    /**
     * @brief std::formatter for std::optional<T>
     *
     * Forwards the format spec to T's own formatter on the value path.
     * Outputs "nullopt" when the optional is disengaged (format spec ignored).
     *
     * @code
     * std::optional<int> a = 42, b = std::nullopt;
     * std::format("{}",    a)  // "42"
     * std::format("{:>6}", a)  // "    42"  (spec forwarded to int formatter)
     * std::format("{}",    b)  // "nullopt"
     * @endcode
     */
    template<typename T>
        requires formattable<T, char>
    struct formatter<optional<T>>
    {
    private:
        formatter<T> value_fmt_;

    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            return value_fmt_.parse(ctx);
        }

        auto format(const optional<T>& opt, format_context& ctx) const
        {
            if (opt.has_value())
                return value_fmt_.format(*opt, ctx);
            return format_to(ctx.out(), "nullopt");
        }
    };

#else   // FXT_USE_TL_OPTIONAL

    template<typename T>
        requires formattable<T, char>
    struct formatter<tl::optional<T>>
    {
    private:
        formatter<T> value_fmt_;

    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            return value_fmt_.parse(ctx);
        }

        auto format(const tl::optional<T>& opt, format_context& ctx) const
        {
            if (opt.has_value())
                return value_fmt_.format(*opt, ctx);
            return format_to(ctx.out(), "nullopt");
        }
    };

#endif  // FXT_USE_TL_OPTIONAL

// ----------------------------------------------------------------------------
// expected formatter (non-void value type)
// ----------------------------------------------------------------------------

#ifndef FXT_USE_TL_EXPECTED

    /**
     * @brief std::formatter for std::expected<T, E> where T is not void.
     *
     * Forwards the format spec to T's formatter on the value path.
     * On the error path the spec is ignored and E is always formatted with "{}".
     *
     * @code
     * expected<int, std::string> ok  = 42;
     * expected<int, std::string> err = unexpected("oops");
     * std::format("{}",    ok)   // "42"
     * std::format("{:>6}", ok)   // "    42"
     * std::format("{}",    err)  // "unexpected(oops)"
     * @endcode
     */
    template<typename T, typename E>
        requires (!is_void_v<T>) && formattable<T, char> && formattable<E, char>
    struct formatter<expected<T, E>>
    {
    private:
        formatter<T> value_fmt_;

    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            return value_fmt_.parse(ctx);
        }

        auto format(const expected<T, E>& exp, format_context& ctx) const
        {
            if (exp.has_value())
                return value_fmt_.format(*exp, ctx);
            return format_to(ctx.out(), "unexpected({})", exp.error());
        }
    };

    /**
     * @brief std::formatter for std::expected<void, E>.
     *
     * Outputs "void" on the success path and "unexpected(<E>)" on the error path.
     * No format spec is supported (any non-empty spec throws std::format_error).
     */
    template<typename E>
        requires formattable<E, char>
    struct formatter<expected<void, E>>
    {
        constexpr auto parse(format_parse_context& ctx)
        {
            auto it = ctx.begin();
            if (it != ctx.end() && *it != '}')
                throw format_error("format spec not supported for expected<void, E>");
            return it;
        }

        auto format(const expected<void, E>& exp, format_context& ctx) const
        {
            if (exp.has_value())
                return format_to(ctx.out(), "void");
            return format_to(ctx.out(), "unexpected({})", exp.error());
        }
    };

#else   // FXT_USE_TL_EXPECTED

    template<typename T, typename E>
        requires (!is_void_v<T>) && formattable<T, char> && formattable<E, char>
    struct formatter<tl::expected<T, E>>
    {
    private:
        formatter<T> value_fmt_;

    public:
        constexpr auto parse(format_parse_context& ctx)
        {
            return value_fmt_.parse(ctx);
        }

        auto format(const tl::expected<T, E>& exp, format_context& ctx) const
        {
            if (exp.has_value())
                return value_fmt_.format(*exp, ctx);
            return format_to(ctx.out(), "unexpected({})", exp.error());
        }
    };

    template<typename E>
        requires formattable<E, char>
    struct formatter<tl::expected<void, E>>
    {
        constexpr auto parse(format_parse_context& ctx)
        {
            auto it = ctx.begin();
            if (it != ctx.end() && *it != '}')
                throw format_error("format spec not supported for expected<void, E>");
            return it;
        }

        auto format(const tl::expected<void, E>& exp, format_context& ctx) const
        {
            if (exp.has_value())
                return format_to(ctx.out(), "void");
            return format_to(ctx.out(), "unexpected({})", exp.error());
        }
    };

#endif  // FXT_USE_TL_EXPECTED

}    // namespace std

// ============================================================================
// operator<< overloads in namespace fxt
//
// Because std::ostream, std::optional, and std::expected all live in
// namespace std, ADL cannot reach these overloads automatically.  Bring them
// into scope with:
//
//     using fxt::operator<<;   // targeted import (preferred)
//     using namespace fxt;     // full import
//
// The std::format path above has no such limitation and is recommended for
// use inside generic lambdas (e.g. fxt::tee callbacks).
// ============================================================================

namespace fxt
{
    /**
     * @brief Stream output for optional_like types.
     *
     * Outputs the contained value when engaged, "nullopt" when empty.
     * Requires the value type to support operator<<.
     */
    template<typename TContainer>
        requires optional_like<std::remove_cvref_t<TContainer>>
              && requires(std::ostream& os, const TContainer& c) { os << *c; }
    std::ostream& operator<<(std::ostream& os, const TContainer& opt)
    {
        if (opt.has_value())
            os << *opt;
        else
            os << "nullopt";
        return os;
    }

    /**
     * @brief Stream output for expected_like types with a non-void value type.
     *
     * Outputs the contained value on the success path and
     * "unexpected(<error>)" on the error path.
     * Requires both value type and error type to support operator<<.
     */
    template<typename TContainer>
        requires expected_like<std::remove_cvref_t<TContainer>>
              && (!std::is_void_v<typename std::remove_cvref_t<TContainer>::value_type>)
              && requires(std::ostream& os, const TContainer& c) {
                     os << *c;
                     os << c.error();
                 }
    std::ostream& operator<<(std::ostream& os, const TContainer& exp)
    {
        if (exp.has_value())
            os << *exp;
        else
            os << "unexpected(" << exp.error() << ")";
        return os;
    }

}    // namespace fxt
