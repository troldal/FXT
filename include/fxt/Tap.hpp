//
// Created by kenne on 04/10/2025.
//

#pragma once

#include <utility>

namespace fxt
{
    /**
     * @brief Tap (or tee) operation for injecting side effects into a pipeline without changing the value
     *
     * This is a functional programming pattern that allows you to perform side effects (like logging,
     * debugging, or other operations) on a value in a pipeline without modifying the value itself.
     * The value is passed through unchanged after the side effect function is executed.
     *
     * The tap operation works with any type, including monadic types like expected and optional.
     * For monadic types, the side effect is only executed if the container holds a value.
     *
     * @tparam TFunction The type of the side effect function to execute
     * @param f The function to execute as a side effect (should accept the unwrapped value)
     * @return A callable that accepts any value and returns it unchanged after executing the side effect
     *
     * @section Usage
     * @code
     * // With regular values
     * auto result1 = 42 | fxt::tap([](int x) { std::cout << "Value: " << x << '\n'; });
     * // result1 == 42, and "Value: 42" is printed
     *
     * // With expected types
     * auto result2 = std::expected<int, Error>{42}
     *              | fxt::transform([](int x) { return x * 2; })
     *              | fxt::tap([](int x) { std::cout << "After transform: " << x << '\n'; })
     *              | fxt::transform([](int x) { return x + 10; });
     * // Logs "After transform: 84" and result2 contains 94
     *
     * // With optional types
     * auto result3 = std::optional<std::string>{"hello"}
     *              | fxt::tap([](const auto& s) { std::cout << "Got: " << s << '\n'; })
     *              | fxt::transform([](const auto& s) { return s + " world"; });
     * // Logs "Got: hello" and result3 contains "hello world"
     *
     * // With empty optional (side effect is not executed)
     * auto result4 = std::optional<int>{}
     *              | fxt::tap([](int x) { std::cout << "This won't print\n"; });
     * // Nothing is logged, result4 is empty
     * @endcode
     */
    inline constexpr auto tap = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
        {
            // Check if the container has a transform method (like expected/optional)
            if constexpr (requires { container.transform(f); })
            {
                // For monadic types, use transform but return the original container
                // We need to execute the side effect without changing the value
                if constexpr (requires { container.has_value(); })
                {
                    if (container.has_value())
                    {
                        // Execute side effect on the contained value
                        f(*container);
                    }
                }
                return std::forward<TContainer>(container);
            }
            else
            {
                // For non-monadic types, execute side effect and return the value
                f(container);
                return std::forward<TContainer>(container);
            }
        };
    };

}    // namespace fxt

