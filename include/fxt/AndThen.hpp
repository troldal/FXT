//
// Created by kenne on 07/04/2025.
//

#pragma once

#include "impl/utils/TupleAppend.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "Overload.hpp"

namespace fxt
{

  struct AndThenWrapper
  {
    template<typename TFunction>
    auto operator()(TFunction&& f) const
    {
      return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
        // if constexpr (impl::expected_like<std::invoke_result_t<TFunction, TValue>>)
        //     return ex.and_then(f);
        // else
        return ex.and_then(f);
      };
    }

  };

  inline constexpr AndThenWrapper and_then = {};
}    // nam