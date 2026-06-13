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
#include "../concepts/IsExpected.hpp"

namespace fxt
{
    /**
     * @brief Converts an expected-like type to fxt::optional, discarding error information.
     *
     * This operation extracts the success value from an expected and wraps it in an optional.
     * If the expected contains an error, an empty optional is returned.
     * The expected is taken by forwarding reference so rvalue expected types can move
     * their contained value rather than copying it.
     *
     * @return A pipe adaptor that accepts any expected_like container
     *
     * @code
     * auto result = fxt::expected<int, std::string>{42}
     *             | fxt::to_optional();
     * // result is fxt::optional<int>{42}
     *
     * auto error = fxt::expected<int, std::string>{fxt::unexpected("error")}
     *            | fxt::to_optional();
     * // error is fxt::optional<int>{fxt::nullopt}
     * @endcode
     */
    inline constexpr auto to_optional = []() {
        return []<typename TContainer>(TContainer&& ex)
            requires expected_like<std::remove_cvref_t<TContainer>>
        {
            using TValue = typename std::remove_cvref_t<TContainer>::value_type;
            if (ex.has_value())
                return fxt::optional<TValue>(std::forward<TContainer>(ex).value());
            return fxt::optional<TValue>(fxt::nullopt);
        };
    };

}    // namespace fxt