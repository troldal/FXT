//
// Created by kenne on 16/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>
#include <stdexcept>
#include <vector>

using namespace std::string_literals;

// Helper functions for testing
int divide(int a, int b) {
    if (b == 0) throw std::runtime_error("Division by zero");
    return a / b;
}

void validate_positive(int x) {
    if (x <= 0) throw std::invalid_argument("Value must be positive");
}

std::string parse_int(const std::string& str) {
    if (str.empty()) throw std::invalid_argument("Empty string");
    int val = std::stoi(str);  // May throw std::invalid_argument or std::out_of_range
    return "Parsed: " + std::to_string(val);
}

class CustomException : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override { return "Custom error"; }
};

void throw_custom() {
    throw CustomException();
}

TEST_CASE("fxt::attempt with non-void returning functions", "[attempt][expected]")
{
    SECTION("Success case returns expected value")
    {
        auto result = fxt::attempt(divide, 10, 2);

        REQUIRE(result.has_value());
        REQUIRE(*result == 5);
    }

    SECTION("Exception is caught and returned as failure")
    {
        auto result = fxt::attempt(divide, 10, 0);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Division by zero") != std::string::npos);
    }

    SECTION("Multiple successful calls are independent")
    {
        auto result1 = fxt::attempt(divide, 20, 4);
        auto result2 = fxt::attempt(divide, 15, 3);

        REQUIRE(result1.has_value());
        REQUIRE(result2.has_value());
        REQUIRE(*result1 == 5);
        REQUIRE(*result2 == 5);
    }

    SECTION("Result type matches function return type")
    {
        auto result = fxt::attempt([](int x) { return x * 2; }, 21);

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
        static_assert(std::is_same_v<decltype(*result), int&>);
    }
}

