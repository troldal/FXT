//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <memory>

TEST_CASE("match with expected - success cases", "[match][expected]") {
    SECTION("match on successful expected with int") {
        auto exp = fxt::expected<int, std::string>{42};
        auto result = exp | fxt::match(
            [](int value) { return value * 2; },
            [](const std::string&) { return -1; }
        );
        REQUIRE(result == 84);
    }

    SECTION("match on successful expected with string") {
        auto exp = fxt::expected<std::string, int>{"hello"};
        auto result = exp | fxt::match(
            [](const std::string& s) { return s + " world"; },
            [](int) { return std::string{"error"}; }
        );
        REQUIRE(result == "hello world");
    }

    SECTION("match returning different type than stored") {
        auto exp = fxt::expected<int, std::string>{100};
        auto result = exp | fxt::match(
            [](int value) { return std::to_string(value); },
            [](const std::string& err) { return "Error: " + err; }
        );
        REQUIRE(result == "100");
    }
}

TEST_CASE("match with expected - error cases", "[match][expected]") {
    SECTION("match on failed expected with string error") {
        auto exp = fxt::expected<int, std::string>{fxt::unexpected{"error occurred"}};
        auto result = exp | fxt::match(
            [](int value) { return value * 2; },
            [](const std::string& err) { return -1; }
        );
        REQUIRE(result == -1);
    }

    SECTION("match on failed expected processing error message") {
        auto exp = fxt::expected<int, std::string>{fxt::unexpected{"failure"}};
        auto result = exp | fxt::match(
            [](int) { return std::string{"success"}; },
            [](const std::string& err) { return "Error: " + err; }
        );
        REQUIRE(result == "Error: failure");
    }

    SECTION("match with int error type") {
        auto exp = fxt::expected<std::string, int>{fxt::unexpected{404}};
        auto result = exp | fxt::match(
            [](const std::string&) { return 0; },
            [](int code) { return code; }
        );
        REQUIRE(result == 404);
    }
}

TEST_CASE("match with expected - both branches return same type", "[match][expected]") {
    SECTION("match returning bool") {
        auto exp1 = fxt::expected<int, std::string>{42};
        auto is_success1 = exp1 | fxt::match(
            [](int) { return true; },
            [](const std::string&) { return false; }
        );
        REQUIRE(is_success1 == true);

        auto exp2 = fxt::expected<int, std::string>{fxt::unexpected{"error"}};
        auto is_success2 = exp2 | fxt::match(
            [](int) { return true; },
            [](const std::string&) { return false; }
        );
        REQUIRE(is_success2 == false);
    }

    SECTION("match returning string from both branches") {
        auto exp = fxt::expected<int, std::string>{25};
        auto msg = exp | fxt::match(
            [](int v) { return "Value: " + std::to_string(v); },
            [](const std::string& e) { return "Error: " + e; }
        );
        REQUIRE(msg == "Value: 25");
    }
}

TEST_CASE("match with optional - value present", "[match][optional]") {
    SECTION("match on optional with value") {
        auto opt = fxt::optional<int>{42};
        auto result = opt | fxt::match(
            [](int value) { return value * 2; },
            []() { return -1; }
        );
        REQUIRE(result == 84);
    }

    SECTION("match on optional with string") {
        auto opt = fxt::optional<std::string>{"hello"};
        auto result = opt | fxt::match(
            [](const std::string& s) { return s + " world"; },
            []() { return std::string{"empty"}; }
        );
        REQUIRE(result == "hello world");
    }

    SECTION("match transforming type") {
        auto opt = fxt::optional<int>{100};
        auto result = opt | fxt::match(
            [](int value) { return std::to_string(value); },
            []() { return std::string{"none"}; }
        );
        REQUIRE(result == "100");
    }
}

TEST_CASE("match with optional - empty cases", "[match][optional]") {
    SECTION("match on empty optional") {
        auto opt = fxt::optional<int>{fxt::nullopt};
        auto result = opt | fxt::match(
            [](int value) { return value * 2; },
            []() { return -1; }
        );
        REQUIRE(result == -1);
    }

    SECTION("match on empty optional with string default") {
        auto opt = fxt::optional<std::string>{fxt::nullopt};
        auto result = opt | fxt::match(
            [](const std::string& s) { return s; },
            []() { return std::string{"default"}; }
        );
        REQUIRE(result == "default");
    }

    SECTION("match on empty optional returning bool") {
        auto opt = fxt::optional<int>{fxt::nullopt};
        auto has_value = opt | fxt::match(
            [](int) { return true; },
            []() { return false; }
        );
        REQUIRE(has_value == false);
    }
}

