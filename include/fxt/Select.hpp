//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <cstddef>
#include <tuple>

namespace fxt
{

    /**
     * @brief Extract multiple elements from a tuple inside an expected object by index
     *
     * Creates a function that extracts multiple elements at the specified indices from a tuple
     * contained within an fxt::expected object. The extracted elements are combined into a new tuple,
     * which is wrapped in an expected object, preserving the error handling semantics.
     *
     * @tparam Is The indices of the elements to extract from the tuple
     * @return A function that transforms an expected<tuple> to extract elements at specified indices
     *
     * @throws static_assert If no indices are provided
     *
     * @example
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = fxt::select<0, 2>()(exp);  // result contains std::tuple<int, std::string>{1, "three"}
     */
    template<size_t... Is>
    auto select()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            static_assert(sizeof...(Is) >= 1, "At least one index must be provided");
            return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Is>(t)...); });
        };
    }

    /**
     * @brief Extract multiple elements from a tuple inside an expected object by type
     *
     * Creates a function that extracts multiple elements of the specified types from a tuple
     * contained within an fxt::expected object. The extracted elements are combined into a new tuple,
     * which is wrapped in an expected object, preserving the error handling semantics.
     *
     * @tparam Ts The types of the elements to extract from the tuple
     * @return A function that transforms an expected<tuple> to extract elements of specified types
     *
     * @throws static_assert If no types are provided
     * @note The tuple must contain exactly one element of each specified type, otherwise std::get will fail
     *
     * @example
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = fxt::select<int, std::string>()(exp);  // result contains std::tuple<int, std::string>{1, "three"}
     */
    template<typename... Ts>
    auto select()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            static_assert(sizeof...(Ts) >= 1, "At least one type must be provided");
            return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Ts>(t)...); });
        };
    }

}    // namespace fxt