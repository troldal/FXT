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
 * @file ForEach.hpp
 * @brief Range-level pass-through side-effect adaptor.
 *
 * Provides `fxt::for_each`, a pipe-compatible adaptor that applies a callable
 * to each element of a range as a side effect and then passes the range through
 * unchanged. Suitable for logging, mutation, accumulation, or any per-element
 * operation that should not alter the sequence of values in the pipeline.
 *
 * ## Key properties
 *
 * - **Pass-through**: the range is returned after visiting every element.
 * - **Value-category safe**: an lvalue range is returned by lvalue reference
 *   (no copy); an rvalue range is returned by value (moved, no dangling
 *   reference). The original `TRange&&` return type handed back a reference to
 *   the argument and dangled for temporaries.
 * - **std::invoke-based dispatch**: the callable is invoked via `std::invoke`,
 *   so member-function pointers (`&Type::method`) work in addition to lambdas
 *   and plain function pointers.
 *
 * ## Usage
 *
 * @code{.cpp}
 * #include <fxt/ranges/ForEach.hpp>
 * #include <vector>
 * #include <iostream>
 *
 * // Pipe syntax — range passes through for further chaining
 * std::vector<int> v = {1, 2, 3};
 * auto result = v | fxt::for_each([](int x) { std::cout << x << '\n'; });
 * // result is a copy of v; each element was printed
 *
 * // Direct-call syntax — lvalue returns reference, rvalue returns value
 * auto& ref = fxt::for_each(print)(v);          // ref aliases v, no copy
 * auto val  = fxt::for_each(print)(std::vector{4, 5, 6}); // moved, no dangling
 *
 * // Member-function pointer
 * struct Widget { void render(); };
 * std::vector<Widget> widgets = ...;
 * widgets | fxt::for_each(&Widget::render);
 * @endcode
 */

#pragma once

#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace fxt
{
    /**
     * @brief Adaptor returned by fxt::for_each().
     *
     * Holds the callable and, when invoked via the pipe operator or directly,
     * visits every element of the range with `std::invoke` then returns the
     * range with the correct value category:
     *   - lvalue input  → lvalue reference returned (no copy)
     *   - rvalue input  → value returned (moved from the range, no dangling)
     *
     * Not intended for direct construction — use fxt::for_each() instead.
     *
     * @tparam TFunction The type of the side-effect callable
     */
    template<typename TFunction>
    struct for_each_adaptor
    {
        TFunction function;

        /**
         * @brief Visit each element and return the range.
         *
         * Value-category dispatch via `if constexpr`:
         *   - Lvalue  (`TRange` deduces as `T&`):  returns `T&` — the original
         *     object, no copy.
         *   - Rvalue  (`TRange` deduces as `T`):   returns `T` move-constructed
         *     from the argument — safe to store, no dangling reference.
         *
         * `std::invoke` is used so that member-function pointers work as
         * callables, matching the rest of the fxt library.
         *
         * @tparam TRange Any type satisfying `std::ranges::range`
         * @param  range  The range to iterate over
         * @return The range itself (by reference for lvalues, by value for rvalues)
         */
        template<std::ranges::range TRange>
        constexpr decltype(auto) operator()(TRange&& range) const
        {
            for (auto&& elem : range) {
                std::invoke(function, elem);
            }
            if constexpr (std::is_lvalue_reference_v<TRange>) {
                return range;
            }
            else {
                return std::remove_reference_t<TRange>(std::forward<TRange>(range));
            }
        }
    };

    /**
     * @brief Apply a side effect to each element of a range and pass it through.
     *
     * Returns a `for_each_adaptor` that can be used with the pipe operator `|`
     * or called directly. The adaptor visits every element via `std::invoke(f, elem)`
     * and returns the range unchanged (by reference for lvalue ranges, by moved
     * value for rvalue ranges).
     *
     * @code{.cpp}
     * // Pipe usage
     * auto result = some_range
     *             | fxt::for_each([](auto& x) { x *= 2; })
     *             | fxt::for_each([](int x) { log(x); });
     *
     * // Direct call — lvalue gets a reference back
     * std::vector<int> v = {1, 2, 3};
     * auto& ref = fxt::for_each(print)(v);  // ref is v, no copy
     *
     * // Direct call — rvalue gets a value back (safe, no dangling)
     * auto out = fxt::for_each(print)(std::vector{1, 2, 3});
     *
     * // Member-function pointer
     * widgets | fxt::for_each(&Widget::render);
     * @endcode
     *
     * @tparam TFunction Callable type (deduced)
     * @param  f         Side-effect function/lambda/member-pointer to apply to each element
     * @return A `for_each_adaptor<decay_t<TFunction>>`
     */
    inline constexpr auto for_each = []<typename TFunction>(TFunction&& f) {
        return for_each_adaptor<std::decay_t<TFunction>>{ std::forward<TFunction>(f) };
    };

    /**
     * @brief Pipe operator for `range | fxt::for_each(f)`.
     *
     * Forwards the range to the adaptor and returns the result by value
     * (deduced as `auto`), which copies for lvalue ranges and moves for
     * rvalue ranges. This is the correct behaviour for pipe chains where
     * each step owns its range.
     */
    template<std::ranges::range TRange, typename TFunction>
    constexpr auto operator|(TRange&& range, for_each_adaptor<TFunction> adaptor)
    {
        return adaptor(std::forward<TRange>(range));
    }

}    // namespace fxt
