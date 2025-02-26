//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <functional>

namespace fxt
{

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