//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <tuple>
#include <cstddef>

namespace fxt {


    template<size_t I>
    auto get()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            return tuple.transform([](const TTuple& t) { return std::get<I>(t); });
        };
    }

    template<typename T>
    auto get()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            return tuple.transform([](const TTuple& t) { return std::get<T>(t); });
        };
    }

} // namespace fxt