//
// Created by GitHub Copilot
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

TEST_CASE("transform_error on fxt::expected", "[transform_error][expected]")
{
    SECTION("transform_error on error value")
    {
        auto result = fxt::expected<int, int>{fxt::unexpected(42)}
                    | fxt::transform_error([](int x) { return x * 2; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 84);
    }

    SECTION("transform_error on success value does nothing")
    {
        auto result = fxt::expected<int, int>{100}
                    | fxt::transform_error([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 100);
    }

    SECTION("transform_error changes error type")
    {
        auto result = fxt::expected<int, int>{fxt::unexpected(404)}
                    | fxt::transform_error([](int code) {
                        return "Error " + std::to_string(code);
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error 404");
    }

    SECTION("transform_error with string errors")
    {
        auto result = fxt::expected<double, std::string>{fxt::unexpected("file not found")}
                    | fxt::transform_error([](const std::string& err) {
                        return "[ERROR] " + err;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "[ERROR] file not found");
    }

    SECTION("chaining transform_error operations")
    {
        auto result = fxt::expected<int, int>{fxt::unexpected(10)}
                    | fxt::transform_error([](int x) { return x + 5; })
                    | fxt::transform_error([](int x) { return x * 2; })
                    | fxt::transform_error([](int x) { return x - 10; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 20);  // ((10 + 5) * 2) - 10 = 20
    }

    SECTION("transform_error with complex error types")
    {
        struct ErrorInfo {
            int code;
            std::string message;
        };

        auto result = fxt::expected<int, int>{fxt::unexpected(500)}
                    | fxt::transform_error([](int code) {
                        return ErrorInfo{code, "Internal Server Error"};
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 500);
        REQUIRE(result.error().message == "Internal Server Error");
    }

    SECTION("transform_error preserves success value type")
    {
        struct Data {
            int id;
            std::string name;
        };

        auto result = fxt::expected<Data, int>{Data{1, "test"}}
                    | fxt::transform_error([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(result.value().id == 1);
        REQUIRE(result.value().name == "test");
    }

    SECTION("transform_error with lambda capture")
    {
        std::string prefix = "ERROR: ";
        auto result = fxt::expected<int, std::string>{fxt::unexpected("timeout")}
                    | fxt::transform_error([prefix](const std::string& err) {
                        return prefix + err;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "ERROR: timeout");
    }

    SECTION("transform_error on rvalue expected")
    {
        auto make_error = []() { return fxt::expected<int, int>{fxt::unexpected(7)}; };

        auto result = make_error() | fxt::transform_error([](int x) { return x * 10; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 70);
    }

    SECTION("transform_error on lvalue expected")
    {
        auto original = fxt::expected<int, int>{fxt::unexpected(3)};
        auto result = original | fxt::transform_error([](int x) { return x + 1; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 4);
        REQUIRE(!original.has_value());
        REQUIRE(original.error() == 3);  // Original unchanged
    }

    SECTION("transform_error converting error to success-like type")
    {
        auto result = fxt::expected<std::string, int>{fxt::unexpected(200)}
                    | fxt::transform_error([](int code) {
                        return "Status: " + std::to_string(code);
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Status: 200");
    }

    SECTION("transform_error with reference wrapper")
    {
        int error_count = 0;
        auto result = fxt::expected<int, int>{fxt::unexpected(1)}
                    | fxt::transform_error([&error_count](int x) {
                        error_count += x;
                        return x * 2;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 2);
        REQUIRE(error_count == 1);
    }

    SECTION("transform_error in a pipeline with other operations")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("parse error")}
                    | fxt::transform_error([](const std::string& s) { return "[" + s + "]"; })
                    | fxt::or_else([](const std::string& err) {
                        return fxt::expected<int, std::string>{-1};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == -1);
    }

    SECTION("transform_error with void error transformation side effects")
    {
        int side_effect = 0;
        auto result = fxt::expected<int, int>{fxt::unexpected(5)}
                    | fxt::transform_error([&side_effect](int x) {
                        side_effect = x * 3;
                        return x + 10;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 15);
        REQUIRE(side_effect == 15);
    }
}
