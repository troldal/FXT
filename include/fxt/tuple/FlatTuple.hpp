//
// Created by kenne on 10-10-2025.
//

#pragma once

#include <array>
#include <utility>
#include <variant>

namespace fxt
{

    template<class... Ts>
    class flat_tuple {
        template<size_t I, class T>
        struct indexed { T value; };

        template<size_t I, class... Us>
        struct type_at_impl;

        template<size_t I, class U, class... Us>
        struct type_at_impl<I, U, Us...> {
            using type = typename type_at_impl<I - 1, Us...>::type;
        };

        template<class U, class... Us>
        struct type_at_impl<0, U, Us...> {
            using type = U;
        };

        template<size_t I>
        using type_at = typename type_at_impl<I, Ts...>::type;

        template<class Seq>
        struct make_variant;

        template<size_t... Is>
        struct make_variant<std::index_sequence<Is...>> {
            // Use std::monostate if the parameter pack is empty, otherwise use the indexed types
            using type = std::conditional_t<
                sizeof...(Is) == 0,
                std::variant<std::monostate>,
                std::variant<indexed<Is, Ts>...>
            >;
        };

        using variant_t = typename make_variant<std::index_sequence_for<Ts...>>::type;

        // For empty tuple, use std::array with size 0 (which is valid and has special behavior)
        std::array<variant_t, sizeof...(Ts)> values;

    public:
        // Constructor for non-empty tuples
        template<size_t... Is>
        flat_tuple(std::index_sequence<Is...>, Ts... args)
            : values{variant_t(std::in_place_index<Is>, indexed<Is, Ts>{std::move(args)})...} {}

        // Default constructor for empty tuples
        flat_tuple() = default;

        // Constructor that forwards to the index_sequence version (only enabled for non-empty)
        template<typename... Args>
            requires (sizeof...(Args) > 0 && sizeof...(Args) == sizeof...(Ts))
        flat_tuple(Args&&... args)
            : flat_tuple(std::index_sequence_for<Ts...>{}, std::forward<Args>(args)...) {}

        // template<class F>
        // void visit_all(F&& f) {
        //     [&]<size_t... Is>(std::index_sequence<Is...>) {
        //         (std::forward<F>(f)(std::get<indexed<Is, type_at<Is>>>(values[Is]).value), ...);
        //     }(std::index_sequence_for<Ts...>{});
        // }

        static constexpr size_t size() { return sizeof...(Ts); }

        // Friend declarations for get functions in fxt namespace
        template<size_t I, class... Us>
        friend auto& get(flat_tuple<Us...>& tuple);

        template<size_t I, class... Us>
        friend const auto& get(const flat_tuple<Us...>& tuple);

        template<size_t I, class... Us>
        friend auto&& get(flat_tuple<Us...>&& tuple);

        template<size_t I, class... Us>
        friend const auto&& get(const flat_tuple<Us...>&& tuple);
    };

    // Free functions for element access in fxt namespace (similar to std::get for std::tuple)
    template<size_t I, class... Ts>
    auto& get(flat_tuple<Ts...>& tuple) {
        return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    }

    template<size_t I, class... Ts>
    const auto& get(const flat_tuple<Ts...>& tuple) {
        return std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value;
    }

    template<size_t I, class... Ts>
    auto&& get(flat_tuple<Ts...>&& tuple) {
        return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    }

    template<size_t I, class... Ts>
    const auto&& get(const flat_tuple<Ts...>&& tuple) {
        return std::move(std::get<typename flat_tuple<Ts...>::template indexed<I, typename flat_tuple<Ts...>::template type_at<I>>>(tuple.values[I]).value);
    }

}