//
// Created by kenne on 15/02/2025.
//

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