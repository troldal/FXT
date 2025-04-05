//
// Created by kenne on 12/02/2025.
//

#pragma once

#include <concepts>
#include <functional>
#include <string>

namespace fxt
{

    template<typename T, typename TFunction>
        requires(not std::ranges::range<T> and std::invocable<TFunction, T>)
    constexpr auto operator|(T&& t, TFunction&& f) -> std::invoke_result_t<TFunction, T>
    {
        return std::invoke(std::forward<TFunction>(f), std::forward<T>(t));
    }

    // template<typename T, typename TFunction>
    //     requires(not std::ranges::range<T> and std::invocable<TFunction, T>)
    // constexpr auto operator|(const T& t, TFunction&& f) -> std::invoke_result_t<TFunction, T>
    // {
    //     return std::invoke(std::forward<TFunction>(f), t);
    // }

}    // namespace fxt