//
// Created by kenne on 24/02/2025.
//

#pragma once

#include "../monad/Expected.hpp"
#include "../monad/Optional.hpp"
#include "../utils/Overload.hpp"
#include <tuple>

namespace fxt
{

    /**
     * @brief Get element at the specified index from a tuple inside a monadic container
     *
     * Creates a function that extracts the element at the Ith position of a tuple contained
     * within an fxt::expected or fxt::optional object. The result is wrapped in a new container
     * of the same type, preserving the error handling or empty state semantics.
     *
     * @tparam I The index of the element to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract the Ith element
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::get<0>();  // result contains 1
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::get<0>();  // result contains 1
     */
    template<size_t I>
    auto get()
    {
        return overload{
            // Handle expected-like containers
            []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::get<I>(t); });
            },
            // Handle optional-like containers
            []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::get<I>(t); });
            }
        };
    }

    /**
     * @brief Get element of the specified type from a tuple inside a monadic container
     *
     * Creates a function that extracts the element of type T from a tuple contained
     * within an fxt::expected or fxt::optional object. The result is wrapped in a new
     * container of the same type, preserving the error handling or empty state semantics.
     *
     * @tparam T The type of the element to extract from the tuple
     * @return A function that transforms a monadic container<tuple> to extract the element of type T
     *
     * @note The tuple must contain exactly one element of type T, otherwise std::get will fail
     *
     * @example
     *   // With expected
     *   auto exp = fxt::expected<std::tuple<int, double, std::string>, Error>{std::make_tuple(1, 2.0, "three")};
     *   auto result = exp | fxt::get<std::string>();  // result contains "three"
     *
     *   // With optional
     *   auto opt = fxt::optional<std::tuple<int, double, std::string>>{std::make_tuple(1, 2.0, "three")};
     *   auto result = opt | fxt::get<std::string>();  // result contains "three"
     */
    template<typename T>
    auto get()
    {
        return overload{
            // Handle expected-like containers
            []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
            },
            // Handle optional-like containers
            []<typename TTuple>(const fxt::optional<TTuple>& tuple) {
                return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
            }
        };
    }

}    // namespace fxt

