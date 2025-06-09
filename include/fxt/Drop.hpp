//
// Created by kenne on 26/02/2025.
//

#pragma once

namespace fxt
{
    namespace impl
    {
        template<std::size_t X, typename Tuple, std::size_t... Indices>
        auto drop_impl(Tuple&& tpl, std::index_sequence<Indices...>)
        {
            return std::make_tuple(std::get<Indices + X>(std::forward<Tuple>(tpl))...);
        }
    }    // namespace impl

    /**
     * @brief Drops the first X elements from a tuple
     *
     * This function creates a new tuple that contains all elements of the input tuple
     * except for the first X elements. Perfect forwarding is preserved for the tuple elements.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @tparam Tuple The tuple type
     * @param tpl The input tuple
     * @return A new tuple containing all but the first X elements of the input tuple
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @example
     *   auto t = std::make_tuple(1, 2.0, "three", 'f');
     *   auto result = fxt::drop<2>(t);  // result is std::tuple<const char*, char>{"three", 'f'}
     */
    template<std::size_t X, typename Tuple>
    auto drop(Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");
        return impl::drop_impl<X>(std::forward<Tuple>(tpl), std::make_index_sequence<tupleSize - X> {});
    }
}    // namespace fxt