//
// Created by kenne on 10-10-2025.
//

#pragma once

#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include <tuple>
#include <type_traits>

namespace fxt
{
    // ========================================================================
    // Primary template - undefined for non-tuple types
    // ========================================================================

    template<std::size_t I, typename T>
    struct tuple_element;

    // ========================================================================
    // Specialization for fxt::tuple (std::tuple)
    // ========================================================================

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, fxt::tuple<Ts...>> : std::tuple_element<I, std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const fxt::tuple<Ts...>> : std::tuple_element<I, const std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, volatile fxt::tuple<Ts...>> : std::tuple_element<I, volatile std::tuple<Ts...>>
    {
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const volatile fxt::tuple<Ts...>> : std::tuple_element<I, const volatile std::tuple<Ts...>>
    {
    };

    // ========================================================================
    // Specialization for fxt::flat_tuple
    // ========================================================================

    namespace impl
    {
        // Helper to extract type at index I from flat_tuple
        template<std::size_t I, typename... Ts>
        struct flat_tuple_element_impl;

        template<std::size_t I, typename T, typename... Ts>
        struct flat_tuple_element_impl<I, T, Ts...>
        {
            using type = typename flat_tuple_element_impl<I - 1, Ts...>::type;
        };

        template<typename T, typename... Ts>
        struct flat_tuple_element_impl<0, T, Ts...>
        {
            using type = T;
        };
    }

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, fxt::flat_tuple<Ts...>>
    {
        using type = typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const fxt::flat_tuple<Ts...>>
    {
        using type = const typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, volatile fxt::flat_tuple<Ts...>>
    {
        using type = volatile typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    template<std::size_t I, typename... Ts>
    struct tuple_element<I, const volatile fxt::flat_tuple<Ts...>>
    {
        using type = const volatile typename impl::flat_tuple_element_impl<I, Ts...>::type;
    };

    // ========================================================================
    // Convenience type alias
    // ========================================================================

    template<std::size_t I, typename T>
    using tuple_element_t = typename tuple_element<I, T>::type;

}    // namespace fxt
