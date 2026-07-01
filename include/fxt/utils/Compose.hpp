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
 * @file Compose.hpp
 * @brief Mathematical function composition: compose(f, g)(x) == f(g(x))
 *
 * This file provides `fxt::compose`, which combines two or more callables into a
 * single callable using conventional (right-to-left) mathematical composition
 * order: `compose(f, g, h)(x...)` evaluates as `f(g(h(x...)))` — the *last*
 * argument is applied first, to the original arguments, and each result flows
 * into the callable to its left.
 *
 * This is the opposite reading order from a left-to-right pipe chain
 * (`x | h | g | f`), which applies the same three callables in the same order
 * but spells them in reverse. Both conventions are legitimate; `fxt::compose`
 * follows the one used by mathematical notation (`f ∘ g`) and by
 * `std::function`-composition idioms elsewhere in C++, since that is what
 * "compose" conventionally means.
 *
 * ## Note on monadic composition
 *
 * `fxt::compose` operates on plain callables and does not special-case
 * `fxt::optional`/`fxt::expected` return types: composing two functions that
 * each return a monad produces a function returning a *nested* monad
 * (`optional<optional<T>>`), not a short-circuiting chain. Kleisli-style
 * composition — where an error/`nullopt` from one step skips the rest — is
 * already expressible by chaining `fxt::and_then`:
 * @code
 * auto pipeline = [](auto&& x) { return x | fxt::and_then(g) | fxt::and_then(f); };
 * @endcode
 * and, once adaptors fuse under the pipe operator, `and_then(g) | and_then(f)`
 * will do the same without a value in hand. A separate monadic `compose` is
 * therefore intentionally not provided here.
 *
 * ## Usage
 *
 * @code
 * auto negate = [](int x) { return -x; };
 * auto square = [](int x) { return x * x; };
 *
 * auto square_then_negate = fxt::compose(negate, square);
 * square_then_negate(3);   // negate(square(3)) == -9
 *
 * // Any number of callables; the innermost may take multiple arguments
 * auto to_string = [](int x) { return std::to_string(x); };
 * auto pipeline = fxt::compose(to_string, negate, square);
 * pipeline(3);             // to_string(negate(square(3))) == "-9"
 * @endcode
 *
 * @see fxt::curry
 * @see fxt::and_then
 */

#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace fxt
{
    namespace impl
    {
        // Composes exactly two callables: h(args...) == f(g(args...)).
        // All variadic compose(...) is built from repeated pairwise composition.
        template<typename F, typename G>
        constexpr auto compose2(F&& f, G&& g)
        {
            return [f = std::forward<F>(f), g = std::forward<G>(g)]<typename... Args>(Args&&... args)
                noexcept(std::is_nothrow_invocable_v<const G&, Args...> &&
                         std::is_nothrow_invocable_v<const F&, std::invoke_result_t<const G&, Args...>>)
                -> decltype(auto)
            {
                return std::invoke(f, std::invoke(g, std::forward<Args>(args)...));
            };
        }
    }    // namespace impl

    /**
     * @brief Compose two or more callables using conventional right-to-left order.
     *
     * `fxt::compose(f, g, h, ...)` returns a callable equivalent to
     * `f(g(h(...(args...))))`: the last callable in the argument list receives the
     * original call arguments (and may take any number of them), and every other
     * callable receives exactly one argument — the result of the callable to its
     * right.
     *
     * @tparam F The outermost (applied-last) callable type
     * @tparam Fs Zero or more additional callable types, applied right-to-left
     * @param f The outermost callable
     * @param fs Additional callables, innermost last
     * @return A callable computing the composition of all arguments
     *
     * @section Usage
     * @code
     * auto negate = [](int x) { return -x; };
     * auto square = [](int x) { return x * x; };
     *
     * auto h = fxt::compose(negate, square);
     * h(3);  // negate(square(3)) == -9
     *
     * // compose(f) with a single callable is just f, forwarded unchanged
     * auto same_as_negate = fxt::compose(negate);
     * same_as_negate(5);  // -5
     * @endcode
     */
    template<typename F, typename... Fs>
    constexpr auto compose(F&& f, Fs&&... fs)
    {
        if constexpr (sizeof...(Fs) == 0) {
            return std::forward<F>(f);
        }
        else {
            return impl::compose2(std::forward<F>(f), fxt::compose(std::forward<Fs>(fs)...));
        }
    }

}    // namespace fxt
