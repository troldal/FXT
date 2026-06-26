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
 * @file TuplePipe.hpp
 * @brief Pipe operator (`operator|`) for tuple-like types
 *
 * Defines a single forwarding-reference `operator|` that enables pipeline-style
 * composition for any type satisfying `fxt::tuple_like` (fxt::tuple / std::tuple,
 * fxt::flat_tuple, std::pair, std::array, …):
 *
 * @code
 * auto result = fxt::make_tuple(1, 2, 3)
 *     | fxt::tuple_reverse()
 *     | fxt::tuple_transform([](int x) { return x * 2; });
 * @endcode
 *
 * ## Why global namespace?
 *
 * `fxt::tuple` is a plain alias for `std::tuple`. Because it is not a distinct
 * type its associated namespace (for ADL) is `std`, not `fxt`. Placing
 * `operator|` inside `namespace fxt` would therefore make it unreachable for
 * `fxt::tuple` expressions without a `using namespace fxt;` directive.
 *
 * The operator is constrained to `fxt::tuple_like` types to minimise unintended
 * matches in user code, but callers that define their own `operator|` for
 * `std::tuple` in the same translation unit may encounter ambiguity — the
 * constraint is the only guard. `fxt::flat_tuple`'s pipe operators are defined
 * separately in `namespace fxt` inside FlatTuple.hpp and are found via ADL;
 * the forwarding-reference overload here is redundant for flat_tuple but
 * harmless (the flat_tuple overloads are more specialised and win in overload
 * resolution).
 */

#pragma once

#include "../concepts/IsTuple.hpp"
#include <functional>
#include <utility>

// Intentionally in the global namespace — see file-level doc comment above.
template<typename TTuple, typename Callable>
    requires fxt::tuple_like<std::remove_cvref_t<TTuple>>
          && requires(TTuple&& t, Callable&& c) {
                 std::invoke(std::forward<Callable>(c), std::forward<TTuple>(t));
             }
constexpr auto operator|(TTuple&& tuple, Callable&& function)
    -> decltype(std::invoke(std::forward<Callable>(function), std::forward<TTuple>(tuple)))
{
    return std::invoke(std::forward<Callable>(function), std::forward<TTuple>(tuple));
}