TEST_CASE("match with optional - both branches", "[match][optional]") {
    SECTION("match returning bool for presence check") {
        auto opt1 = fxt::optional<int>{42};
        auto present1 = opt1 | fxt::match(
            [](int) { return true; },
            []() { return false; }
        );
        REQUIRE(present1 == true);

        auto opt2 = fxt::optional<int>{fxt::nullopt};
        auto present2 = opt2 | fxt::match(
            [](int) { return true; },
            []() { return false; }
        );
        REQUIRE(present2 == false);
    }

    SECTION("match providing default value") {
        auto get_value = [](fxt::optional<int> opt) {
            return opt | fxt::match(
                [](int v) { return v; },
                []() { return 0; }
            );
        };

        REQUIRE(get_value(fxt::optional<int>{42}) == 42);
        REQUIRE(get_value(fxt::optional<int>{fxt::nullopt}) == 0);
    }
}

TEST_CASE("match with side effects", "[match][expected][optional]") {
    SECTION("match on expected with side effects") {
        int counter = 0;
        auto exp = fxt::expected<int, std::string>{42};
        exp | fxt::match(
            [&counter](int) { counter++; return 0; },
            [&counter](const std::string&) { counter--; return 0; }
        );
        REQUIRE(counter == 1);

        auto exp2 = fxt::expected<int, std::string>{fxt::unexpected{"error"}};
        exp2 | fxt::match(
            [&counter](int) { counter++; return 0; },
            [&counter](const std::string&) { counter--; return 0; }
        );
        REQUIRE(counter == 0);
    }

    SECTION("match on optional with side effects") {
        int counter = 0;
        auto opt = fxt::optional<int>{42};
        opt | fxt::match(
            [&counter](int) { counter++; return 0; },
            [&counter]() { counter--; return 0; }
        );
        REQUIRE(counter == 1);

        auto opt2 = fxt::optional<int>{fxt::nullopt};
        opt2 | fxt::match(
            [&counter](int) { counter++; return 0; },
            [&counter]() { counter--; return 0; }
        );
        REQUIRE(counter == 0);
    }
}

TEST_CASE("match in pipelines", "[match][expected][optional]") {
    SECTION("match after transform on expected") {
        auto result = fxt::expected<int, std::string>{10}
            | fxt::transform([](int x) { return x * 2; })
            | fxt::match(
                [](int v) { return "Result: " + std::to_string(v); },
                [](const std::string& e) { return "Error: " + e; }
            );
        REQUIRE(result == "Result: 20");
    }

    SECTION("match after and_then on expected") {
        auto divide = [](int x) -> fxt::expected<int, std::string> {
            if (x == 0) return fxt::unexpected<std::string>{"division by zero"};
            return 100 / x;
        };

        auto result1 = fxt::expected<int, std::string>{10}
            | fxt::and_then(divide)
            | fxt::match(
                [](int v) { return v; },
                [](const std::string&) { return -1; }
            );
        REQUIRE(result1 == 10);

        auto result2 = fxt::expected<int, std::string>{0}
            | fxt::and_then(divide)
            | fxt::match(
                [](int v) { return v; },
                [](const std::string&) { return -1; }
            );
        REQUIRE(result2 == -1);
    }

    SECTION("match after transform on optional") {
        auto result = fxt::optional<int>{5}
            | fxt::transform([](int x) { return x * x; })
            | fxt::match(
                [](int v) { return "Square: " + std::to_string(v); },
                []() { return std::string{"No value"}; }
            );
        REQUIRE(result == "Square: 25");
    }

    SECTION("match after and_then on optional") {
        auto validate = [](int x) -> fxt::optional<int> {
            if (x > 10) return x;
            return fxt::nullopt;
        };

        auto result1 = fxt::optional<int>{15}
            | fxt::and_then(validate)
            | fxt::match(
                [](int v) { return v; },
                []() { return 0; }
            );
        REQUIRE(result1 == 15);

        auto result2 = fxt::optional<int>{5}
            | fxt::and_then(validate)
            | fxt::match(
                [](int v) { return v; },
                []() { return 0; }
            );
        REQUIRE(result2 == 0);
    }
}

TEST_CASE("match without pipe operator", "[match][expected][optional]") {
    SECTION("direct match call on expected") {
        auto exp = fxt::expected<int, std::string>{42};
        auto result = fxt::match(
            [](int v) { return v + 10; },
            [](const std::string&) { return 0; }
        )(exp);
        REQUIRE(result == 52);
    }

    SECTION("direct match call on optional") {
        auto opt = fxt::optional<int>{42};
        auto result = fxt::match(
            [](int v) { return v + 10; },
            []() { return 0; }
        )(opt);
        REQUIRE(result == 52);
    }
}

