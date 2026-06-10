//
// Created by kenne on 19-10-2025.
//

#pragma once

#include "../concepts/IsTuple.hpp"

/**
 * @brief Pipe operator for fxt::tuple with callable (lvalue reference)
 *
 * Allows piping a tuple to a callable function, enabling functional-style composition.
 * The callable receives the tuple and returns the result.
 *
 * @tparam Ts Types in the tuple
 * @tparam Callable Type of the callable
 * @param tuple The tuple to pipe
 * @param callable The callable to apply to the tuple
 * @return The result of invoking the callable with the tuple
 *
 * @code
 * fxt::tuple<int, double> t{42, 3.14};
 * auto result = t | fxt::get<0>;  // Returns 42
 * @endcode
 */
// template<typename... Ts, typename Callable>
//     requires requires(fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
// constexpr auto operator|(fxt::tuple<Ts...>& tuple, Callable&& callable) -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
// {
//     return std::invoke(std::forward<Callable>(callable), tuple);
// }
//
// /**
//  * @brief Pipe operator for fxt::tuple with callable (const lvalue reference)
//  */
// template<typename... Ts, typename Callable>
//     requires requires(const fxt::tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
// constexpr auto operator|(const fxt::tuple<Ts...>& tuple, Callable&& callable)
//     -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
// {
//     return std::invoke(std::forward<Callable>(callable), tuple);
// }
//
// /**
//  * @brief Pipe operator for fxt::tuple with callable (rvalue reference)
//  */
// template<typename... Ts, typename Callable>
//     requires requires(fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
// constexpr auto operator|(fxt::tuple<Ts...>&& tuple, Callable&& callable)
//     -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
// {
//     return std::invoke(std::forward<Callable>(callable), std::move(tuple));
// }
//
// /**
//  * @brief Pipe operator for fxt::tuple with callable (const rvalue reference)
//  */
// template<typename... Ts, typename Callable>
//     requires requires(const fxt::tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
// constexpr auto operator|(const fxt::tuple<Ts...>&& tuple, Callable&& callable)
//     -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
// {
//     return std::invoke(std::forward<Callable>(callable), std::move(tuple));
// }

// TODO: CLEANUP — delete the commented-out per-value-category overloads above; they are
//       superseded by the forwarding-reference overload below.
// TODO: DESIGN — this operator| lives in the GLOBAL namespace and matches any
//       fxt::tuple (i.e. any std::tuple) piped to any compatible callable. Like
//       LogicalOr.hpp, it is found by unqualified lookup rather than ADL and injects a
//       very broad operator into every translation unit that includes fxt.hpp, which can
//       change overload resolution in unrelated code using std::tuple. Consider scoping it
//       to namespace fxt with an explicit opt-in. Also: missing #include <functional> /
//       <utility> for std::invoke/std::forward, and no license banner (every other header
//       has one).
template<typename TTuple, typename Callable>
    requires fxt::tuple_like<std::remove_cvref_t<TTuple>> && requires(TTuple&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::forward<TTuple>(t)); }
constexpr auto operator|(TTuple&& tuple, Callable&& function)
-> decltype(std::invoke(std::forward<Callable>(function), std::forward<TTuple>(tuple)))
{
    return std::invoke(std::forward<Callable>(function), std::forward<TTuple>(tuple));
}