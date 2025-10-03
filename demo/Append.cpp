//
// Created by kenne on 30-09-2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <tuple>

// Helper functions for demonstration
fxt::expected<std::tuple<int, int>, std::string> make_tuple_expected(int a, int b) {
    return fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(a, b)};
}

fxt::expected<std::tuple<int, int>, std::string> make_error_expected() {
    return fxt::unexpected<std::string>{"Initial error"};
}

fxt::optional<std::tuple<int, int>> make_tuple_optional(int a, int b) {
    return fxt::optional<std::tuple<int, int>>{std::make_tuple(a, b)};
}

fxt::optional<std::tuple<int, int>> make_empty_optional() {
    return fxt::nullopt;
}

fxt::expected<int, std::string> parse_int(const std::string& str) {
    try {
        int value = std::stoi(str);
        return fxt::expected<int, std::string>{value};
    } catch (...) {
        return fxt::unexpected<std::string>{"Parse error: " + str};
    }
}

fxt::optional<int> maybe_int(int value, bool should_have_value = true) {
    return should_have_value ? fxt::optional<int>{value} : fxt::nullopt;
}

// Helper to print tuple contents
template<typename T>
void print_value(const T& value) {
    if constexpr (requires { value.has_value(); }) {
        // Handle expected or optional types
        if (value.has_value()) {
            if constexpr (requires { value.error(); }) {
                // It's an expected type
                std::cout << *value;
            } else {
                // It's an optional type
                std::cout << *value;
            }
        } else {
            if constexpr (requires { value.error(); }) {
                // It's an expected type with error
                std::cout << "error(" << value.error() << ")";
            } else {
                // It's an empty optional
                std::cout << "nullopt";
            }
        }
    } else {
        // Regular value
        std::cout << value;
    }
}

template<typename... Args>
void print_tuple(const std::tuple<Args...>& t) {
    std::cout << "(";
    std::apply([](const auto&... args) {
        std::size_t n = 0;
        ((print_value(args), std::cout << (++n != sizeof...(args) ? ", " : "")), ...);
    }, t);
    std::cout << ")";
}

