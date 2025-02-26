//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "../concepts/IsTuple.hpp"

namespace fxt::impl {

template<typename A, typename B>
auto tuple_append(A&& a, B&& b)
{
    return std::tuple_cat(tuple_from(std::forward<A>(a)), tuple_from(std::forward<B>(b)));
}
}