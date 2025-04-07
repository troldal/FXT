//
// Created by kenne on 07/04/2025.
//

#pragma once

#ifdef FXT_USE_TL_OPTIONAL
#    include <tl/optional.hpp>
namespace fxt
{
  template<typename T>
  using optional = tl::optional<T>;
}
#else
#    include <optional>

namespace fxt
{
  template<typename T>
  using optional = std::optional<T>;
}

#endif
