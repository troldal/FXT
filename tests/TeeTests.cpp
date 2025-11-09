//
// Created by GitHub Copilot
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <vector>

TEST_CASE("tap on fxt::expected", "[tap][expected]")
{
    SECTION("tap executes on success value")
    {
        int side_effect_value = 0;

        auto result = fxt::expected<int, std::string>{42}
                    | fxt::tee([&side_effect_value](int x) {
                        side_effect_value = x;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
        REQUIRE(side_effect_value == 42);
    }

    SECTION("tap does not execute on error")
    {
        bool executed = false;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee([&executed](int x) {
                        executed = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "error");
        REQUIRE(!executed);
    }

    SECTION("tap does not modify the value")
    {
        auto result = fxt::expected<int, std::string>{100}
                    | fxt::tee([](int x) {
                        // Even if we try to modify x, it won't affect the result
                        int modified = x * 2;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 100);
    }

    SECTION("multiple taps in sequence")
    {
        std::vector<int> trace;

        auto result = fxt::expected<int, std::string>{10}
                    | fxt::tee([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tee([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x + 5; })
                    | fxt::tee([&trace](int x) { trace.push_back(x); });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 25);
        REQUIRE(trace.size() == 3);
        REQUIRE(trace[0] == 10);
        REQUIRE(trace[1] == 20);
        REQUIRE(trace[2] == 25);
    }

    SECTION("tap with string values")
    {
        std::string logged_value;

        auto result = fxt::expected<std::string, int>{"hello"}
                    | fxt::tee([&logged_value](const std::string& s) {
                        logged_value = s;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "hello");
        REQUIRE(logged_value == "hello");
    }

    SECTION("tap with complex types")
    {
        struct Point {
            int x, y;
        };

        Point logged_point{0, 0};

        auto result = fxt::expected<Point, std::string>{Point{3, 4}}
                    | fxt::tee([&logged_point](const Point& p) {
                        logged_point = p;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value().x == 3);
        REQUIRE(result.value().y == 4);
        REQUIRE(logged_point.x == 3);
        REQUIRE(logged_point.y == 4);
    }

    SECTION("tap for counting operations")
    {
        int count = 0;

        auto result = fxt::expected<int, std::string>{5}
                    | fxt::tee([&count](int x) { count++; })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tee([&count](int x) { count++; })
                    | fxt::transform([](int x) { return x + 3; })
                    | fxt::tee([&count](int x) { count++; });

        REQUIRE(result.has_value());
        REQUIRE(count == 3);
    }

    SECTION("tap in pipeline with and_then")
    {
        bool tap_executed = false;

        auto result = fxt::expected<int, std::string>{10}
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::and_then([](int x) -> fxt::expected<std::string, std::string> {
                        return "value: " + std::to_string(x);
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "value: 10");
        REQUIRE(tap_executed);
    }

    SECTION("tap with or_else (tap not executed on error)")
    {
        bool tap_executed = false;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::or_else([](const std::string& err) {
                        return fxt::expected<int, std::string>{0};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 0);
        REQUIRE(!tap_executed);
    }

    SECTION("tap preserves lvalue reference")
    {
        int side_effect = 0;
        auto original = fxt::expected<int, std::string>{42};

        auto result = original | fxt::tee([&side_effect](int x) { side_effect = x; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
        REQUIRE(original.has_value());
        REQUIRE(original.value() == 42);
        REQUIRE(side_effect == 42);
    }

    SECTION("tap on rvalue expected")
    {
        int side_effect = 0;

        auto make_expected = []() { return fxt::expected<int, std::string>{99}; };

        auto result = make_expected() | fxt::tee([&side_effect](int x) { side_effect = x; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 99);
        REQUIRE(side_effect == 99);
    }
}

TEST_CASE("tap on fxt::optional", "[tap][optional]")
{
    SECTION("tap executes on optional with value")
    {
        int side_effect_value = 0;

        auto result = fxt::optional<int>{42}
                    | fxt::tee([&side_effect_value](int x) {
                        side_effect_value = x;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
        REQUIRE(side_effect_value == 42);
    }

    SECTION("tap does not execute on empty optional")
    {
        bool executed = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee([&executed](int x) {
                        executed = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(!executed);
    }

    SECTION("tap does not modify the value")
    {
        auto result = fxt::optional<int>{100}
                    | fxt::tee([](int x) {
                        int modified = x * 2;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 100);
    }

    SECTION("multiple taps in sequence")
    {
        std::vector<int> trace;

        auto result = fxt::optional<int>{10}
                    | fxt::tee([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tee([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x + 5; })
                    | fxt::tee([&trace](int x) { trace.push_back(x); });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 25);
        REQUIRE(trace.size() == 3);
        REQUIRE(trace[0] == 10);
        REQUIRE(trace[1] == 20);
        REQUIRE(trace[2] == 25);
    }

    SECTION("tap with string values")
    {
        std::string logged_value;

        auto result = fxt::optional<std::string>{"world"}
                    | fxt::tee([&logged_value](const std::string& s) {
                        logged_value = s;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "world");
        REQUIRE(logged_value == "world");
    }

    SECTION("tap for counting operations")
    {
        int count = 0;

        auto result = fxt::optional<int>{7}
                    | fxt::tee([&count](int x) { count++; })
                    | fxt::transform([](int x) { return x * 3; })
                    | fxt::tee([&count](int x) { count++; })
                    | fxt::transform([](int x) { return x - 1; })
                    | fxt::tee([&count](int x) { count++; });

        REQUIRE(result.has_value());
        REQUIRE(count == 3);
    }

    SECTION("tap in pipeline with and_then")
    {
        bool tap_executed = false;

        auto result = fxt::optional<int>{15}
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::and_then([](int x) -> fxt::optional<std::string> {
                        return "number: " + std::to_string(x);
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "number: 15");
        REQUIRE(tap_executed);
    }

    SECTION("tap with or_else (tap not executed on empty)")
    {
        bool tap_executed = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::or_else([]() {
                        return fxt::optional<int>{-1};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == -1);
        REQUIRE(!tap_executed);
    }

    SECTION("tap preserves lvalue reference")
    {
        int side_effect = 0;
        auto original = fxt::optional<int>{88};

        auto result = original | fxt::tee([&side_effect](int x) { side_effect = x; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 88);
        REQUIRE(original.has_value());
        REQUIRE(original.value() == 88);
        REQUIRE(side_effect == 88);
    }

    SECTION("tap on rvalue optional")
    {
        int side_effect = 0;

        auto make_optional = []() { return fxt::optional<int>{77}; };

        auto result = make_optional() | fxt::tee([&side_effect](int x) { side_effect = x; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 77);
        REQUIRE(side_effect == 77);
    }

    SECTION("tap with value_or")
    {
        int side_effect = 0;

        auto result = fxt::optional<int>{33}
                    | fxt::tee([&side_effect](int x) { side_effect = x; })
                    | fxt::value_or(0);

        REQUIRE(result == 33);
        REQUIRE(side_effect == 33);
    }

    SECTION("tap with to_expected conversion")
    {
        bool tap_executed = false;

        auto result = fxt::optional<int>{25}
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::to_expected<std::string>("no value");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 25);
        REQUIRE(tap_executed);
    }
}

TEST_CASE("tap with complex scenarios", "[tap][expected][optional]")
{
    SECTION("tap can collect debug traces")
    {
        std::vector<std::string> debug_log;

        auto result = fxt::expected<int, std::string>{5}
                    | fxt::tee([&debug_log](int x) {
                        debug_log.push_back("Initial: " + std::to_string(x));
                    })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tee([&debug_log](int x) {
                        debug_log.push_back("After *2: " + std::to_string(x));
                    })
                    | fxt::transform([](int x) { return x + 10; })
                    | fxt::tee([&debug_log](int x) {
                        debug_log.push_back("After +10: " + std::to_string(x));
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 20);
        REQUIRE(debug_log.size() == 3);
        REQUIRE(debug_log[0] == "Initial: 5");
        REQUIRE(debug_log[1] == "After *2: 10");
        REQUIRE(debug_log[2] == "After +10: 20");
    }

    SECTION("tap for validation checks")
    {
        bool validation_passed = false;

        auto result = fxt::optional<int>{50}
                    | fxt::tee([&validation_passed](int x) {
                        validation_passed = (x >= 0 && x <= 100);
                    });

        REQUIRE(result.has_value());
        REQUIRE(validation_passed);
    }

    SECTION("tap with mutable lambda capture")
    {
        int counter = 0;

        auto result = fxt::expected<int, std::string>{1}
                    | fxt::tee([&counter](int x) { counter += x; })
                    | fxt::transform([](int x) { return x + 1; })
                    | fxt::tee([&counter](int x) { counter += x; })
                    | fxt::transform([](int x) { return x + 1; })
                    | fxt::tee([&counter](int x) { counter += x; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 3);
        REQUIRE(counter == 6);  // 1 + 2 + 3
    }

    SECTION("tap does not interfere with error propagation")
    {
        bool tap_executed = false;

        auto result = fxt::expected<int, std::string>{10}
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::and_then([](int x) -> fxt::expected<int, std::string> {
                        return fxt::unexpected("error in and_then");
                    })
                    | fxt::tee([&tap_executed](int x) { tap_executed = true; })
                    | fxt::transform([](int x) { return x + 5; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "error in and_then");
        REQUIRE(!tap_executed);
    }

    SECTION("tap with both expected and optional in same pipeline")
    {
        int expected_tap_count = 0;
        int optional_tap_count = 0;

        auto result = fxt::expected<int, std::string>{10}
                    | fxt::tee([&expected_tap_count](int x) { expected_tap_count++; })
                    | fxt::to_optional()
                    | fxt::tee([&optional_tap_count](int x) { optional_tap_count++; })
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 20);
        REQUIRE(expected_tap_count == 1);
        REQUIRE(optional_tap_count == 1);
    }
}

