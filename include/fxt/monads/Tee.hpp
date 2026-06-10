/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/


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
    // TODO: CONSISTENCY — this file is a verbatim duplicate of Tap.hpp (including the doc
    //       comments, which still say "tap"/"tap_error"/"tap_none") except that the return
    //       statements are active here. Keep one implementation under one name and remove
    //       the other file; having both fxt::tap (broken) and fxt::tee (working) for the
    //       same operation is confusing.
    // TODO: COMPLETENESS — tee passes the container through but only invokes the side effect
    //       via `container.value()` on an lvalue; consider std::invoke(f, ...) for member
    //       pointers, and a plain-value overload (docs show `42 | fxt::tap(...)`, which the
    //       requires-clause `container.has_value()` rejects).
    inline constexpr auto tee = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires {container.has_value();container.value();}
        {
            if (container.has_value()) {
                // Execute side effect on the contained value
                f(container.value());
            }

            // Always return the original container unchanged
            return std::forward<TContainer>(container);
        };
    };

    /**
     * @brief Tap error operation for injecting side effects on error values in expected-like types
     *
     * This is the complementary operation to tap, specifically for handling error cases in expected types.
     * The side effect is only executed if the container holds an error value.
     * The container is passed through unchanged after the side effect function is executed.
     *
     * @tparam TFunction The type of the side effect function to execute
     * @param f The function to execute as a side effect (should accept the error value)
     * @return A callable that accepts an expected-like value and returns it unchanged after executing the side effect
     *
     * @section Usage
     * @code
     * // With expected types - success case (side effect not executed)
     * auto result1 = fxt::expected<int, std::string>{42}
     *              | fxt::tap_error([](const std::string& err) {
     *                  std::cout << "Error: " << err << '\n';
     *              });
     * // Nothing is logged, result1 contains 42
     *
     * // With expected types - error case
     * auto result2 = fxt::expected<int, std::string>{fxt::unexpected("failed")}
     *              | fxt::tap_error([](const std::string& err) {
     *                  std::cout << "Error: " << err << '\n';
     *              })
     *              | fxt::or_else([](const std::string&) {
     *                  return fxt::expected<int, std::string>{0};
     *              });
     * // Logs "Error: failed" and result2 contains 0
     *
     * // Combined with tap for complete logging
     * auto result3 = some_operation()
     *              | fxt::tap([](int x) { std::cout << "Success: " << x << '\n'; })
     *              | fxt::tap_error([](const Error& e) { std::cout << "Error: " << e << '\n'; });
     * // Logs either success or error, never both
     * @endcode
     */
    inline constexpr auto tee_error = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires { container.has_value(); container.error(); }
        {
            if (!container.has_value()) {
                // Execute side effect on the error value
                f(container.error());
            }

            // Always return the original container unchanged
            return std::forward<TContainer>(container);
        };
    };

    /**
     * @brief Tap none operation for injecting side effects on empty optional-like types
     *
     * This is the complementary operation to tap, specifically for handling empty cases in optional types.
     * The side effect is only executed if the container is empty (has no value).
     * The container is passed through unchanged after the side effect function is executed.
     *
     * Note: Since there's no value to pass when the optional is empty, the function takes no parameters.
     *
     * @tparam TFunction The type of the side effect function to execute (should take no parameters)
     * @param f The function to execute as a side effect (takes no parameters)
     * @return A callable that accepts an optional-like value and returns it unchanged after executing the side effect
     *
     * @section Usage
     * @code
     * // With optional types - has value (side effect not executed)
     * auto result1 = fxt::optional<int>{42}
     *              | fxt::tap_none([] { std::cout << "Empty!\n"; });
     * // Nothing is logged, result1 contains 42
     *
     * // With optional types - empty case
     * auto result2 = fxt::optional<int>{fxt::nullopt}
     *              | fxt::tap_none([] { std::cout << "No value found\n"; })
     *              | fxt::or_else([]() { return fxt::optional<int>{0}; });
     * // Logs "No value found" and result2 contains 0
     *
     * // Combined with tap for complete logging
     * auto result3 = find_user(id)
     *              | fxt::tap([](const User& u) {
     *                  std::cout << "Found: " << u.name << '\n';
     *              })
     *              | fxt::tap_none([] {
     *                  std::cout << "User not found\n";
     *              });
     * // Logs either the user name or "User not found", never both
     *
     * // Counting empty optionals
     * int empty_count = 0;
     * auto result4 = some_optional()
     *              | fxt::tap_none([&empty_count] { empty_count++; });
     * @endcode
     */
    inline constexpr auto tee_none = []<typename TFunction>(TFunction&& f) {
        return [f = std::forward<TFunction>(f)]<typename TContainer>(TContainer&& container)
            requires requires { container.has_value(); }
        {
            if (!container.has_value()) {
                // Execute side effect (no value to pass)
                f();
            }

            // Always return the original container unchanged
            return std::forward<TContainer>(container);
        };
    };

}    // namespace fxt
