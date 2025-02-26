//
// Created by kenne on 23/02/2025.
//

#pragma once

namespace fxt {
    template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
    template<class... Ts> overload(Ts...) -> overload<Ts...>;
}