//
// Created by kenne on 28/02/2025.
//

#pragma once

namespace fxt
{

    struct OrElseWrapper
    {
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const std::expected<TValue, TError>& ex) {
                // if constexpr (impl::IsExpected<std::invoke_result_t<TFunction, TValue>>)
                //   return ex.and_then(f);
                // else
                //   return ex.transform(f);
                return ex.or_else(f);
            };
        }
    };

    inline constexpr OrElseWrapper or_else = {};

}    // namespace fxt