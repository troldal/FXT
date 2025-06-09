//
// Created by kenne on 28/04/2025.
//

#pragma once

#include "Expected.hpp"
#include "Overload.hpp"
#include "impl/concepts/IsExpected.hpp"
#include "impl/utils/TupleAppend.hpp"

namespace fxt
{

    /**
     * @brief Wrapper for the value operation on expected-like objects
     *
     * ValueWrapper provides a way to extract the contained value from a fxt::expected object
     * in a functional style. It creates a higher-order function that accesses the
     * contained value using the value() method of the expected object.
     */
    struct ValueWrapper
    {
        /**
         * @brief Creates a function that extracts the value from an expected object
         *
         * @return A lambda that takes an expected object and returns its contained value
         *
         * The returned lambda handles the value extraction on any fxt::expected instance.
         *
         * @note This function will throw an exception if called on an expected object
         * that contains an error rather than a value.
         */
        auto operator()() const
        {
            return []<typename TValue, typename TError>(const fxt::expected<TValue, TError>& ex) { return ex.value(); };
        }
    };

    inline constexpr ValueWrapper value = {};
}    // namespace fxt