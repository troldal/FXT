//
// Created by kenne on 07/04/2025.
//

#pragma once

#include "Overload.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"

namespace fxt
{

    /**
     * @brief Wrapper for the and_then operation on expected-like objects
     *
     * AndThenWrapper provides a way to chain operations on fxt::expected objects
     * in a functional style. It creates a higher-order function that applies the
     * provided function to the value inside an expected object using and_then,
     * but only if the expected object contains a value.
     */
    struct AndThenWrapper
    {
        /**
         * @brief Creates a function that applies and_then with the provided function
         *
         * @tparam TFunction Type of the function to apply via and_then
         * @param f The function to apply to the expected's value
         * @return A lambda that takes an expected object and applies the function via and_then
         *
         * The returned lambda preserves perfect forwarding of the function and handles
         * the and_then operation on any fxt::expected instance. The function f should
         * return an expected-like type for proper monadic composition.
         */
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.and_then(f);
            };
        }
    };

    inline constexpr AndThenWrapper and_then = {};
}    // namespace fxt