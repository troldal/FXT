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

#include "../concepts/IsVariant.hpp"
#include "../concepts/IsMonad.hpp"
#include <variant>
#include <functional>
#include <utility>

namespace fxt
{
    /**
     * @brief Conditionally execute a side-effect function when a variant holds a specific alternative type
     *
     * The when combinator checks if a variant currently holds the specified type T. If it does,
     * it executes the provided function with that value as a side effect. The variant is returned
     * unchanged, allowing it to flow through a pipeline.
     *
     * This is useful for logging, validation, debugging, or other side effects that should only
     * occur when the variant is in a specific state.
     *
     * @tparam T The alternative type to check for
     * @tparam TVariant The variant type (deduced)
     * @tparam F The function type (deduced)
     * @param variant The variant to check
     * @param func The side-effect function to execute if the variant holds type T
     * @return The variant unchanged (forwarded with the same value category)
     *
     * @section Direct Usage
     * @code
     * fxt::variant<int, std::string> v = 42;
     *
     * // Execute side effect only if v holds int
     * fxt::when<int>(v, [](int x) {
     *     std::cout << "Got int: " << x << '\n';
     * }); // Prints "Got int: 42"
     *
     * // This won't execute
     * fxt::when<std::string>(v, [](const auto& s) {
     *     std::cout << "Got string: " << s << '\n';
     * }); // Nothing printed
     * @endcode
     */
    template<typename T, typename TVariant, typename F>
        requires fxt::variant_like<std::remove_cvref_t<TVariant>> &&
                 std::invocable<F, T>
    constexpr decltype(auto) when(TVariant&& variant, F&& func)
    {
        if (std::holds_alternative<T>(variant)) {
            std::invoke(std::forward<F>(func), std::get<T>(std::forward<TVariant>(variant)));
        }
        return std::forward<TVariant>(variant);
    }

    /**
     * @brief Returns a curried version of when for use in pipelines
     *
     * This overload enables the when combinator to be used with the pipe operator,
     * allowing for elegant functional composition in variant processing pipelines.
     *
     * @tparam T The alternative type to check for
     * @tparam F The function type (deduced)
     * @param func The side-effect function to execute if the variant holds type T
     * @return A callable that accepts a variant and conditionally executes the function
     *
     * @section Pipeline Usage
     * @code
     * fxt::variant<int, double, std::string> v = 42;
     *
     * v | fxt::when<int>([](int x) { std::cout << "int: " << x << '\n'; })
     *   | fxt::when<double>([](double x) { std::cout << "double: " << x << '\n'; })
     *   | fxt::when<std::string>([](const auto& s) { std::cout << "string: " << s << '\n'; });
     * // Only prints "int: 42"
     *
     * // Useful for logging in complex pipelines
     * auto result = get_variant()
     *             | fxt::transform_variant(some_transform)
     *             | fxt::when<ErrorType>([](const auto& err) { log_error(err); })
     *             | fxt::visit(final_handler);
     * @endcode
     */
    template<typename T, typename F>
    constexpr auto when(F&& func)
    {
        return [f = std::forward<F>(func)]<typename TVariant>(TVariant&& variant)
            -> decltype(auto)
            requires fxt::variant_like<std::remove_cvref_t<TVariant>> &&
                     std::invocable<F, T>
        {
            if (std::holds_alternative<T>(variant)) {
                std::invoke(f, std::get<T>(std::forward<TVariant>(variant)));
            }
            return std::forward<TVariant>(variant);
        };
    }

    /**
     * @brief Conditionally execute a side-effect function on a variant inside a monadic container
     *
     * The mwhen combinator works on monadic containers (like optional or expected) that hold variants.
     * If the container has a value and that variant holds the specified type T, it executes the
     * provided function. The container with its variant is returned unchanged.
     *
     * This enables conditional side effects on variants wrapped in monadic types, maintaining
     * the monadic context throughout the pipeline.
     *
     * @tparam T The alternative type to check for in the wrapped variant
     * @tparam TMonad The monadic container type (deduced)
     * @tparam F The function type (deduced)
     * @param monad The monadic container holding a variant
     * @param func The side-effect function to execute if the variant holds type T
     * @return The monadic container unchanged (forwarded with the same value category)
     *
     * @section Direct Usage
     * @code
     * using MyVariant = fxt::variant<int, std::string>;
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * // Execute side effect only if opt has value and holds int
     * fxt::mwhen<int>(opt, [](int x) {
     *     std::cout << "Got int: " << x << '\n';
     * }); // Prints "Got int: 42"
     *
     * fxt::optional<MyVariant> empty;
     * fxt::mwhen<int>(empty, [](int x) {
     *     std::cout << "Got int: " << x << '\n';
     * }); // Nothing printed (container is empty)
     * @endcode
     */
    template<typename T, typename TMonad, typename F>
        requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                 fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type> &&
                 std::invocable<F, T>
    constexpr decltype(auto) mwhen(TMonad&& monad, F&& func)
    {
        if (monad.has_value()) {
            auto& variant = *monad;
            if (std::holds_alternative<T>(variant)) {
                std::invoke(std::forward<F>(func), std::get<T>(variant));
            }
        }
        return std::forward<TMonad>(monad);
    }

    /**
     * @brief Returns a curried version of mwhen for use in pipelines
     *
     * This overload enables the mwhen combinator to be used with the pipe operator,
     * allowing for elegant functional composition when working with variants wrapped
     * in monadic containers.
     *
     * @tparam T The alternative type to check for in the wrapped variant
     * @tparam F The function type (deduced)
     * @param func The side-effect function to execute if the variant holds type T
     * @return A callable that accepts a monadic container and conditionally executes the function
     *
     * @section Pipeline Usage
     * @code
     * using MyVariant = fxt::variant<int, double, std::string>;
     *
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * opt | fxt::mwhen<int>([](int x) { std::cout << "int: " << x << '\n'; })
     *     | fxt::mwhen<double>([](double x) { std::cout << "double: " << x << '\n'; })
     *     | fxt::mwhen<std::string>([](const auto& s) { std::cout << "string: " << s << '\n'; });
     * // Only prints "int: 42"
     *
     * // Complex pipeline with error handling
     * auto result = parse_variant()
     *             | fxt::transform(process_variant)
     *             | fxt::mwhen<ErrorVariant>([](const auto& err) { log_error(err); })
     *             | fxt::mwhen<SuccessVariant>([](const auto& val) { log_success(val); })
     *             | fxt::and_then(save_to_database);
     * @endcode
     */
    template<typename T, typename F>
    constexpr auto mwhen(F&& func)
    {
        return [f = std::forward<F>(func)]<typename TMonad>(TMonad&& monad)
            -> decltype(auto)
            requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                     fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type> &&
                     std::invocable<F, T>
        {
            if (monad.has_value()) {
                auto& variant = *monad;
                if (std::holds_alternative<T>(variant)) {
                    std::invoke(f, std::get<T>(variant));
                }
            }
            return std::forward<TMonad>(monad);
        };
    }

} // namespace fxt

