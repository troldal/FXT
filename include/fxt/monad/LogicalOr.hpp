//
// Created by kenne on 28/02/2025.
//

#pragma once

#include "Expected.hpp"
#include "Optional.hpp"

// Operators are in global namespace so they can be found via ADL (Argument Dependent Lookup)

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