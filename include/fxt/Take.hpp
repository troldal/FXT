//
// Created by kenne on 26/02/2025.
//

#pragma once
#include <tuple>

namespace fxt
{

    namespace impl
    {
        template<std::size_t X, typename Tuple, std::size_t... Indices>
        auto take_impl(Tuple&& tpl, std::index_sequence<Indices...>)
        {
            return std::make_tuple(std::get<Indices>(std::forward<Tuple>(tpl))...);
        }
    }    // namespace impl

    template<std::size_t X, typename Tuple>
    auto take(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");
        return impl::take_impl<X>(std::forward<Tuple>(tpl), std::make_index_sequence<X> {});
    }

}    // namespace fxt