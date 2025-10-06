//
// Created by GitHub Copilot
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

TEST_CASE("to_optional on fxt::expected", "[to_optional][expected]")
{
    SECTION("to_optional on success value")
    {
        auto result = fxt::expected<int, std::string>{42} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
    }

    SECTION("to_optional on error value returns empty optional")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")} | fxt::to_optional();

        REQUIRE(!result.has_value());
    }

    SECTION("to_optional with string value")
    {
        auto result = fxt::expected<std::string, int>{std::string("hello")} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "hello");
    }

    SECTION("to_optional with string error")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("file not found")} | fxt::to_optional();

        REQUIRE(!result.has_value());
    }

    SECTION("to_optional with different error types")
    {
        auto int_error = fxt::expected<double, int>{fxt::unexpected(404)} | fxt::to_optional();
        auto string_error = fxt::expected<double, std::string>{fxt::unexpected("error")} | fxt::to_optional();

        REQUIRE(!int_error.has_value());
        REQUIRE(!string_error.has_value());
    }

    SECTION("to_optional preserves value type")
    {
        struct Data {
            int id;
            std::string name;
        };

        auto result = fxt::expected<Data, int>{Data{1, "test"}} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value().id == 1);
        REQUIRE(result.value().name == "test");
    }

    SECTION("to_optional on lvalue expected")
    {
        auto original = fxt::expected<int, std::string>{100};
        auto result = original | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 100);
        REQUIRE(original.has_value());
        REQUIRE(original.value() == 100);  // Original unchanged
    }

    SECTION("to_optional on rvalue expected")
    {
        auto make_expected = []() { return fxt::expected<int, std::string>{50}; };

        auto result = make_expected() | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 50);
    }

    SECTION("chaining to_optional with value_or")
    {
        auto success = fxt::expected<int, std::string>{42}
                     | fxt::to_optional()
                     | fxt::value_or(0);
        auto error = fxt::expected<int, std::string>{fxt::unexpected("error")}
                   | fxt::to_optional()
                   | fxt::value_or(0);

        REQUIRE(success == 42);
        REQUIRE(error == 0);
    }

    SECTION("chaining to_optional with transform")
    {
        auto result = fxt::expected<int, std::string>{10}
                    | fxt::to_optional()
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 20);
    }

    SECTION("chaining to_optional with transform on error")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::to_optional()
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(!result.has_value());
    }

    SECTION("to_optional with complex value type")
    {
        using ComplexType = std::pair<int, std::string>;
        auto result = fxt::expected<ComplexType, int>{ComplexType{42, "answer"}}
                    | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value().first == 42);
        REQUIRE(result.value().second == "answer");
    }

    SECTION("to_optional discards specific error information")
    {
        auto error1 = fxt::expected<int, std::string>{fxt::unexpected("error type 1")} | fxt::to_optional();
        auto error2 = fxt::expected<int, std::string>{fxt::unexpected("error type 2")} | fxt::to_optional();

        // Both become empty optionals, error details are lost
        REQUIRE(!error1.has_value());
        REQUIRE(!error2.has_value());
        // No way to distinguish between different errors
    }

    SECTION("to_optional in pipeline with and_then")
    {
        auto result = fxt::expected<int, std::string>{10}
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::to_optional()
                    | fxt::and_then([](int x) -> fxt::optional<std::string> {
                        return std::to_string(x);
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "20");
    }

    SECTION("to_optional with zero value")
    {
        auto result = fxt::expected<int, std::string>{0} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 0);
    }

    SECTION("to_optional with empty string value")
    {
        auto result = fxt::expected<std::string, int>{std::string("")} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "");
    }

    SECTION("to_optional with boolean value")
    {
        auto true_result = fxt::expected<bool, int>{true} | fxt::to_optional();
        auto false_result = fxt::expected<bool, int>{false} | fxt::to_optional();

        REQUIRE(true_result.has_value());
        REQUIRE(true_result.value() == true);
        REQUIRE(false_result.has_value());
        REQUIRE(false_result.value() == false);
    }

    SECTION("to_optional with pointer value")
    {
        int value = 42;
        auto result = fxt::expected<int*, std::string>{&value} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(*result.value() == 42);
    }

    SECTION("multiple to_optional conversions")
    {
        auto exp1 = fxt::expected<int, std::string>{10};
        auto exp2 = fxt::expected<int, std::string>{20};
        auto exp3 = fxt::expected<int, std::string>{fxt::unexpected("error")};

        auto opt1 = exp1 | fxt::to_optional();
        auto opt2 = exp2 | fxt::to_optional();
        auto opt3 = exp3 | fxt::to_optional();

        REQUIRE(opt1.has_value());
        REQUIRE(opt2.has_value());
        REQUIRE(!opt3.has_value());
        REQUIRE(opt1.value() == 10);
        REQUIRE(opt2.value() == 20);
    }

    SECTION("to_optional with floating point")
    {
        auto result = fxt::expected<double, std::string>{3.14159} | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 3.14159);
    }

    SECTION("to_optional in conditional expression")
    {
        auto maybe_value = fxt::expected<int, std::string>{42} | fxt::to_optional();

        if (auto val = maybe_value)
        {
            REQUIRE(*val == 42);
        }
        else
        {
            REQUIRE(false);  // Should not reach here
        }
    }

    SECTION("to_optional with or_else on optional")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::to_optional()
                    | fxt::or_else([]() { return fxt::optional<int>{99}; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 99);
    }
}

