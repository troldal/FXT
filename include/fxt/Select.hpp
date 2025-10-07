//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <cstddef>
#include <tuple>
#include "Expected.hpp"
#include "Optional.hpp"
#include "Overload.hpp"

namespace fxt
{

    /**
     * @brief Extract multiple elements from a tuple inside a monadic container by index
     *
     * Creates a function that extracts multiple elements at the specified indices from a tuple
     * contained within an fxt::expected or fxt::optional object. The extracted elements are combined
     * into a new tuple, which is wrapped in a container of the same type, preserving the error
     * handling or empty state semantics.
     *
     * @tparam Is The indices of the elements to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract elements at specified indices
     *
     * @throws static_assert If no indices are provided
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::select<0, 2>();  // result contains std::tuple<int, std::string>{1, "three"}
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::select<0, 2>();  // result contains std::tuple<int, std::string>{1, "three"}
     */
    template<size_t... Is>
    auto select()
    {
        static_assert(sizeof...(Is) >= 1, "At least one index must be provided");

        return overload{
            // Handle expected-like containers
            []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Is>(t)...); });
            },
            // Handle optional-like containers
            []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Is>(t)...); });
            }
        };
    }

    /**
     * @brief Extract multiple elements from a tuple inside a monadic container by type
     *
     * Creates a function that extracts multiple elements of the specified types from a tuple
     * contained within an fxt::expected or fxt::optional object. The extracted elements are combined
     * into a new tuple, which is wrapped in a container of the same type, preserving the error
     * handling or empty state semantics.
     *
     * @tparam Ts The types of the elements to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract elements of specified types
     *
     * @throws static_assert If no types are provided
     * @note The tuple must contain exactly one element of each specified type, otherwise std::get will fail
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::select<int, std::string>();  // result contains std::tuple<int, std::string>{1, "three"}
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::select<int, std::string>();  // result contains std::tuple<int, std::string>{1, "three"}
     */
    template<typename... Ts>
    auto select()
    {
        static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");

        return overload{
            // Handle expected-like containers
            []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Ts>(t)...); });
            },
            // Handle optional-like containers
            []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Ts>(t)...); });
            }
        };
    }

}    // namespace fxt

