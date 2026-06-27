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

namespace fxt
{
    /**
     * @brief Check if a variant currently holds a specific alternative type
     *
     * Direct alias for std::holds_alternative, lifted into the fxt namespace for
     * consistency. Checks whether the variant currently holds the specified type T.
     *
     * @tparam T The alternative type to check for
     * @tparam Types The variant's alternative types (deduced)
     * @param v The variant to check
     * @return true if the variant currently holds the alternative T, false otherwise
     *
     * @code
     * fxt::variant<int, double, std::string> v{42};
     * bool has_int    = fxt::holds_alternative<int>(v);     // true
     * bool has_double = fxt::holds_alternative<double>(v);  // false
     * @endcode
     */
    using std::holds_alternative;

    /**
     * @brief Returns a lambda that checks for a specific alternative (for use with pipe operator)
     *
     * This nullary overload returns a callable suitable for the pipe operator,
     * enabling holds_alternative to participate in variant processing pipelines.
     *
     * @tparam T The alternative type to check for
     * @return A callable that takes a variant and returns bool
     *
     * @code
     * fxt::variant<int, double, std::string> v{42};
     * bool has_int = v | fxt::holds_alternative<int>();  // true
     *
     * // In a pipeline
     * auto result = get_variant()
     *             | fxt::when<ErrorType>([](const auto& e) { log(e); })
     *             | fxt::holds_alternative<SuccessType>();
     * @endcode
     */
    template<typename T>
    constexpr auto holds_alternative()
    {
        return []<typename TVariant>(TVariant&& variant)
            requires fxt::variant_like<std::remove_cvref_t<TVariant>>
        {
            return std::holds_alternative<T>(std::forward<TVariant>(variant));
        };
    }

    /**
     * @brief Check if a monadic container holds a variant with a specific alternative
     *
     * Returns true only if the monad has a value and that variant holds type T.
     * Returns false if the monad is empty, or if the variant holds a different alternative.
     *
     * @tparam T The alternative type to check for in the wrapped variant
     * @tparam TMonad The monadic container type (deduced)
     * @param monad The monadic container holding a variant
     * @return true if the monad has a value and its variant holds T, false otherwise
     *
     * @code
     * using MyVariant = fxt::variant<int, std::string>;
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * bool yes = fxt::mholds_alternative<int>(opt);     // true
     * bool no  = fxt::mholds_alternative<std::string>(opt);  // false
     *
     * fxt::optional<MyVariant> empty;
     * bool also_no = fxt::mholds_alternative<int>(empty);  // false (monad is empty)
     * @endcode
     */
    template<typename T, typename TMonad>
        requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                 fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type>
    constexpr bool mholds_alternative(TMonad&& monad)
    {
        return monad.has_value() && std::holds_alternative<T>(*monad);
    }

    /**
     * @brief Returns a lambda that checks a monadic variant for a specific alternative (for use with pipe operator)
     *
     * This nullary overload returns a callable suitable for the pipe operator,
     * enabling mholds_alternative to participate in monadic variant pipelines.
     *
     * @tparam T The alternative type to check for in the wrapped variant
     * @return A callable that takes a monadic container and returns bool
     *
     * @code
     * using MyVariant = fxt::variant<int, double, std::string>;
     * fxt::optional<MyVariant> opt = MyVariant{42};
     *
     * bool has_int = opt | fxt::mholds_alternative<int>();  // true
     *
     * // In a pipeline
     * auto ok = parse_variant()
     *         | fxt::mwhen<ErrorType>([](const auto& e) { log(e); })
     *         | fxt::mholds_alternative<SuccessType>();
     * @endcode
     */
    template<typename T>
    constexpr auto mholds_alternative()
    {
        return []<typename TMonad>(TMonad&& monad)
            requires fxt::monad_like<std::remove_cvref_t<TMonad>> &&
                     fxt::variant_like<typename std::remove_cvref_t<TMonad>::value_type>
        {
            return monad.has_value() && std::holds_alternative<T>(*monad);
        };
    }

} // namespace fxt
