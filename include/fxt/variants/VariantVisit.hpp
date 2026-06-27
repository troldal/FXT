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
     * @brief Visit a variant with the given visitor (alias for std::visit)
     *
     * This function is a direct alias for std::visit, providing a consistent
     * interface within the fxt namespace for variant visitation.
     *
     * @tparam TVisitor The visitor callable type (deduced)
     * @tparam TVariant The variant type (deduced)
     * @param visitor A callable that can handle all alternatives in the variant
     * @param variant The variant to visit
     * @return The result of invoking the visitor with the variant's active alternative
     *
     * @code
     * fxt::variant<int, double, std::string> v{42};
     * auto result = fxt::visit([](auto&& val) {
     *     return std::to_string(val);
     * }, v);
     * @endcode
     */
    template<typename TVisitor, typename TVariant>
        requires fxt::variant_like<std::remove_cvref_t<TVariant>>
    constexpr decltype(auto) visit(TVisitor&& visitor, TVariant&& variant)
    {
        return std::visit(std::forward<TVisitor>(visitor), std::forward<TVariant>(variant));
    }

    /**
     * @brief Returns a lambda that visits a variant with the given visitor (for use with pipe operator)
     *
     * This overload takes a visitor and returns a lambda that can be used with the pipe operator,
     * enabling functional composition with variants.
     *
     * @tparam TVisitor The visitor callable type (deduced)
     * @param visitor A callable that can handle all alternatives in the variant
     * @return A lambda that takes a variant and applies the visitor to it
     *
     * @code
     * fxt::variant<int, double, std::string> v{3.14};
     * auto result = v | fxt::visit([](auto&& val) {
     *     return std::to_string(val);
     * });
     * @endcode
     */
    template<typename TVisitor>
    constexpr auto visit(TVisitor&& visitor)
    {
        return [visitor = std::forward<TVisitor>(visitor)]<typename TVariant>(TVariant&& variant)
            requires fxt::variant_like<std::remove_cvref_t<TVariant>>
        {
            return std::visit(visitor, std::forward<TVariant>(variant));
        };
    }

    /**
     * @brief Visit a variant contained within a monad with the given visitor
     *
     * This function applies a visitor to a variant that is contained within a monadic
     * container (like fxt::optional or fxt::expected). It uses the monad's transform
     * operation to apply the visit.
     *
     * @tparam TVisitor The visitor callable type (deduced)
     * @tparam TMonad The monad type containing a variant (deduced)
     * @param visitor A callable that can handle all alternatives in the variant
     * @param monad The monad containing a variant
     * @return A monad containing the result of the visitation
     *
     * @code
     * fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
     * auto result = fxt::mvisit([](auto&& val) {
     *     return std::to_string(val);
     * }, opt);
     * // result is fxt::optional<std::string> containing "3.14"
     * @endcode
     */
    template<typename TVisitor, typename TMonad>
        requires monad_like<std::remove_cvref_t<TMonad>>
    constexpr auto mvisit(TVisitor&& visitor, TMonad&& monad)
    {
        return std::forward<TMonad>(monad).transform(visit(std::forward<TVisitor>(visitor)));
    }

    /**
     * @brief Returns a lambda that visits a variant in a monad with the given visitor (for use with pipe operator)
     *
     * This overload takes a visitor and returns a lambda that can be used with the pipe operator,
     * enabling functional composition with monadic variants.
     *
     * @tparam TVisitor The visitor callable type (deduced)
     * @param visitor A callable that can handle all alternatives in the variant
     * @return A lambda that takes a monad containing a variant and applies the visitor to it
     *
     * @code
     * fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
     * auto result = opt | fxt::mvisit([](auto&& val) {
     *     return std::to_string(val);
     * });
     * // result is fxt::optional<std::string> containing "3.14"
     * @endcode
     */
    template<typename TVisitor>
    constexpr auto mvisit(TVisitor&& visitor)
    {
        return [visitor = std::forward<TVisitor>(visitor)]<typename TMonad>(TMonad&& monad)
            requires monad_like<std::remove_cvref_t<TMonad>>
        {
            return std::forward<TMonad>(monad).transform(visit(visitor));
        };
    }
}
