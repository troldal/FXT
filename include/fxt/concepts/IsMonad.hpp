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

#include "IsOptional.hpp"
#include "IsExpected.hpp"

namespace fxt
{
    /**
     * @brief Concept to check if a type behaves like a monad
     *
     * A monad-like type is either an optional-like type or an expected-like type.
     * This concept can be used to constrain template parameters to only accept
     * monadic types that support common operations like transform, and_then, etc.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::monad_like M>
     * void process_monad(M&& monad) {
     *     // Works with both fxt::optional and fxt::expected
     * }
     *
     * auto opt = fxt::optional<int>{42};
     * auto exp = fxt::expected<int, std::string>{42};
     * process_monad(opt); // OK
     * process_monad(exp); // OK
     * process_monad(42);  // Error: 42 is not monad_like
     * @endcode
     */
    template<typename T>
    concept monad_like = optional_like<T> || expected_like<T>;

    /**
     * @brief Bool variable-template wrappers for @ref monad_like.
     *
     * clang-cl's Microsoft-ABI name mangler cannot mangle the associated
     * constraints of a *variadic* function template when the constraint expands
     * to @ref monad_like / @ref expected_like / @ref optional_like — those
     * concepts are defined with `requires`-expressions containing generic
     * lambdas, and the mangler bails out with "cannot mangle this pack expansion
     * yet". Constraining a variadic template on these plain `bool` variable
     * templates keeps the mangled constraint a simple variable-template-id
     * (exactly as @c fxt::tuple_append does with @c impl::is_tuple_like_v),
     * sidestepping the bug while preserving identical overload-resolution
     * behaviour. Non-variadic templates are unaffected and may keep using the
     * concepts directly.
     */
    template<typename T>
    inline constexpr bool monad_like_v = monad_like<T>;

    template<typename... Ts>
    inline constexpr bool all_monad_like_v = (monad_like<Ts> && ...);

} // namespace fxt