int main() {
    std::cout << "=== fxt::append Demonstration ===\n\n";

    // Demonstration with fxt::expected containing tuples
    std::cout << "--- fxt::expected examples ---\n";

    // Example 1: Successful append with direct value
    auto exp_result1 = make_tuple_expected(1, 2) | fxt::append(42);
    if (exp_result1.has_value()) {
        std::cout << "make_tuple_expected(1, 2) | append(42) = ";
        print_tuple(*exp_result1);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_expected(1, 2) | append(42) = error: " << exp_result1.error() << std::endl;
    }

    // Example 2: Failed append due to initial error
    auto exp_result2 = make_error_expected() | fxt::append(42);
    if (exp_result2.has_value()) {
        std::cout << "make_error_expected() | append(42) = ";
        print_tuple(*exp_result2);
        std::cout << std::endl;
    } else {
        std::cout << "make_error_expected() | append(42) = error: " << exp_result2.error() << std::endl;
    }

    // Example 3: Chaining multiple appends
    auto exp_result3 = make_tuple_expected(1, 2)
                     | fxt::append(42)
                     | fxt::append(std::string("hello"));
    if (exp_result3.has_value()) {
        std::cout << "Chain: (1,2) | append(42) | append(\"hello\") = ";
        print_tuple(*exp_result3);
        std::cout << std::endl;
    } else {
        std::cout << "Chain: (1,2) | append(42) | append(\"hello\") = error: " << exp_result3.error() << std::endl;
    }

    // Example 4: Appending from another expected (success)
    auto exp_result4 = make_tuple_expected(10, 20) | fxt::append(parse_int("99"));
    if (exp_result4.has_value()) {
        std::cout << "make_tuple_expected(10, 20) | append(parse_int(\"99\")) = ";
        print_tuple(*exp_result4);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_expected(10, 20) | append(parse_int(\"99\")) = error: " << exp_result4.error() << std::endl;
    }

    // Example 5: Appending from another expected (parse error)
    auto exp_result5 = make_tuple_expected(10, 20) | fxt::append(parse_int("invalid"));
    if (exp_result5.has_value()) {
        std::cout << "make_tuple_expected(10, 20) | append(parse_int(\"invalid\")) = ";
        print_tuple(*exp_result5);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_expected(10, 20) | append(parse_int(\"invalid\")) = error: " << exp_result5.error() << std::endl;
    }

    std::cout << "\n--- fxt::optional examples ---\n";

    // Example 6: Successful append with direct value to optional
    auto opt_result1 = make_tuple_optional(3, 4) | fxt::append(100);
    if (opt_result1.has_value()) {
        std::cout << "make_tuple_optional(3, 4) | append(100) = ";
        print_tuple(*opt_result1);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_optional(3, 4) | append(100) = nullopt" << std::endl;
    }

    // Example 7: Failed append due to empty optional
    auto opt_result2 = make_empty_optional() | fxt::append(100);
    if (opt_result2.has_value()) {
        std::cout << "make_empty_optional() | append(100) = ";
        print_tuple(*opt_result2);
        std::cout << std::endl;
    } else {
        std::cout << "make_empty_optional() | append(100) = nullopt" << std::endl;
    }

    // Example 8: Chaining appends with optional
    auto opt_result3 = make_tuple_optional(5, 6)
                     | fxt::append(200)
                     | fxt::append(std::string("world"));
    if (opt_result3.has_value()) {
        std::cout << "Chain: (5,6) | append(200) | append(\"world\") = ";
        print_tuple(*opt_result3);
        std::cout << std::endl;
    } else {
        std::cout << "Chain: (5,6) | append(200) | append(\"world\") = nullopt" << std::endl;
    }

    // Example 9: Appending from optional to optional (success)
    auto opt_result4 = make_tuple_optional(7, 8) | fxt::append(maybe_int(500, true));
    if (opt_result4.has_value()) {
        std::cout << "make_tuple_optional(7, 8) | append(maybe_int(500, true)) = ";
        print_tuple(*opt_result4);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_optional(7, 8) | append(maybe_int(500, true)) = nullopt" << std::endl;
    }

    // Example 10: Appending from optional to optional (empty optional)
    auto opt_result5 = make_tuple_optional(7, 8) | fxt::append(maybe_int(500, false));
    if (opt_result5.has_value()) {
        std::cout << "make_tuple_optional(7, 8) | append(maybe_int(500, false)) = ";
        print_tuple(*opt_result5);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_optional(7, 8) | append(maybe_int(500, false)) = nullopt" << std::endl;
    }

    std::cout << "\n--- Mixed type examples ---\n";

    // Example 11: Appending expected to optional (success)
    auto mixed_result1 = make_tuple_optional(11, 12) | fxt::append(parse_int("777"));
    if (mixed_result1.has_value()) {
        std::cout << "make_tuple_optional(11, 12) | append(parse_int(\"777\")) = ";
        print_tuple(*mixed_result1);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_optional(11, 12) | append(parse_int(\"777\")) = nullopt" << std::endl;
    }

    // Example 12: Appending expected to optional (parse error)
    auto mixed_result2 = make_tuple_optional(11, 12) | fxt::append(parse_int("bad"));
    if (mixed_result2.has_value()) {
        std::cout << "make_tuple_optional(11, 12) | append(parse_int(\"bad\")) = ";
        print_tuple(*mixed_result2);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_optional(11, 12) | append(parse_int(\"bad\")) = nullopt" << std::endl;
    }

    // Example 13: Appending optional to expected (success)
    auto mixed_result3 = make_tuple_expected(13, 14) | fxt::append(maybe_int(888, true));
    if (mixed_result3.has_value()) {
        std::cout << "make_tuple_expected(13, 14) | append(maybe_int(888, true)) = ";
        print_tuple(*mixed_result3);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_expected(13, 14) | append(maybe_int(888, true)) = error: " << mixed_result3.error() << std::endl;
    }

    // Example 14: Appending optional to expected (empty optional)
    auto mixed_result4 = make_tuple_expected(13, 14) | fxt::append(maybe_int(888, false));
    if (mixed_result4.has_value()) {
        std::cout << "make_tuple_expected(13, 14) | append(maybe_int(888, false)) = ";
        print_tuple(*mixed_result4);
        std::cout << std::endl;
    } else {
        std::cout << "make_tuple_expected(13, 14) | append(maybe_int(888, false)) = error: " << mixed_result4.error() << std::endl;
    }

    std::cout << "\n--- Complex chaining example ---\n";

    // Example 15: Complex real-world scenario
    auto complex_result = make_tuple_expected(1, 2)
                        | fxt::append(parse_int("42"))        // Add parsed integer
                        | fxt::append(std::string("test"))    // Add string
                        | fxt::append(maybe_int(999, true));  // Add optional value

    std::cout << "Complex chain: (1,2) | append(parse_int(\"42\")) | append(\"test\") | append(maybe_int(999))\n";
    if (complex_result.has_value()) {
        std::cout << "Result: ";
        print_tuple(*complex_result);
        std::cout << std::endl;
    } else {
        std::cout << "Result: error: " << complex_result.error() << std::endl;
    }

    return 0;
}
