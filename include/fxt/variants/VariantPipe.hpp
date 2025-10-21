//
// Created by kenne on 21-10-2025.
//

#pragma once

#include "../concepts/IsVariant.hpp"
#include <functional>

/**
 * @brief Pipe operator for fxt::variant with callable
 *
 * Allows piping a variant to a callable function, enabling functional-style composition.
 * The callable receives the variant and returns the result.
 *
 * @tparam TVariant Type of the variant (deduced)
 * @tparam Callable Type of the callable
 * @param variant The variant to pipe
 * @param function The callable to apply to the variant
 * @return The result of invoking the callable with the variant
 *
 * @code
 * fxt::variant<int, double, std::string> v{42};
 * auto result = v | [](auto&& var) { return std::visit([](auto&& x) { return x; }, var); };
 * @endcode
 */
template<typename TVariant, typename Callable>
    requires fxt::variant_like<std::remove_cvref_t<TVariant>>
        && requires(TVariant&& v, Callable&& c) {
                 std::invoke(std::forward<Callable>(c), std::forward<TVariant>(v));
             }
constexpr auto operator|(TVariant&& variant, Callable&& function)
-> decltype(std::invoke(std::forward<Callable>(function), std::forward<TVariant>(variant)))
{
    return std::invoke(std::forward<Callable>(function), std::forward<TVariant>(variant));
}
