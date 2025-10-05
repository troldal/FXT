//
// Created by kenne on 30-09-2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <tuple>

// Helper function to print tuples
template<typename... Args>
void print_tuple(const std::tuple<Args...>& t) {
    std::cout << "(";
    std::apply([](const auto&... args) {
        std::size_t n = 0;
        ((std::cout << args << (++n != sizeof...(args) ? ", " : "")), ...);
    }, t);
    std::cout << ")";
}

int main() {
    std::cout << "=== fxt::append Demo ===\n\n";

    // Example 1: Appending a plain value to an expected tuple
    std::cout << "1. Appending plain values to expected<tuple>:\n";
    auto result1 = fxt::expected<std::tuple<int, double>, std::string>{std::tuple{42, 3.14}}
        | fxt::append(std::string{"hello"});

    if (result1) {
        std::cout << "   Result: ";
        print_tuple(*result1);
        std::cout << "\n\n";
    }

    // Example 2: Chaining multiple appends
    std::cout << "2. Chaining multiple appends:\n";
    auto result2 = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}}
        | fxt::append(2)
        | fxt::append(3.0)
        | fxt::append(std::string{"four"});

    if (result2) {
        std::cout << "   Result: ";
        print_tuple(*result2);
        std::cout << "\n\n";
    }

    // Example 3: Error propagation
    std::cout << "3. Error propagation:\n";
    auto result3 = fxt::expected<std::tuple<int, int>, std::string>{fxt::unexpected{"Error!"}}
        | fxt::append(100);

    if (!result3) {
        std::cout << "   Error: " << result3.error() << "\n\n";
    }

    // Example 4: Appending an expected value to a tuple
    std::cout << "4. Appending expected<int> to expected<tuple>:\n";
    auto value = fxt::expected<int, std::string>{999};
    auto result4 = fxt::expected<std::tuple<std::string>, std::string>{std::tuple{"start"}}
        | fxt::append(value);

    if (result4) {
        std::cout << "   Result: ";
        print_tuple(*result4);
        std::cout << "\n\n";
    }

    // Example 5: Error in appended expected
    std::cout << "5. Error in appended expected:\n";
    auto error_value = fxt::expected<int, std::string>{fxt::unexpected{"Parse failed"}};
    auto result5 = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}}
        | fxt::append(error_value);

    if (!result5) {
        std::cout << "   Error: " << result5.error() << "\n\n";
    }

    // Example 6: Optional with append
    std::cout << "6. Appending to optional<tuple>:\n";
    auto result6 = fxt::optional<std::tuple<int, int>>{std::tuple{10, 20}}
        | fxt::append(30)
        | fxt::append(std::string{"end"});

    if (result6) {
        std::cout << "   Result: ";
        print_tuple(*result6);
        std::cout << "\n\n";
    }

    // Example 7: Empty optional
    std::cout << "7. Empty optional:\n";
    auto result7 = fxt::optional<std::tuple<int>>{fxt::nullopt}
        | fxt::append(100);

    if (!result7) {
        std::cout << "   Result: nullopt\n\n";
    }

    // Example 8: Appending optional to optional
    std::cout << "8. Appending optional<int> to optional<tuple>:\n";
    auto opt_value = fxt::optional<int>{777};
    auto result8 = fxt::optional<std::tuple<std::string>>{std::tuple{"data"}}
        | fxt::append(opt_value);

    if (result8) {
        std::cout << "   Result: ";
        print_tuple(*result8);
        std::cout << "\n\n";
    }

    // Example 9: Appending empty optional
    std::cout << "9. Appending empty optional:\n";
    auto empty_opt = fxt::optional<int>{fxt::nullopt};
    auto result9 = fxt::optional<std::tuple<int>>{std::tuple{42}}
        | fxt::append(empty_opt);

    if (!result9) {
        std::cout << "   Result: nullopt\n\n";
    }

    // Example 10: Real-world scenario - building a result tuple
    std::cout << "10. Real-world: Building a configuration tuple:\n";
    auto config = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::append(std::string{"AppName"})
        | fxt::append(42)                    // version
        | fxt::append(true)                  // debug mode
        | fxt::append(3.14);                 // scaling factor

    if (config) {
        std::cout << "   Config: ";
        print_tuple(*config);
        std::cout << "\n";
    }

    return 0;
}
