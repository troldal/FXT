//
// Created by kenne on 25/02/2025.
//

#pragma once

#include <tuple>

namespace fxt
{
    // Forward declaration
    template<class... Ts>
    class flat_tuple;

    // Forward declaration of get
    template<size_t I, class... Ts>
    auto& get(flat_tuple<Ts...>& tuple);

    template<size_t I, class... Ts>
    const auto& get(const flat_tuple<Ts...>& tuple);

    template<size_t I, class... Ts>
    auto&& get(flat_tuple<Ts...>&& tuple);

    template<size_t I, class... Ts>
    const auto&& get(const flat_tuple<Ts...>&& tuple);
}

namespace fxt::impl
{
    // Original tuple_append for std::tuple
    template<class Tuple, class U, std::size_t... I>
    auto append_impl(Tuple&& t, U&& u, std::index_sequence<I...>)
    {
        using T = std::tuple<std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>..., std::decay_t<U>>;
        return T(std::get<I>(std::forward<Tuple>(t))..., std::forward<U>(u));
    }

    template<class Tuple, class U>
    auto tuple_append(Tuple&& t, U&& u)
    {
        constexpr std::size_t N = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        return impl::append_impl(std::forward<Tuple>(t), std::forward<U>(u), std::make_index_sequence<N> {});
    }

    // Overload for flat_tuple - extract elements and create new flat_tuple with appended element
    template<class... Ts, class U, std::size_t... I>
    auto append_impl(flat_tuple<Ts...>&& t, U&& u, std::index_sequence<I...>)
    {
        return flat_tuple<std::decay_t<Ts>..., std::decay_t<U>>(
            fxt::get<I>(std::move(t))...,
            std::forward<U>(u)
        );
    }

    template<class... Ts, class U, std::size_t... I>
    auto append_impl(const flat_tuple<Ts...>& t, U&& u, std::index_sequence<I...>)
    {
        return flat_tuple<std::decay_t<Ts>..., std::decay_t<U>>(
            fxt::get<I>(t)...,
            std::forward<U>(u)
        );
    }

    // Overload for flat_tuple
    template<class... Ts, class U>
    auto tuple_append(flat_tuple<Ts...>&& t, U&& u)
    {
        constexpr std::size_t N = sizeof...(Ts);
        return impl::append_impl(std::move(t), std::forward<U>(u), std::make_index_sequence<N>{});
    }

    template<class... Ts, class U>
    auto tuple_append(const flat_tuple<Ts...>& t, U&& u)
    {
        constexpr std::size_t N = sizeof...(Ts);
        return impl::append_impl(t, std::forward<U>(u), std::make_index_sequence<N>{});
    }

}    // namespace fxt::impl