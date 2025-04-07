//
// Created by kenne on 07/04/2025.
//

#pragma once

#ifdef FXT_USE_TL_EXPECTED
#    include <tl/expected.hpp>
namespace fxt
{
    template<typename T, typename E>
    using expected = tl::expected<T, E>;

    template<typename E>
    using unexpected = tl::unexpected<E>;
}
#else
#    include <expected>

namespace fxt
{
    template<typename T, typename E>
    using expected = std::expected<T, E>;

    template<typename E>
    using unexpected = std::unexpected<E>;
}

#endif
