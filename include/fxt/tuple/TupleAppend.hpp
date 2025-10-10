//
// Created by kenne on 10-10-2025.
//

#pragma once

#include "TupleSize.hpp"

namespace fxt::impl
{
    // Helper to detect if a type is fxt::tuple (std::tuple)
    template<typename T>
    struct is_fxt_tuple : std::false_type {};

    template<typename... Ts>
    struct is_fxt_tuple<fxt::tuple<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_fxt_tuple_v = is_fxt_tuple<std::remove_cvref_t<T>>::value;

    // Helper to detect if a type is fxt::flat_tuple
    template<typename T>
    struct is_flat_tuple : std::false_type {};

    template<typename... Ts>
    struct is_flat_tuple<flat_tuple<Ts...>> : std::true_type {};

    template<typename T>
    inline constexpr bool is_flat_tuple_v = is_flat_tuple<std::remove_cvref_t<T>>::value;

    // Helper to detect if a type is a tuple-like type (either fxt::tuple or fxt::flat_tuple)
    template<typename T>
    inline constexpr bool is_tuple_like_v = is_fxt_tuple_v<T> || is_flat_tuple_v<T>;

    // Generic append implementation that works for both tuple types - single value
    template<class Tuple, class U, std::size_t... I>
    auto append_impl(Tuple&& t, U&& u, std::index_sequence<I...>)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>..., std::decay_t<U>>;
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<U>(u));
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) {
                return flat_tuple<std::decay_t<Ts>..., std::decay_t<U>>{};
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<U>(u));
        }
    }

    // Generic append implementation that works for both tuple types - multiple values
    template<class Tuple, class... Us, std::size_t... I>
    auto append_impl_variadic(Tuple&& t, std::index_sequence<I...>, Us&&... us)
    {
        // Determine the result tuple type based on input
        if constexpr (is_fxt_tuple_v<Tuple>) {
            // For fxt::tuple (std::tuple)
            using ResultType = fxt::tuple<std::decay_t<std::tuple_element_t<I, std::remove_reference_t<Tuple>>>..., std::decay_t<Us>...>;
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<Us>(us)...);
        } else if constexpr (is_flat_tuple_v<Tuple>) {
            // For fxt::flat_tuple - extract element types from the tuple
            using TupleType = std::remove_cvref_t<Tuple>;
            using ResultType = decltype([&]<typename... Ts>(flat_tuple<Ts...>*) {
                return flat_tuple<std::decay_t<Ts>..., std::decay_t<Us>...>{};
            }(static_cast<TupleType*>(nullptr)));
            return ResultType(fxt::get<I>(std::forward<Tuple>(t))..., std::forward<Us>(us)...);
        }
    }

}    // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Append one or more elements to a tuple
     *
     * Generic overload that works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Uses fxt::tuple_size_v to determine the tuple size, which works for both types.
     * Accepts at least one element to append, but can append multiple elements at once.
     *
     * @tparam Tuple The tuple type (fxt::tuple or fxt::flat_tuple)
     * @tparam U The type of the first element to append
     * @tparam Us The types of additional elements to append
     * @param t The tuple to append to
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A new tuple with all elements appended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2};
     * auto result = fxt::tuple_append(t, 3); // fxt::tuple{1, 2, 3}
     * auto result2 = fxt::tuple_append(t, 3, 4, 5); // fxt::tuple{1, 2, 3, 4, 5}
     * @endcode
     */
    template<class Tuple, class U, class... Us>
        requires impl::is_tuple_like_v<std::remove_cvref_t<Tuple>>
    auto tuple_append(Tuple&& t, U&& u, Us&&... us)
    {
        constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
        if constexpr (sizeof...(Us) == 0) {
            // Single element append
            return impl::append_impl(std::forward<Tuple>(t), std::forward<U>(u), std::make_index_sequence<N>{});
        } else {
            // Multiple elements append
            return impl::append_impl_variadic(std::forward<Tuple>(t), std::make_index_sequence<N>{}, std::forward<U>(u), std::forward<Us>(us)...);
        }
    }

    /**
     * @brief Append one or more elements to a tuple (curried version for pipeline usage)
     *
     * Returns a lambda that appends the given elements to a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple (std::tuple) and fxt::flat_tuple.
     * Accepts at least one element to append, but can append multiple elements at once.
     *
     * @tparam U The type of the first element to append
     * @tparam Us The types of additional elements to append
     * @param u The first element to append
     * @param us Additional elements to append
     * @return A lambda that takes a tuple and returns a new tuple with all elements appended
     *
     * Example:
     * @code
     * auto t = fxt::tuple{1, 2, 3};
     * auto result = t | fxt::tuple_append(4); // fxt::tuple{1, 2, 3, 4}
     * auto result2 = t | fxt::tuple_append(4, 5, 6); // fxt::tuple{1, 2, 3, 4, 5, 6}
     *
     * auto ft = fxt::flat_tuple<double, double>{1.0, 2.0};
     * auto result3 = ft | fxt::tuple_append(3.0, 4.0); // fxt::flat_tuple{1.0, 2.0, 3.0, 4.0}
     * @endcode
     */
    template<class U, class... Us>
        requires (!impl::is_tuple_like_v<std::remove_cvref_t<U>>)
    auto tuple_append(U&& u, Us&&... us)
    {
        if constexpr (sizeof...(Us) == 0) {
            // Single element version
            return [value = std::forward<U>(u)]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                return impl::append_impl(std::forward<Tuple>(t), std::forward<U>(value), std::make_index_sequence<N>{});
            };
        } else {
            // Multiple elements version - capture values in a tuple and unpack them
            return [values = fxt::tuple{std::forward<U>(u), std::forward<Us>(us)...}]<class Tuple>(Tuple&& t) mutable {
                constexpr std::size_t N = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;
                constexpr std::size_t M = sizeof...(Us) + 1;

                // Unpack the captured tuple and pass to append_impl_variadic
                return [&t, &values]<std::size_t... I>(std::index_sequence<I...>) {
                    return impl::append_impl_variadic(
                        std::forward<Tuple>(t),
                        std::make_index_sequence<N>{},
                        std::move(fxt::get<I>(values))...
                    );
                }(std::make_index_sequence<M>{});
            };
        }
    }

}    // namespace fxt
