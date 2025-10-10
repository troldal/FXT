//
// Created by kenne on 10-10-2025.
//

#pragma once

#include <array>
#include <utility>
#include <variant>
#include <concepts>

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
        explicit flat_tuple(std::index_sequence<Is...>, Ts... args)
            : values{variant_t(std::in_place_index<Is>, indexed<Is, Ts>{std::move(args)})...} {}

        // Default constructor for empty tuples
        flat_tuple() = default;

        // Constructor that forwards to the index_sequence version (only enabled for non-empty)
        template<typename... Args>
            requires (sizeof...(Args) > 0 && sizeof...(Args) == sizeof...(Ts))
        explicit flat_tuple(Args&&... args)
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

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (lvalue reference)
     *
     * Allows piping a flat_tuple to a callable function, enabling functional-style composition.
     * The callable receives the flat_tuple and returns the result.
     *
     * @tparam Ts Types in the flat_tuple
     * @tparam Callable Type of the callable
     * @param tuple The flat_tuple to pipe
     * @param callable The callable to apply to the flat_tuple
     * @return The result of invoking the callable with the flat_tuple
     *
     * @code
     * fxt::flat_tuple<int, double> t{42, 3.14};
     * auto result = t | fxt::get<0>;  // Returns 42
     * @endcode
     */
    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (const lvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>& t, Callable&& c) { std::invoke(std::forward<Callable>(c), t); }
    constexpr auto operator|(const flat_tuple<Ts...>& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), tuple))
    {
        return std::invoke(std::forward<Callable>(callable), tuple);
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

    /**
     * @brief Pipe operator for fxt::flat_tuple with callable (const rvalue reference)
     */
    template<typename... Ts, typename Callable>
    requires requires(const flat_tuple<Ts...>&& t, Callable&& c) { std::invoke(std::forward<Callable>(c), std::move(t)); }
    constexpr auto operator|(const flat_tuple<Ts...>&& tuple, Callable&& callable)
        -> decltype(std::invoke(std::forward<Callable>(callable), std::move(tuple)))
    {
        return std::invoke(std::forward<Callable>(callable), std::move(tuple));
    }

}