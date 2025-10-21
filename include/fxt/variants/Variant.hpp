//
// Created by kenne on 21-10-2025.
//

#pragma once

#include <concepts>
#include <functional>
#include <variant>

namespace fxt
{
    template<typename... Ts>
    using variant = std::variant<Ts...>;

}    // namespace fxt

