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

    /**
     * @brief Takes the first X elements from a tuple
     *
     * This function creates a new tuple that contains only the first X elements
     * of the input tuple. Perfect forwarding is preserved for the tuple elements.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @tparam Tuple The tuple type
     * @param tpl The input tuple
     * @return A new tuple containing the first X elements of the input tuple
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @example
     *   auto t = std::make_tuple(1, 2.0, "three", 'f');
     *   auto result = fxt::take<2>(t);  // result is std::tuple<int, double>{1, 2.0}
     */
    template<std::size_t X, typename Tuple>
    auto take(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");
        return impl::take_impl<X>(std::forward<Tuple>(tpl), std::make_index_sequence<X> {});
    }

}    // namespace fxt