TEST_CASE("match with move-only types", "[match][expected][optional]") {
    SECTION("match on expected with unique_ptr - direct call") {
        auto exp = fxt::expected<std::unique_ptr<int>, std::string>{
            std::make_unique<int>(42)
        };
        auto result = fxt::match(
            [](std::unique_ptr<int> ptr) { return *ptr; },
            [](const std::string&) { return -1; }
        )(std::move(exp));
        REQUIRE(result == 42);
    }

    SECTION("match on optional with unique_ptr - direct call") {
        auto opt = fxt::optional<std::unique_ptr<int>>{
            std::make_unique<int>(100)
        };
        auto result = fxt::match(
            [](std::unique_ptr<int> ptr) { return *ptr; },
            []() { return -1; }
        )(std::move(opt));
        REQUIRE(result == 100);
    }

    SECTION("match on empty optional with unique_ptr - direct call") {
        auto opt = fxt::optional<std::unique_ptr<int>>{fxt::nullopt};
        auto result = fxt::match(
            [](std::unique_ptr<int> ptr) { return *ptr; },
            []() { return -1; }
        )(std::move(opt));
        REQUIRE(result == -1);
    }
}

TEST_CASE("match returning void", "[match][expected][optional]") {
    SECTION("match on expected with void return") {
        int result = 0;
        auto exp = fxt::expected<int, std::string>{42};
        exp | fxt::match(
            [&result](int v) { result = v; },
            [&result](const std::string&) { result = -1; }
        );
        REQUIRE(result == 42);
    }

    SECTION("match on optional with void return") {
        int result = 0;
        auto opt = fxt::optional<int>{42};
        opt | fxt::match(
            [&result](int v) { result = v; },
            [&result]() { result = -1; }
        );
        REQUIRE(result == 42);
    }
}

TEST_CASE("match with complex types", "[match][expected][optional]") {
    SECTION("match with nested expected") {
        auto exp = fxt::expected<fxt::expected<int, std::string>, int>{
            fxt::expected<int, std::string>{42}
        };
        auto result = exp | fxt::match(
            [](const fxt::expected<int, std::string>& inner) {
                return inner | fxt::match(
                    [](int v) { return v; },
                    [](const std::string&) { return -1; }
                );
            },
            [](int) { return -2; }
        );
        REQUIRE(result == 42);
    }

    SECTION("match with tuple") {
        auto opt = fxt::optional<std::tuple<int, std::string>>{
            std::make_tuple(42, "hello")
        };
        auto result = opt | fxt::match(
            [](const std::tuple<int, std::string>& t) {
                return std::get<0>(t) + static_cast<int>(std::get<1>(t).length());
            },
            []() { return 0; }
        );
        REQUIRE(result == 47);
    }
}

TEST_CASE("match with const and non-const", "[match][expected][optional]") {
    SECTION("match on const expected") {
        const auto exp = fxt::expected<int, std::string>{42};
        auto result = exp | fxt::match(
            [](int v) { return v; },
            [](const std::string&) { return 0; }
        );
        REQUIRE(result == 42);
    }

    SECTION("match on const optional") {
        const auto opt = fxt::optional<int>{42};
        auto result = opt | fxt::match(
            [](int v) { return v; },
            []() { return 0; }
        );
        REQUIRE(result == 42);
    }

    SECTION("match on non-const expected") {
        auto exp = fxt::expected<int, std::string>{42};
        auto result = exp | fxt::match(
            [](int v) { return v; },
            [](const std::string&) { return 0; }
        );
        REQUIRE(result == 42);
    }

    SECTION("match on non-const optional") {
        auto opt = fxt::optional<int>{42};
        auto result = opt | fxt::match(
            [](int v) { return v; },
            []() { return 0; }
        );
        REQUIRE(result == 42);
    }
}

TEST_CASE("match with references", "[match][expected][optional]") {
    SECTION("match handler taking value by reference") {
        auto exp = fxt::expected<int, std::string>{42};
        auto result = exp | fxt::match(
            [](const int& v) { return v * 2; },
            [](const std::string&) { return 0; }
        );
        REQUIRE(result == 84);
    }

    SECTION("match handler taking optional value by reference") {
        auto opt = fxt::optional<int>{42};
        auto result = opt | fxt::match(
            [](const int& v) { return v * 2; },
            []() { return 0; }
        );
        REQUIRE(result == 84);
    }
}
