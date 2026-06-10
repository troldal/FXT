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

/**
 * @brief Perfect forwarding macro that wraps std::forward with automatic type deduction.
 *
 * This macro simplifies the use of std::forward by automatically deducing the type
 * from the argument, eliminating the need to explicitly specify template parameters.
 *
 * @param ... The arguments to be perfectly forwarded
 * @return The forwarded arguments with preserved value category (lvalue/rvalue)
 *
 * @example
 * template<typename T>
 * void foo(T&& arg) {
 *     bar(FWD(arg));  // Equivalent to: bar(std::forward<decltype(arg)>(arg))
 * }
 */
// TODO: SAFETY — FWD and LIFT are unprefixed object-like macro names defined for every
//       includer of fxt.hpp; they leak into all downstream code and will silently collide
//       with other libraries' FWD/LIFT macros (a common name in forwarding utilities).
//       Prefix them (FXT_FWD / FXT_LIFT) and/or guard with #ifndef. Also: this header uses
//       std::forward but does not #include <utility>.
#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

/**
 * @brief Lifts a function or callable into a generic lambda with perfect forwarding.
 *
 * The LIFT macro converts any function, method, or callable into a generic lambda that:
 * - Accepts any number and type of arguments via variadic templates
 * - Perfectly forwards all arguments using FWD
 * - Preserves the noexcept specification of the original callable
 * - Automatically deduces the return type
 * - Maintains the exact semantics of calling the original function
 *
 * This is particularly useful for:
 * - Passing overloaded functions to higher-order functions
 * - Using template functions in functional pipelines
 * - Working with functions that can't be directly passed as function objects
 * - Composing functions in the pipe operator
 *
 * @param X The function, method, or callable to lift
 * @return A generic lambda that wraps the callable with perfect forwarding
 *
 * @note The macro name is intentionally uppercase following C++ convention that
 *       macros should be visually distinct from regular functions and variables.
 *
 * @example
 * // Lifting an overloaded function
 * auto get_first = LIFT(std::get<0>);
 * auto result = tuple_opt | fxt::transform(get_first);
 *
 * @example
 * // Using with pipe operator for transformations
 * auto result = optional
 *     | fxt::transform(LIFT(std::toupper))
 *     | fxt::transform(LIFT(std::abs));
 *
 * @example
 * // Lifting a template function
 * auto to_string = LIFT(std::to_string);
 * auto str_opt = int_opt | fxt::transform(to_string);
 */
#define LIFT(X) [](auto&&... args) noexcept(noexcept(X(FWD(args)...))) -> decltype(X(FWD(args)...)) { return X(FWD(args)...); }