//
// Created by kenne on 11-10-2025.
//

#pragma once

#include "../tuples/Tuple.hpp"
#include "../tuples/FlatTuple.hpp"
#include <type_traits>

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

} // namespace fxt::impl

namespace fxt
{
    /**
     * @brief Concept to check if a type is a tuple-like type
     *
     * A tuple-like type is either an fxt::tuple (std::tuple) or an fxt::flat_tuple.
     * This concept can be used to constrain template parameters to only accept tuple types.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::tuple_like T>
     * void process_tuple(T&& tpl) {
     *     // Works with both fxt::tuple and fxt::flat_tuple
     * }
     *
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto ft = fxt::make_flat_tuple(1.0, 2.0);
     * process_tuple(t);  // OK
     * process_tuple(ft); // OK
     * process_tuple(42); // Error: 42 is not tuple_like
     * @endcode
     */
    template<typename T>
    concept tuple_like = impl::is_tuple_like_v<std::remove_cvref_t<T>>;

} // namespace fxt

