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
#include "../concepts/IsOptional.hpp"

namespace fxt
{
    /**
     * @brief Converts an optional-like type to fxt::expected, using a provided error value if empty.
     *
     * This operation extracts the value from an optional and wraps it in an expected.
     * If the optional is empty, an expected with the provided error is returned.
     * The optional is taken by forwarding reference so rvalue optionals can move their
     * contained value rather than copying it.
     *
     * @tparam TError The error type for the resulting expected (decayed automatically)
     * @param err The error value to use if the optional is empty
     * @return A pipe adaptor that accepts any optional_like container
     *
     * @code
     * auto result = fxt::optional<int>{42}
     *             | fxt::to_expected<std::string>("no value");
     * // result is fxt::expected<int, std::string>{42}
     *
     * auto error = fxt::optional<int>{fxt::nullopt}
     *            | fxt::to_expected<std::string>("no value");
     * // error is fxt::expected<int, std::string>{fxt::unexpected("no value")}
     * @endcode
     */
    template<typename TError>
    inline constexpr auto to_expected(TError&& err)
    {
        return [err = std::forward<TError>(err)]<typename TContainer>(TContainer&& opt)
            requires optional_like<std::remove_cvref_t<TContainer>>
        {
            using TValue = typename std::remove_cvref_t<TContainer>::value_type;
            using EType  = std::decay_t<TError>;
            if (opt.has_value())
                return fxt::expected<TValue, EType>(std::forward<TContainer>(opt).value());
            return fxt::expected<TValue, EType>(fxt::unexpected(err));
        };
    }

}    // namespace fxt