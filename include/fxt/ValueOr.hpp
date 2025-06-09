//
// Created by kenne on 24/02/2025.
//

#pragma once

#include "Expected.hpp"
#include "Overload.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"

namespace fxt
{

    /**
     * @brief Wrapper for the value_or operation on expected-like objects
     *
     * ValueOrWrapper provides a functional way to access the contained value of a fxt::expected object
     * or return an alternative value if the expected contains an error. It creates a higher-order function
     * that applies the value_or pattern, simplifying error handling in functional chains.
     */
    struct ValueOrWrapper
    {
        /**
         * @brief Creates a function that applies value_or with the provided alternative value
         *
         * @tparam TAlternative Type of the alternative value
         * @param val The alternative value to return if the expected contains an error
         * @return A lambda that takes an expected object and returns either its value or the alternative
         *
         * The returned lambda preserves perfect forwarding of the alternative value and handles
         * the value_or operation on any fxt::expected instance. If the expected contains a value,
         * that value is returned; otherwise, the provided alternative value is returned.
         */
        template<typename TAlternative>
        auto operator()(TAlternative&& val) const
        {
            return [val = std::forward<TAlternative>(val)]<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) {
                return ex.value_or(val);
            };
        }
    };

    inline constexpr ValueOrWrapper value_or = {};
}    // namespace fxt