//
// Created by kenne on 25/02/2025.
//

#pragma once

#include <tuple>

namespace fxt::impl
{
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

}    // namespace fxt::impl