TEST_CASE("fxt::attempt with void returning functions", "[attempt][expected]")
{
    SECTION("Success case returns unit")
    {
        auto result = fxt::attempt(validate_positive, 5);

        REQUIRE(result.has_value());
        // The value is fxt::unit, which is a placeholder
        static_assert(std::is_same_v<decltype(*result), fxt::unit&>);
    }

    SECTION("Exception is caught and returned as failure")
    {
        auto result = fxt::attempt(validate_positive, -1);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Value must be positive") != std::string::npos);
    }

    SECTION("Void lambda success")
    {
        int side_effect = 0;
        auto result = fxt::attempt([&side_effect]() { side_effect = 42; });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 42);
    }

    SECTION("Void lambda with exception")
    {
        auto result = fxt::attempt([]() { throw std::logic_error("Test error"); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Test error") != std::string::npos);
    }
}

TEST_CASE("fxt::attempt with lambda expressions", "[attempt][expected]")
{
    SECTION("Lambda with capture - success")
    {
        int multiplier = 3;
        auto result = fxt::attempt([multiplier](int x) { return x * multiplier; }, 14);

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Lambda with capture - failure")
    {
        int threshold = 10;
        auto result = fxt::attempt([threshold](int x) -> int {
            if (x > threshold) throw std::out_of_range("Too large");
            return x;
        }, 100);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Too large") != std::string::npos);
    }

    SECTION("Immediately invoked lambda")
    {
        auto result = fxt::attempt([]() { return 42; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }
}

TEST_CASE("fxt::attempt with different exception types", "[attempt][expected]")
{
    SECTION("std::runtime_error")
    {
        auto result = fxt::attempt([]() -> int { throw std::runtime_error("Runtime error"); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Runtime error") != std::string::npos);
    }

    SECTION("std::invalid_argument")
    {
        auto result = fxt::attempt(parse_int, "");

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Empty string") != std::string::npos);
    }

    SECTION("std::out_of_range")
    {
        auto result = fxt::attempt(parse_int, "99999999999999999999");

        REQUIRE_FALSE(result.has_value());
        // The exact message depends on the implementation
        REQUIRE_FALSE(result.error().message().empty());
    }

    SECTION("Custom exception type")
    {
        auto result = fxt::attempt(throw_custom);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Custom error") != std::string::npos);
    }

    SECTION("Non-std::exception throw (int)")
    {
        auto result = fxt::attempt([]() -> int { throw 42; });

        REQUIRE_FALSE(result.has_value());
        // Unknown exception will have a generic message
        REQUIRE_FALSE(result.error().message().empty());
    }

    SECTION("Non-std::exception throw (string)")
    {
        auto result = fxt::attempt([]() -> int { throw std::string("string error"); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().message().empty());
    }
}

TEST_CASE("fxt::attempt with perfect forwarding", "[attempt][expected]")
{
    SECTION("Forwards rvalue references correctly")
    {
        std::string str = "test";
        auto result = fxt::attempt([](std::string&& s) { return s + " moved"; }, std::move(str));

        REQUIRE(result.has_value());
        REQUIRE(*result == "test moved");
    }

    SECTION("Forwards lvalue references correctly")
    {
        int value = 42;
        auto result = fxt::attempt([](int& x) { x *= 2; return x; }, value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
        REQUIRE(value == 84);
    }

    SECTION("Forwards const references correctly")
    {
        const int value = 42;
        auto result = fxt::attempt([](const int& x) { return x * 2; }, value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }
}

TEST_CASE("fxt::attempt composition with monadic operations", "[attempt][expected][composition]")
{
    SECTION("Chain with and_then - all succeed")
    {
        auto result = fxt::attempt(divide, 100, 5)
            .and_then([](int x) { return fxt::attempt(divide, x, 2); })
            .and_then([](int x) { return fxt::attempt(divide, x, 5); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 2);  // (100 / 5) / 2 / 5 = 2
    }

    SECTION("Chain with and_then - first fails")
    {
        auto result = fxt::attempt(divide, 100, 0)
            .and_then([](int x) { return fxt::attempt(divide, x, 2); });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain with and_then - later operation fails")
    {
        auto result = fxt::attempt(divide, 100, 5)
            .and_then([](int x) { return fxt::attempt(divide, x, 0); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Division by zero") != std::string::npos);
    }

    SECTION("Chain with transform")
    {
        auto result = fxt::attempt(divide, 10, 2)
            .transform([](int x) { return x * x; })
            .transform([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "25");
    }

    SECTION("Chain with or_else - recovery from error")
    {
        auto result = fxt::attempt(divide, 10, 0)
            .or_else([](const fxt::failure&) { return fxt::expected<int, fxt::failure>(42); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Chain with or_else - no error to recover")
    {
        auto result = fxt::attempt(divide, 10, 2)
            .or_else([](const fxt::failure&) { return fxt::expected<int, fxt::failure>(999); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 5);  // Original value, or_else not invoked
    }

    SECTION("Complex chain with multiple operations")
    {
        auto result = fxt::attempt(divide, 100, 4)
            .transform([](int x) { return x * 2; })
            .and_then([](int x) { return fxt::attempt(divide, x, 5); })
            .transform([](int x) { return x + 10; })
            .or_else([](const fxt::failure&) { return fxt::expected<int, fxt::failure>(0); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);  // ((100/4)*2)/5 + 10 = 20
    }
}

TEST_CASE("fxt::attempt with value extraction", "[attempt][expected]")
{
    SECTION("value_or with success")
    {
        int result = fxt::attempt(divide, 10, 2).value_or(999);

        REQUIRE(result == 5);
    }

    SECTION("value_or with failure")
    {
        int result = fxt::attempt(divide, 10, 0).value_or(999);

        REQUIRE(result == 999);
    }

    SECTION("Dereference operator with success")
    {
        auto result = fxt::attempt(divide, 20, 4);

        REQUIRE(*result == 5);
    }

    SECTION("has_value predicate")
    {
        auto success = fxt::attempt(divide, 10, 2);
        auto failure = fxt::attempt(divide, 10, 0);

        REQUIRE(success.has_value());
        REQUIRE_FALSE(failure.has_value());
    }

    SECTION("Boolean conversion")
    {
        auto success = fxt::attempt(divide, 10, 2);
        auto failure = fxt::attempt(divide, 10, 0);

        REQUIRE(success);
        REQUIRE_FALSE(failure);
    }
}

TEST_CASE("fxt::attempt error information", "[attempt][expected][failure]")
{
    SECTION("Error message is preserved")
    {
        auto result = fxt::attempt([]() -> int {
            throw std::runtime_error("Specific error message");
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message() == "Specific error message");
    }

    SECTION("Error can be inspected")
    {
        auto result = fxt::attempt(divide, 10, 0);

        if (!result) {
            auto& error = result.error();
            REQUIRE_FALSE(error.message().empty());
            REQUIRE(error.message().find("Division by zero") != std::string::npos);
        }
    }

    SECTION("Multiple failures maintain distinct error messages")
    {
        auto error1 = fxt::attempt([]() -> int { throw std::runtime_error("Error 1"); });
        auto error2 = fxt::attempt([]() -> int { throw std::runtime_error("Error 2"); });

        REQUIRE(error1.error().message() == "Error 1");
        REQUIRE(error2.error().message() == "Error 2");
    }
}

TEST_CASE("fxt::attempt is noexcept", "[attempt][expected][noexcept]")
{
    SECTION("attempt itself is noexcept")
    {
        static_assert(noexcept(fxt::attempt(divide, 10, 2)));
        static_assert(noexcept(fxt::attempt(validate_positive, 5)));
        static_assert(noexcept(fxt::attempt([]() { throw std::exception(); })));
    }

    SECTION("No exception escapes even with throwing function")
    {
        // This should not throw, even though the lambda throws
        REQUIRE_NOTHROW(fxt::attempt([]() -> int { throw std::runtime_error("test"); }));
    }
}

// ---------------------------------------------------------------------------
// Pipe-adaptor overload: attempt(fn) — returns a closure for use with |
// ---------------------------------------------------------------------------

TEST_CASE("fxt::attempt pipe-adaptor: success path", "[attempt][adaptor]")
{
    SECTION("fn is called with the piped value")
    {
        auto result = fxt::attempt(divide, 20, 4)
                    | fxt::attempt([](int x) { return x * 3; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 15);
    }

    SECTION("void-returning fn produces fxt::unit")
    {
        bool called = false;
        auto result = fxt::attempt(divide, 10, 2)
                    | fxt::attempt([&called](int) { called = true; });

        REQUIRE(result.has_value());
        REQUIRE(called);
        static_assert(std::is_same_v<decltype(*result), fxt::unit&>);
    }

    SECTION("chained attempt adaptors all succeed")
    {
        auto result = fxt::attempt(divide, 100, 5)
                    | fxt::attempt([](int x) { return divide(x, 2); })
                    | fxt::attempt([](int x) { return divide(x, 5); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 2);    // (100/5)/2/5
    }

    SECTION("stored adaptor can be applied multiple times")
    {
        auto double_it = fxt::attempt([](int x) { return x * 2; });

        auto r1 = fxt::attempt(divide, 10, 2) | double_it;
        auto r2 = fxt::attempt(divide, 6,  3) | double_it;

        REQUIRE(r1.has_value());
        REQUIRE(*r1 == 10);
        REQUIRE(r2.has_value());
        REQUIRE(*r2 == 4);
    }
}

TEST_CASE("fxt::attempt pipe-adaptor: error propagation", "[attempt][adaptor]")
{
    SECTION("existing failure propagates without calling fn")
    {
        bool called = false;
        auto result = fxt::attempt(divide, 10, 0)
                    | fxt::attempt([&called](int x) { called = true; return x; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("Division by zero") != std::string::npos);
        REQUIRE_FALSE(called);
    }

    SECTION("failure from first adaptor propagates through subsequent adaptors")
    {
        bool second_called = false;
        auto result = fxt::attempt(divide, 100, 5)
                    | fxt::attempt([](int x) { return divide(x, 0); })    // throws
                    | fxt::attempt([&second_called](int x) {
                          second_called = true;
                          return x;
                      });

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(second_called);
    }
}

TEST_CASE("fxt::attempt pipe-adaptor: exception handling", "[attempt][adaptor]")
{
    SECTION("exception thrown by fn is caught and returned as failure")
    {
        auto result = fxt::attempt(divide, 10, 2)
                    | fxt::attempt([](int) -> int { throw std::runtime_error("adaptor error"); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message().find("adaptor error") != std::string::npos);
    }

    SECTION("non-std::exception is caught")
    {
        auto result = fxt::attempt(divide, 10, 2)
                    | fxt::attempt([](int) -> int { throw 99; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(result.error().message().empty());
    }

    SECTION("exception in adaptor does not propagate")
    {
        REQUIRE_NOTHROW(
            fxt::attempt(divide, 10, 2)
                | fxt::attempt([](int) -> int { throw std::runtime_error("boom"); })
        );
    }
}

TEST_CASE("fxt::attempt pipe-adaptor: interop with other adaptors", "[attempt][adaptor]")
{
    SECTION("combined with transform")
    {
        auto result = fxt::attempt(divide, 20, 4)
                    | fxt::attempt([](int x) { return x * x; })
                    | fxt::transform([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "25");
    }

    SECTION("combined with or_else for recovery")
    {
        auto result = fxt::attempt(divide, 10, 2)
                    | fxt::attempt([](int) -> int { throw std::runtime_error("oops"); })
                    | fxt::or_else([](const fxt::failure&) {
                          return fxt::expected<int, fxt::failure>{ 0 };
                      });

        REQUIRE(result.has_value());
        REQUIRE(*result == 0);
    }
}

TEST_CASE("fxt::attempt with complex return types", "[attempt][expected]")
{
    SECTION("Returns std::string")
    {
        auto result = fxt::attempt(parse_int, "42");

        REQUIRE(result.has_value());
        REQUIRE(*result == "Parsed: 42");
    }

    SECTION("Returns std::vector")
    {
        auto result = fxt::attempt([]() {
            return std::vector<int>{1, 2, 3, 4, 5};
        });

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 5);
        REQUIRE((*result)[2] == 3);
    }

    SECTION("Returns custom struct")
    {
        struct Point { int x, y; };

        auto result = fxt::attempt([](int a, int b) {
            return Point{a, b};
        }, 10, 20);

        REQUIRE(result.has_value());
        REQUIRE(result->x == 10);
        REQUIRE(result->y == 20);
    }

    SECTION("Returns std::pair")
    {
        auto result = fxt::attempt([]() {
            return std::make_pair(42, "answer");
        });

        REQUIRE(result.has_value());
        REQUIRE(result->first == 42);
        REQUIRE(result->second == "answer"s);
    }
}

TEST_CASE("fxt::attempt real-world scenarios", "[attempt][expected]")
{
    SECTION("File parsing simulation")
    {
        auto parse_line = [](const std::string& line) {
            if (line.empty()) throw std::invalid_argument("Empty line");
            if (line[0] == '#') throw std::runtime_error("Comment line");
            return std::stoi(line);
        };

        std::vector<std::string> lines = {"42", "", "123", "#comment", "456"};
        std::vector<int> parsed_values;

        for (const auto& line : lines) {
            auto result = fxt::attempt(parse_line, line);
            if (result) {
                parsed_values.push_back(*result);
            }
        }

        REQUIRE(parsed_values.size() == 3);
        REQUIRE(parsed_values[0] == 42);
        REQUIRE(parsed_values[1] == 123);
        REQUIRE(parsed_values[2] == 456);
    }

    SECTION("Calculation pipeline with error handling")
    {
        auto safe_sqrt = [](double x) -> double {
            if (x < 0) throw std::domain_error("Negative square root");
            return std::sqrt(x);
        };

        auto result = fxt::attempt(divide, 100, 4)
            .transform([](int x) { return static_cast<double>(x); })
            .and_then([&safe_sqrt](double x) { return fxt::attempt(safe_sqrt, x); })
            .transform([](double x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == Catch::Approx(10.0));  // sqrt(100/4) * 2 = sqrt(25) * 2 = 10
    }

    SECTION("Resource validation chain")
    {
        auto validate_non_empty = [](const std::string& s) {
            if (s.empty()) throw std::invalid_argument("Empty string");
            return s;
        };

        auto validate_length = [](const std::string& s) {
            if (s.length() > 10) throw std::length_error("String too long");
            return s;
        };

        auto to_upper = [](const std::string& s) {
            std::string result = s;
            for (char& c : result) c = std::toupper(c);
            return result;
        };

        auto process = [&](const std::string& input) {
            return fxt::attempt(validate_non_empty, input)
                .and_then([&](const std::string& s) { return fxt::attempt(validate_length, s); })
                .transform(to_upper);
        };

        auto success = process("hello");
        REQUIRE(success.has_value());
        REQUIRE(*success == "HELLO");

        auto failure1 = process("");
        REQUIRE_FALSE(failure1.has_value());

        auto failure2 = process("verylongstring");
        REQUIRE_FALSE(failure2.has_value());
    }
}

