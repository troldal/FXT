//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <tuple>
#include <utility>

namespace fxt
{
    /**
     * @brief Drops the first X elements from a tuple
     *
     * This function object creates a new tuple that contains all elements of the input tuple
     * except for the first X elements. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     *
     * @tparam X The number of elements to drop from the beginning of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple containing all but the first X elements of the input tuple
     *
     * @section Usage
     * @code
     * // With lvalue
     * auto t = std::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::drop<2>(t);
     * // result is std::tuple<int, double, const char*, char>{"three", 'f'}
     *
     * // With rvalue
     * auto result2 = fxt::drop<1>(std::make_tuple(10, 20, 30));
     * // result2 is std::tuple<int, int>{20, 30}
     *
     * // In a pipeline
     * auto result3 = std::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::drop<2>(std::move(t)); };
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mdrop = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::make_tuple(std::get<Indices + X>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<tupleSize - X>{});
    };

    /**
     * @brief Drops the last X elements from a tuple
     *
     * This function object creates a new tuple that contains all elements of the input tuple
     * except for the last X elements. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     *
     * @tparam X The number of elements to drop from the end of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple containing all but the last X elements of the input tuple
     *
     * @section Usage
     * @code
     * // With lvalue
     * auto t = std::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::drop_last<2>(t);
     * // result is std::tuple<int, double>{1, 2.0}
     *
     * // With rvalue
     * auto result2 = fxt::drop_last<1>(std::make_tuple(10, 20, 30));
     * // result2 is std::tuple<int, int>{10, 20}
     *
     * // In a pipeline
     * auto result3 = std::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::drop_last<2>(std::move(t)); };
     * // result3 is std::tuple<int, int>{1, 2}
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mdrop_last = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot drop more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::make_tuple(std::get<Indices>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<tupleSize - X>{});
    };
}    // namespace fxt