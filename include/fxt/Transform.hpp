//
// Created by kenne on 23/02/2025.
//

#pragma once

#include "Overload.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"

namespace fxt
{

    /**
     * @brief Wrapper for the transform operation on expected-like objects
     *
     * TransformWrapper provides a way to chain transformation operations on fxt::expected objects
     * in a functional style. It creates a higher-order function that applies the
     * provided function to the value inside an expected object using transform,
     * but only if the expected object contains a value.
     */
    struct TransformWrapper
    {
        /**
         * @brief Creates a function that applies transform with the provided function
         *
         * @tparam TFunction Type of the function to apply via transform
         * @param f The function to apply to the expected's value
         * @return A lambda that takes an expected object and applies the function via transform
         *
         * The returned lambda preserves perfect forwarding of the function and handles
         * the transform operation on any fxt::expected instance. The resulting expected object
         * will contain the transformed value if the original expected contained a value,
         * or the original error if it contained an error.
         */
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.transform(f);
            };
        }
    };

    inline constexpr TransformWrapper transform = {};
}    // namespace fxt