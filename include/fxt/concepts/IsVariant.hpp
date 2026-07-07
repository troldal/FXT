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

#include "../variants/Variant.hpp"
#include "IsMonad.hpp"
#include <concepts>
#include <cstddef>
#include <type_traits>

namespace fxt::impl
{
    // Generic variant-protocol detection: a member .index() returning the
    // active alternative's index, plus an ADL-discoverable get<0> — covers
    // std::variant (and thus fxt::variant), mpark::variant,
    // boost::variant2::variant, and other third-party sum types that follow
    // the same protocol.
    //
    // Deliberately does NOT check visit(...): fxt::visit is itself constrained
    // on fxt::variant_like, so an unqualified visit(f, t) here would consider
    // fxt::visit as a candidate, making this concept's satisfaction depend on
    // itself. get<0>(t) has no such problem — fxt::get (Get.hpp) is an
    // unconstrained `using std::get` plus flat_tuple overloads, neither of
    // which references variant_like.
    template<typename T>
    concept has_variant_protocol = !std::is_reference_v<T> && requires(T& t) {
        { t.index() } -> std::convertible_to<std::size_t>;
        get<0>(t);
    };

} // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Concept to check if a type is a variant-like type
     *
     * A variant-like type is any type that follows the standard variant protocol:
     * a member `.index()` returning the active alternative's index, plus `get<I>`
     * found via ADL. This covers fxt::variant (std::variant), as well as
     * third-party sum types such as mpark::variant and boost::variant2::variant.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::variant_like T>
     * void process_variant(T&& var) {
     *     // Works with fxt::variant, mpark::variant, ...
     * }
     *
     * auto v = fxt::variant<int, double, std::string>(42);
     * process_variant(v);  // OK
     * process_variant(42); // Error: 42 is not variant_like
     * @endcode
     */
    template<typename T>
    concept variant_like = impl::has_variant_protocol<std::remove_cvref_t<T>>;

    /**
     * @brief A monad-like type whose value_type is variant-like.
     *
     * Exposed as a single named concept so that constraints referring to it
     * mangle as a plain concept-id. Spelling the nested requirement inline in a
     * requires-clause — @c variant_like<typename std::remove_cvref_t<M>::value_type>
     * — makes clang-cl's Microsoft-ABI mangler fail with "cannot mangle this
     * dependent name type yet"; hiding the @c typename-specifier inside this
     * concept's definition avoids that (clang mangles the constraint by the
     * concept-id, not its expansion).
     */
    template<typename M>
    concept monad_holding_variant =
        monad_like<std::remove_cvref_t<M>> &&
        variant_like<typename std::remove_cvref_t<M>::value_type>;

}    // namespace fxt

