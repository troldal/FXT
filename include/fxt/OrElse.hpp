//
// Created by kenne on 28/02/2025.
//

#pragma once

namespace fxt
{

    /**
     * @brief Wrapper for the or_else operation on expected-like objects
     *
     * OrElseWrapper provides a way to chain error handling operations on fxt::expected objects
     * in a functional style. It creates a higher-order function that applies the
     * provided function to the error inside an expected object using or_else,
     * but only if the expected object contains an error.
     */
    struct OrElseWrapper
    {
        /**
         * @brief Creates a function that applies or_else with the provided function
         *
         * @tparam TFunction Type of the function to apply via or_else
         * @param f The function to apply to the expected's error
         * @return A lambda that takes an expected object and applies the function via or_else
         *
         * The returned lambda preserves perfect forwarding of the function and handles
         * the or_else operation on any fxt::expected instance. The function f should
         * return an expected-like type with the same value type for proper error handling
         * composition.
         */
        template<typename TFunction>
        auto operator()(TFunction&& f) const
        {
            return [f = std::forward<TFunction>(f)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.or_else(f);
            };
        }
    };

    inline constexpr OrElseWrapper or_else = {};

}    // namespace fxt