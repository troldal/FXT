//
// Created by kenne on 24/02/2025.
//

#pragma once

#include <tuple>
#include <cstddef>

namespace fxt {


    template<size_t... Is>
    auto select()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            static_assert(sizeof...(Is) >= 1 , "At least one index must be provided");
            // if constexpr (sizeof...(I) == 1)
            //     return tuple.transform([](const TTuple& t) { return std::get<I...>(t); });
            // else
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Is>(t)...); });
        };
    }

    template<typename... Ts>
    auto select()
    {
        return []<typename TTuple, typename TError>(const fxt::expected<TTuple, TError>& tuple) {
            static_assert(sizeof...(Ts) >= 1 , "At least one type must be provided");
            // if constexpr (sizeof...(Ts) == 1)
            //     return tuple.transform([](const TTuple& t) { return std::get<Ts...>(t); });
            // else
                return tuple.transform([](const TTuple& t) { return std::make_tuple(std::get<Ts>(t)...); });
        };
    }

} // namespace fxt