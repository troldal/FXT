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

#include "Expected.hpp"
#include "Optional.hpp"

// Operators are in global namespace so they can be found via ADL (Argument Dependent Lookup)

// operator|| overloads intentionally evaluate both arguments (function call semantics).
// -Weffc++ warns about this because the built-in || short-circuits; suppress for this file.
#ifdef __GNUC__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Weffc++"
#endif

// expected || expected: const lvalue references
template<typename TV, typename TE>
auto operator||(const fxt::expected<TV, TE>& v1, const fxt::expected<TV, TE>& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? v1 : v2;
}

// expected || expected: rvalue references (move optimization)
template<typename TV, typename TE>
auto operator||(fxt::expected<TV, TE>&& v1, fxt::expected<TV, TE>&& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? std::move(v1) : std::move(v2);
}

// expected || expected: mixed lvalue || rvalue
template<typename TV, typename TE>
auto operator||(const fxt::expected<TV, TE>& v1, fxt::expected<TV, TE>&& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? v1 : std::move(v2);
}

// expected || expected: mixed rvalue || lvalue
template<typename TV, typename TE>
auto operator||(fxt::expected<TV, TE>&& v1, const fxt::expected<TV, TE>& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? std::move(v1) : v2;
}

// expected || value: const lvalue expected
template<typename TV, typename TE, typename U>
  requires std::convertible_to<std::decay_t<U>, TV>
auto operator||(const fxt::expected<TV, TE>& v1, U&& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? v1 : fxt::expected<TV, TE>(std::forward<U>(v2));
}

// expected || value: rvalue expected
template<typename TV, typename TE, typename U>
  requires std::convertible_to<std::decay_t<U>, TV>
auto operator||(fxt::expected<TV, TE>&& v1, U&& v2) -> fxt::expected<TV, TE>
{
  return v1.has_value() ? std::move(v1) : fxt::expected<TV, TE>(std::forward<U>(v2));
}

// optional || optional: const lvalue references
template<typename T>
auto operator||(const fxt::optional<T>& v1, const fxt::optional<T>& v2) -> fxt::optional<T>
{
  return v1.has_value() ? v1 : v2;
}

// optional || optional: rvalue references (move optimization)
template<typename T>
auto operator||(fxt::optional<T>&& v1, fxt::optional<T>&& v2) -> fxt::optional<T>
{
  return v1.has_value() ? std::move(v1) : std::move(v2);
}

// optional || optional: mixed lvalue || rvalue
template<typename T>
auto operator||(const fxt::optional<T>& v1, fxt::optional<T>&& v2) -> fxt::optional<T>
{
  return v1.has_value() ? v1 : std::move(v2);
}

// optional || optional: mixed rvalue || lvalue
template<typename T>
auto operator||(fxt::optional<T>&& v1, const fxt::optional<T>& v2) -> fxt::optional<T>
{
  return v1.has_value() ? std::move(v1) : v2;
}

// optional || value: const lvalue optional
template<typename T, typename U>
  requires std::convertible_to<std::decay_t<U>, T>
auto operator||(const fxt::optional<T>& v1, U&& v2) -> fxt::optional<T>
{
  return v1.has_value() ? v1 : fxt::optional<T>(std::forward<U>(v2));
}

// optional || value: rvalue optional
template<typename T, typename U>
  requires std::convertible_to<std::decay_t<U>, T>
auto operator||(fxt::optional<T>&& v1, U&& v2) -> fxt::optional<T>
{
  return v1.has_value() ? std::move(v1) : fxt::optional<T>(std::forward<U>(v2));
}

#ifdef __GNUC__
#  pragma GCC diagnostic pop
#endif
