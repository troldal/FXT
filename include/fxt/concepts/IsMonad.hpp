//
// Created by kenne on 11-10-2025.
//

#pragma once

#include "IsOptional.hpp"
#include "IsExpected.hpp"

namespace fxt
{
    /**
     * @brief Concept to check if a type behaves like a monad
     *
     * A monad-like type is either an optional-like type or an expected-like type.
     * This concept can be used to constrain template parameters to only accept
     * monadic types that support common operations like transform, and_then, etc.
     *
     * @tparam T The type to check
     *
     * @section Usage
     * @code
     * template<fxt::monad_like M>
     * void process_monad(M&& monad) {
     *     // Works with both fxt::optional and fxt::expected
     * }
     *
     * auto opt = fxt::optional<int>{42};
     * auto exp = fxt::expected<int, std::string>{42};
     * process_monad(opt); // OK
     * process_monad(exp); // OK
     * process_monad(42);  // Error: 42 is not monad_like
     * @endcode
     */
    template<typename T>
    concept monad_like = optional_like<T> || expected_like<T>;

} // namespace fxt

