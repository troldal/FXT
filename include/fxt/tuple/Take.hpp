//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <tuple>
#include <utility>

namespace fxt
{
    /**
     * @brief Takes the first X elements from a tuple
     *
     * This function object creates a new tuple that contains only the first X elements
     * of the input tuple. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     *
     * @tparam X The number of elements to take from the beginning of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple containing the first X elements of the input tuple
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @section Usage
     * @code
     * // With lvalue
     * auto t = std::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::take<2>(t);
     * // result is std::tuple<int, double>{1, 2.0}
     *
     * // With rvalue
     * auto result2 = fxt::take<2>(std::make_tuple(10, 20, 30));
     * // result2 is std::tuple<int, int>{10, 20}
     *
     * // In a pipeline
     * auto result3 = std::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::take<2>(std::move(t)); };
     * // result3 is std::tuple<int, int>{1, 2}
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mtake = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::make_tuple(std::get<Indices>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<X>{});
    };

    /**
     * @brief Takes the last X elements from a tuple
     *
     * This function object creates a new tuple that contains only the last X elements
     * of the input tuple. Perfect forwarding is preserved for the tuple elements.
     * Supports both lvalue and rvalue references.
     *
     * @tparam X The number of elements to take from the end of the tuple
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple containing the last X elements of the input tuple
     *
     * @throws static_assert If X exceeds the size of the tuple
     *
     * @section Usage
     * @code
     * // With lvalue
     * auto t = std::make_tuple(1, 2.0, "three", 'f');
     * auto result = fxt::take_last<2>(t);
     * // result is std::tuple<const char*, char>{"three", 'f'}
     *
     * // With rvalue
     * auto result2 = fxt::take_last<2>(std::make_tuple(10, 20, 30));
     * // result2 is std::tuple<int, int>{20, 30}
     *
     * // In a pipeline
     * auto result3 = std::make_tuple(1, 2, 3, 4)
     *              | [](auto t) { return fxt::take_last<2>(std::move(t)); };
     * // result3 is std::tuple<int, int>{3, 4}
     * @endcode
     */
    template<std::size_t X>
    inline constexpr auto mtake_last = []<typename Tuple>(Tuple&& tpl) {
        constexpr std::size_t tupleSize = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        static_assert(X <= tupleSize, "Cannot take more elements than the tuple size");

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            return std::make_tuple(std::get<Indices + (tupleSize - X)>(std::forward<Tuple>(tpl))...);
        }(std::make_index_sequence<X>{});
    };

}    // namespace fxt