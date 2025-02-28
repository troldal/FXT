//
// Created by kenne on 12/02/2025.
//

#pragma once

#include <concepts>
#include <functional>
#include <string>

namespace fxt
{

    template<typename T, typename Function>
        requires(not std::ranges::range<T> and std::invocable<Function, T>)
    constexpr auto operator|(T&& t, Function&& f) -> std::invoke_result_t<Function, T>
    {
        return std::invoke(std::forward<Function>(f), std::forward<T>(t));
    }
}    // namespace fxt