//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <tuple>

namespace fxt
{

    /**
     * @brief Get element at the specified index from a tuple inside an expected object
     *
     * Creates a function that extracts the element at the Ith position of a tuple contained
     * within an fxt::expected object. The result is wrapped in a new expected object,
     * preserving the error handling semantics.
     *
     * @tparam I The index of the element to extract from the tuple
     * @return A function that transforms an expected<tuple> to extract the Ith element
     *
     * @example
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = fxt::get<0>()(exp);  // result contains 1
     */
    template<size_t I>
    auto get()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            return tuple.transform([](const TTuple& t) { return std::get<I>(t); });
        };
    }

    /**
     * @brief Get element of the specified type from a tuple inside an expected object
     *
     * Creates a function that extracts the element of type T from a tuple contained
     * within an fxt::expected object. The result is wrapped in a new expected object,
     * preserving the error handling semantics.
     *
     * @tparam T The type of the element to extract from the tuple
     * @return A function that transforms an expected<tuple> to extract the element of type T
     *
     * @note The tuple must contain exactly one element of type T, otherwise std::get will fail
     *
     * @example
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = fxt::get<std::string>()(exp);  // result contains "three"
     */
    template<typename T>
    auto get()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
        };
    }

}    // namespace fxt