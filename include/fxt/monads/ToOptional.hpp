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
     * @brief Converts a fxt::expected to fxt::optional, discarding error information.
     *
     * This operation extracts the success value from an expected and wraps it in an optional.
     * If the expected contains an error, an empty optional is returned.
     *
     * @return A callable that can be used with the pipe operator
     *
     * @example
     * auto result = fxt::expected<int, std::string>{42}
     *             | fxt::to_optional();
     * // result is fxt::optional<int>{42}
     *
     * auto error = fxt::expected<int, std::string>{fxt::unexpected("error")}
     *            | fxt::to_optional();
     * // error is fxt::optional<int>{fxt::nullopt}
     */
    // TODO: ERGONOMICS — same issues as to_expected: const-lvalue-only parameter forces a
    //       copy of the contained value (no move from rvalue expected), and the nullary
    //       factory requires `| fxt::to_optional()` while sibling adaptors like and_then(f)
    //       are used without the extra `()` (see TODO in Value.hpp on unifying the convention).
    inline constexpr auto to_optional = []() {
        return []<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) -> fxt::optional<TValue> {
            if (ex.has_value()) return ex.value();
            return fxt::nullopt;
        };
    };

}    // namespace fxt