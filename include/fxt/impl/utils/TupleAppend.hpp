//
// Created by kenne on 25/02/2025.
//

#pragma once

#include "../concepts/IsTuple.hpp"

namespace fxt::impl
{

    template<typename A, typename B>
    auto tuple_append(A&& a, B&& b)
    {
        return std::tuple_cat(impl::tuple_from(std::forward<A>(a)), impl::tuple_from(std::forward<B>(b)));
    }
}    // namespace fxt::impl