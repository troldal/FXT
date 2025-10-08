//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <functional>
#include <utility>

namespace fxt
{
    /**
     * @brief A recursive lambda that enables automatic currying of functions
     *
     * This lambda transforms any function into a curried version that can be
     * partially applied. When all required arguments are supplied, the function
     * executes. Otherwise, it returns a new function that accepts the remaining
     * arguments.
     *
     * @tparam F Type of the function to curry
     * @param curry Self-reference for recursive calls
     * @param f The function to curry
     * @return A curried version of the function
     *
     * The implementation uses deducing `this` and SFINAE via `requires` to determine
     * whether the function can be called with the provided arguments. If not enough
     * arguments are provided, it returns a new function using std::bind_front.
     *
     * @example
     *   auto add = [](int a, int b) { return a + b; };
     *   auto curriedAdd = fxt::curry(add);
     *   auto add5 = curriedAdd(5);
     *   int result = add5(10); // 15
     */
    inline auto curry = []<typename F>(this auto& curry, F&& f) {
        return [curry, f = std::forward<F>(f)]<typename Self, typename... Ts>(this Self&&, Ts&&... ts) -> decltype(auto) {
            if constexpr (requires { std::forward_like<Self>(f)(std::forward<Ts>(ts)...); }) {
                return std::forward_like<Self>(f)(std::forward<Ts>(ts)...);
            }
            else {
                return curry(std::bind_front(std::forward_like<Self>(f), std::forward<Ts>(ts)...));
            }
        };
    };

}    // namespace fxt