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
#include <type_traits>

namespace fxt::impl
{
    // Helper to detect if a type is fxt::variant (std::variant)
    template<typename T>
    struct is_fxt_variant : std::false_type {};

    template<typename... Ts>
    struct is_fxt_variant<fxt::variant<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_fxt_variant_v = is_fxt_variant<std::remove_cvref_t<T>>::value;

} // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Concept to check if a type is a variant-like type
     *
     * A variant-like type is currently defined as an fxt::variant (std::variant).
     * This concept can be used to constrain template parameters to only accept variant types.
     * Other variant implementations may be added at a later time.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::variant_like T>
     * void process_variant(T&& var) {
     *     // Works with fxt::variant
     * }
     *
     * auto v = fxt::variant<int, double, std::string>(42);
     * process_variant(v);  // OK
     * process_variant(42); // Error: 42 is not variant_like
     * @endcode
     */
    template<typename T>
    concept variant_like = impl::is_fxt_variant_v<std::remove_cvref_t<T>>;

}    // namespace fxt

