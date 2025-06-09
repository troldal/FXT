//
// Created by kenne on 23/02/2025.
//

#pragma once

#include "Overload.hpp"
#include "impl/concepts/IsExpected.hpp"

namespace fxt
{

    /**
     * @brief Wrapper for the transform_error operation on expected-like objects
     *
     * TransformErrorWrapper provides a way to chain error transformation operations on fxt::expected objects
     * in a functional style. It creates a higher-order function that applies the
     * provided function to the error inside an expected object using transform_error,
     * but only if the expected object contains an error.
     */
    struct TransformErrorWrapper
    {
        /**
         * @brief Creates a function that applies transform_error with the provided function
         *
         * @tparam TFunction Type of the function to apply via transform_error
         * @param f The function to apply to the expected's error
         * @return A lambda that takes an expected object and applies the function via transform_error
         *
         * The returned lambda preserves perfect forwarding of the function and handles
         * the transform_error operation on any fxt::expected instance. The resulting expected object
         * will contain the transformed error if the original expected contained an error,
         * or the original value if it contained a value.
         */
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.transform_error(f);
            };
        }
    };

    inline constexpr TransformErrorWrapper transform_error = {};
}    // namespace fxt