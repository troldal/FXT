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
     * @brief Conditionally transform a variant's value when it holds a specific alternative type
     *
     * The transform_when combinator checks if a variant currently holds the specified type T.
     * If it does, it applies the provided transformation function to that value and updates
     * the variant with the result. If the variant holds a different type, it is returned unchanged.
     *
     * This is useful for conditional transformations that should only apply to specific
     * alternative types in a variant, allowing the variant to flow through a pipeline.
     *
     * @tparam T The alternative type to check for and transform
     * @tparam TVariant The variant type (deduced)
     * @tparam F The transformation function type (deduced)
     * @param variant The variant to check and potentially transform
     * @param func The transformation function to apply if the variant holds type T
     * @return The variant (potentially transformed). Lvalue inputs are returned by reference;
     *         rvalue inputs are returned by value to prevent dangling references in pipelines.
     *
     * @section Direct Usage
     * @code
     * fxt::variant<int, std::string> v = 42;
     *
     * // Transform only if v holds int
     * auto result = fxt::transform_when<int>(v, [](int x) {
     *     return x * 2;
     * }); // v now holds 84
     *
     * // This won't transform (v holds int, not string)
     * fxt::transform_when<std::string>(v, [](std::string s) {
     *     return s + " world";
     * }); // v unchanged
     * @endcode
     */
    template<typename T, typename TVariant, typename F>
        requires fxt::variant_like<std::remove_cvref_t<TVariant>> &&
                 std::invocable<F, T>
    constexpr auto transform_when(TVariant&& variant, F&& func)
        -> std::conditional_t<std::is_lvalue_reference_v<TVariant>,
                               TVariant,
                               std::remove_cvref_t<TVariant>>
    {
        if (std::holds_alternative<T>(variant)) {
            std::get<T>(variant) = std::invoke(std::forward<F>(func), std::get<T>(std::forward<TVariant>(variant)));
        }
        return std::forward<TVariant>(variant);
    }

    /**
     * @brief Returns a curried version of transform_when for use in pipelines
     *
     * This overload enables the transform_when combinator to be used with the pipe operator,
     * allowing for elegant functional composition in variant processing pipelines.
     *
     * @tparam T The alternative type to check for and transform
     * @tparam F The transformation function type (deduced)
     * @param func The transformation function to apply if the variant holds type T
     * @return A callable that accepts a variant and conditionally transforms it
     *
     * @section Pipeline Usage
     * @code
     * fxt::variant<int, double, std::string> v = 42;
     *
     * v | fxt::transform_when<int>([](int x) { return x * 2; })
     *   | fxt::transform_when<double>([](double x) { return x * 3.0; })
     *   | fxt::transform_when<std::string>([](std::string s) { return s + "!"; });
     * // v now holds 84 (only int transformation applied)
     *
     * // Useful in complex pipelines
     * auto result = get_variant()
     *             | fxt::transform_when<int>([](int x) { return x + 10; })
     *             | fxt::when<int>([](int x) { std::cout << "Result: " << x << '\n'; })
     *             | fxt::visit(final_handler);
     * @endcode
     */
    template<typename T, typename F>
    constexpr auto transform_when(F&& func)
    {
        return [f = std::forward<F>(func)]<typename TVariant>(TVariant&& variant)
            -> std::conditional_t<std::is_lvalue_reference_v<TVariant>,
                                  TVariant,
                                  std::remove_cvref_t<TVariant>>
            requires fxt::variant_like<std::remove_cvref_t<TVariant>> &&
                     std::invocable<F, T>
        {
            if (std::holds_alternative<T>(variant)) {
                std::get<T>(variant) = std::invoke(f, std::get<T>(std::forward<TVariant>(variant)));
            }
            return std::forward<TVariant>(variant);
        };
    }

    /**
     * @brief Conditionally transform a variant's value inside a monadic container
     *
     * The mtransform_when combinator works on monadic containers (like optional or expected)
     * that hold variants. If the container has a value and that variant holds the specified
     * type T, it applies the transformation function to that value. The container with its
     * (potentially transformed) variant is returned.
     *
     * This enables conditional transformations on variants wrapped in monadic types,
     * maintaining the monadic context throughout the pipeline.
     *
     * @tparam T The alternative type to check for and transform in the wrapped variant
     * @tparam TMonad The monadic container type (deduced)
     * @tparam F The transformation function type (deduced)
     * @param monad The monadic container holding a variant
     * @param func The transformation function to apply if the variant holds type T
     * @return The monadic container (potentially transformed). Lvalue inputs are returned by
     *         reference; rvalue inputs are returned by value to prevent dangling references.
     *
     * @section Direct Usage
     * @code
     * using MyVariant = fxt::variant<int, std::string>;
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * // Transform only if opt has value and holds int
     * auto result = fxt::mtransform_when<int>(opt, [](int x) {
     *     return x * 2;
     * }); // opt now contains variant holding 84
     *
     * fxt::optional<MyVariant> empty;
     * fxt::mtransform_when<int>(empty, [](int x) {
     *     return x * 2;
     * }); // empty unchanged (container is empty)
     * @endcode
     */
    template<typename T, typename TMonad, typename F>
        requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                 fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type> &&
                 std::invocable<F, T>
    constexpr auto mtransform_when(TMonad&& monad, F&& func)
        -> std::conditional_t<std::is_lvalue_reference_v<TMonad>,
                               TMonad,
                               std::remove_cvref_t<TMonad>>
    {
        if (monad.has_value()) {
            auto& variant = *monad;
            if (std::holds_alternative<T>(variant)) {
                std::get<T>(variant) = std::invoke(std::forward<F>(func), std::get<T>(variant));
            }
        }
        return std::forward<TMonad>(monad);
    }

    /**
     * @brief Returns a curried version of mtransform_when for use in pipelines
     *
     * This overload enables the mtransform_when combinator to be used with the pipe operator,
     * allowing for elegant functional composition when working with variants wrapped
     * in monadic containers.
     *
     * @tparam T The alternative type to check for and transform in the wrapped variant
     * @tparam F The transformation function type (deduced)
     * @param func The transformation function to apply if the variant holds type T
     * @return A callable that accepts a monadic container and conditionally transforms it
     *
     * @section Pipeline Usage
     * @code
     * using MyVariant = fxt::variant<int, double, std::string>;
     *
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * opt | fxt::mtransform_when<int>([](int x) { return x * 2; })
     *     | fxt::mtransform_when<double>([](double x) { return x * 3.0; })
     *     | fxt::mtransform_when<std::string>([](std::string s) { return s + "!"; });
     * // opt now contains variant holding 84 (only int transformation applied)
     *
     * // Complex pipeline with error handling and transformations
     * auto result = parse_variant()
     *             | fxt::transform(validate_variant)
     *             | fxt::mtransform_when<int>([](int x) { return x + 100; })
     *             | fxt::mtransform_when<double>([](double x) { return x * 2.5; })
     *             | fxt::mwhen<int>([](int x) { log_int_value(x); })
     *             | fxt::and_then(save_to_database);
     * @endcode
     */
    template<typename T, typename F>
    constexpr auto mtransform_when(F&& func)
    {
        return [f = std::forward<F>(func)]<typename TMonad>(TMonad&& monad)
            -> std::conditional_t<std::is_lvalue_reference_v<TMonad>,
                                  TMonad,
                                  std::remove_cvref_t<TMonad>>
            requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                     fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type> &&
                     std::invocable<F, T>
        {
            if (monad.has_value()) {
                auto& variant = *monad;
                if (std::holds_alternative<T>(variant)) {
                    std::get<T>(variant) = std::invoke(f, std::get<T>(variant));
                }
            }
            return std::forward<TMonad>(monad);
        };
    }

} // namespace fxt

