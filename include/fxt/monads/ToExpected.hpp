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

#include "Expected.hpp"
#include "Optional.hpp"

namespace fxt
{
    /**
     * @brief Converts a fxt::optional to fxt::expected, using a provided error value if empty.
     *
     * This operation extracts the value from an optional and wraps it in an expected.
     * If the optional is empty, an expected with the provided error is returned.
     *
     * @tparam TError The error type for the resulting expected
     * @param err The error value to use if the optional is empty
     * @return A callable that can be used with the pipe operator
     *
     * @example
     * auto result = fxt::optional<int>{42}
     *             | fxt::to_expected<std::string>("no value");
     * // result is fxt::expected<int, std::string>{42}
     *
     * auto error = fxt::optional<int>{fxt::nullopt}
     *            | fxt::to_expected<std::string>("no value");
     * // error is fxt::expected<int, std::string>{fxt::unexpected("no value")}
     */
    // TODO: ERGONOMICS — the adaptor only accepts `const fxt::optional<TValue>&`, so the
    //       contained value is always copied; add an rvalue overload (or take the optional
    //       by forwarding reference) so `make_opt() | to_expected(...)` can move. Note also
    //       that TError deduced from a forwarding reference can become an lvalue reference
    //       type (e.g. passing a named std::string), making the returned expected's error
    //       type a reference — decay TError. The doc example `to_expected<std::string>(...)`
    //       suggests an explicit template parameter that the deduced signature ignores.
    template<typename TError>
    inline constexpr auto to_expected(TError&& err)
    {
        return [err = std::forward<TError>(err)]<typename TValue>(const fxt::optional<TValue>& opt) -> fxt::expected<TValue, TError> {
            if (opt.has_value()) return opt.value();
            return fxt::unexpected(err);
        };
    }

}    // namespace fxt