//
// Created by kenne on 28/02/2025.
//

#pragma once

namespace fxt
{
  template<typename TV, typename TE>
  auto operator||(const std::expected<TV, TE>& v1, const std::expected<TV, TE>& v2)
  {
    return v1.has_value() ? v1 : v2;
  }
} // namespace fxt