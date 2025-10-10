//
// Created by kenne on 10-10-2025.
//

#pragma once

#include <tuple>

namespace fxt
{
    /**
     * @brief Alias for std::tuple
     *
     * Provides a consistent fxt namespace interface for tuple types,
     * allowing users to write fxt::tuple instead of std::tuple.
     */
    template<typename... Ts>
    using tuple = std::tuple<Ts...>;

    /**
     * @brief Get element from tuple by index
     *
     * Forwards to std::get for std::tuple. Provides a consistent fxt::get
     * interface that works with both fxt::tuple and fxt::flat_tuple.
     *
     * @tparam I Index of the element to get
     * @param t Tuple to get element from
     * @return Reference to the element at index I
     */
    template<std::size_t I, typename... Ts>
    constexpr auto& get(tuple<Ts...>& t) noexcept
    {
        return std::get<I>(t);
    }

    template<std::size_t I, typename... Ts>
    constexpr const auto& get(const tuple<Ts...>& t) noexcept
    {
        return std::get<I>(t);
    }

    template<std::size_t I, typename... Ts>
    constexpr auto&& get(tuple<Ts...>&& t) noexcept
    {
        return std::get<I>(std::move(t));
    }

    template<std::size_t I, typename... Ts>
    constexpr const auto&& get(const tuple<Ts...>&& t) noexcept
    {
        return std::get<I>(std::move(t));
    }

}    // namespace fxt
