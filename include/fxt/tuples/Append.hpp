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
 * @file Append.hpp
 * @brief Deprecated shim — use fxt::mtuple_append from TupleAppend.hpp instead.
 *
 * The monadic tuple-append functionality previously exposed here as fxt::mappend
 * has been merged into fxt::mtuple_append (TupleAppend.hpp) under the standard
 * fxt/tuples naming convention (see Tuple.hpp for the full rename table).
 *
 * fxt::mtuple_append now supports error-type coercion for expected-like arguments,
 * covering every use case fxt::mappend provided.
 *
 * Migration — replace all uses of:
 * @code
 * container | fxt::mappend(value)
 * @endcode
 * with:
 * @code
 * container | fxt::mtuple_append(value)
 * @endcode
 *
 * @deprecated Include TupleAppend.hpp directly and use fxt::mtuple_append.
 * @see fxt::mtuple_append
 */

#pragma once

#include "TupleAppend.hpp"

namespace fxt
{
    namespace detail
    {
        struct mappend_deprecated_t
        {
            template<typename... Args>
            auto operator()(Args&&... args) const
            {
                return fxt::mtuple_append(std::forward<Args>(args)...);
            }
        };
    }    // namespace detail

    /**
     * @brief Deprecated alias for fxt::mtuple_append.
     *
     * Replace fxt::mappend(...) with fxt::mtuple_append(...).
     *
     * @deprecated Use fxt::mtuple_append (TupleAppend.hpp).
     */
    [[deprecated("fxt::mappend has been renamed to fxt::mtuple_append — see TupleAppend.hpp")]]
    inline constexpr detail::mappend_deprecated_t mappend{};

}    // namespace fxt
