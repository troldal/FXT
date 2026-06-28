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
#include <concepts>
#include <type_traits>

namespace fxt
{
    /**
     * @brief Converts an optional-like type to fxt::expected, using an eager error value if empty.
     *
     * The error value is constructed and captured when the adaptor is created, regardless
     * of whether the optional ultimately holds a value. Prefer the factory overload when
     * constructing the error is expensive and the optional is expected to be engaged most
     * of the time.
     *
     * @tparam TError The error type for the resulting expected (must not be callable with no args)
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
        requires (!std::invocable<std::decay_t<TError>>)
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

    /**
     * @brief Converts an optional-like type to fxt::expected, using a lazy error factory if empty.
     *
     * The factory is invoked only when the optional is disengaged, so no error object is
     * constructed on the success path. This overload is selected automatically when the
     * argument is callable with no arguments and returns a non-void type.
     *
     * @tparam TFactory Callable type invocable with no arguments; its return type becomes the error type
     * @param factory A no-argument callable that produces the error value on demand
     * @return A pipe adaptor that accepts any optional_like container
     *
     * @code
     * // Factory is called only when the optional is empty
     * auto result = fxt::optional<int>{42}
     *             | fxt::to_expected([]{ return std::string{"no value"}; });
     * // result is fxt::expected<int, std::string>{42}   — factory never called
     *
     * auto error = fxt::optional<int>{fxt::nullopt}
     *            | fxt::to_expected([]{ return std::string{"no value"}; });
     * // error is fxt::expected<int, std::string>{fxt::unexpected("no value")}
     * @endcode
     */
    template<typename TFactory>
        requires std::invocable<std::decay_t<TFactory>>
              && (!std::is_void_v<std::invoke_result_t<std::decay_t<TFactory>>>)
    inline constexpr auto to_expected(TFactory&& factory)
    {
        return [factory = std::forward<TFactory>(factory)]<typename TContainer>(TContainer&& opt)
            requires optional_like<std::remove_cvref_t<TContainer>>
        {
            using TValue = typename std::remove_cvref_t<TContainer>::value_type;
            using EType  = std::decay_t<std::invoke_result_t<std::decay_t<TFactory>>>;
            if (opt.has_value())
                return fxt::expected<TValue, EType>(std::forward<TContainer>(opt).value());
            return fxt::expected<TValue, EType>(fxt::unexpected(std::invoke(factory)));
        };
    }

}    // namespace fxt