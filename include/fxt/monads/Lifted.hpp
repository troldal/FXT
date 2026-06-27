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

#include <utility>

namespace fxt
{
    /**
     * @brief Lift a plain adaptor into a monadic context via `.transform()`.
     *
     * Given any adaptor `op` that operates on a plain value, `lifted(op)` returns
     * a new adaptor that applies `op` to the value inside a monad (optional, expected,
     * or anything with a `.transform()` method).
     *
     * This is the universal implementation of the `mX` adaptor family:
     * @code
     * // These three are equivalent:
     * fxt::mindex(opt)
     * fxt::lifted(fxt::index())(opt)
     * opt | fxt::lifted(fxt::index())
     *
     * // Use lifted directly with any adaptor:
     * auto result = exp | fxt::lifted(fxt::tuple_take<2>());
     * @endcode
     *
     * All `mX` names in the library are kept as one-line aliases over `lifted` for
     * backward compatibility and discoverability.
     *
     * @param a The adaptor to lift (stored by value; stateless lambdas cost nothing)
     * @return A callable `M&& m -> m.transform(a)` usable in pipelines
     */
    template<typename Adaptor>
    constexpr auto lifted(Adaptor&& a)
    {
        return [a = std::forward<Adaptor>(a)]<typename M>(M&& m) {
            return std::forward<M>(m).transform(a);
        };
    }

}  // namespace fxt
