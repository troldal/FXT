//
// Created by GitHub Copilot
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

TEST_CASE("to_expected on fxt::optional", "[to_expected][optional]")
{
    SECTION("to_expected on optional with value")
    {
        auto result = fxt::optional<int>{42} | fxt::to_expected<std::string>("error");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
    }

    SECTION("to_expected on empty optional returns error")
    {
        auto result = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("no value");

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "no value");
    }

    SECTION("to_expected with string value")
    {
        auto result = fxt::optional<std::string>{"hello"} | fxt::to_expected<int>(404);

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "hello");
    }

    SECTION("to_expected with integer error code")
    {
        auto result = fxt::optional<double>{fxt::nullopt} | fxt::to_expected<int>(404);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 404);
    }

    SECTION("to_expected with different error types")
    {
        auto string_result = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("not found");
        auto int_result = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<int>(500);

        REQUIRE(!string_result.has_value());
        REQUIRE(string_result.error() == "not found");
        REQUIRE(!int_result.has_value());
        REQUIRE(int_result.error() == 500);
    }

    SECTION("to_expected preserves value type")
    {
        struct Data {
            int id;
            std::string name;
        };

        auto result = fxt::optional<Data>{Data{1, "test"}} | fxt::to_expected<int>(0);

        REQUIRE(result.has_value());
        REQUIRE(result.value().id == 1);
        REQUIRE(result.value().name == "test");
    }

    SECTION("to_expected on lvalue optional")
    {
        auto original = fxt::optional<int>{100};
        auto result = original | fxt::to_expected<std::string>("error");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 100);
        REQUIRE(original.has_value());
        REQUIRE(original.value() == 100);  // Original unchanged
    }

    SECTION("to_expected on rvalue optional")
    {
        auto make_optional = []() { return fxt::optional<int>{50}; };

        auto result = make_optional() | fxt::to_expected<std::string>("error");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 50);
    }

    SECTION("chaining to_expected with transform")
    {
        auto result = fxt::optional<int>{10}
                    | fxt::to_expected<std::string>("no value")
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 20);
    }

    SECTION("chaining to_expected with transform on empty")
    {
        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::to_expected<std::string>("no value")
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "no value");
    }

    SECTION("to_expected with structured error type")
    {
        struct ErrorInfo {
            int code;
            std::string message;
        };

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::to_expected<ErrorInfo>(ErrorInfo{404, "Not found"});

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 404);
        REQUIRE(result.error().message == "Not found");
    }

    SECTION("to_expected with complex value type")
    {
        using ComplexType = std::pair<int, std::string>;
        auto result = fxt::optional<ComplexType>{ComplexType{42, "answer"}}
                    | fxt::to_expected<std::string>("error");

        REQUIRE(result.has_value());
        REQUIRE(result.value().first == 42);
        REQUIRE(result.value().second == "answer");
    }

    SECTION("to_expected in pipeline with and_then")
    {
        auto result = fxt::optional<int>{10}
                    | fxt::to_expected<std::string>("no value")
                    | fxt::and_then([](int x) -> fxt::expected<std::string, std::string> {
                        return "Result: " + std::to_string(x * 2);
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "Result: 20");
    }

    SECTION("to_expected with zero value")
    {
        auto result = fxt::optional<int>{0} | fxt::to_expected<std::string>("no value");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 0);
    }

    SECTION("to_expected with empty string value")
    {
        auto result = fxt::optional<std::string>{""} | fxt::to_expected<int>(404);

        REQUIRE(result.has_value());
        REQUIRE(result.value() == "");
    }

    SECTION("to_expected with boolean value")
    {
        auto true_result = fxt::optional<bool>{true} | fxt::to_expected<int>(0);
        auto false_result = fxt::optional<bool>{false} | fxt::to_expected<int>(0);

        REQUIRE(true_result.has_value());
        REQUIRE(true_result.value() == true);
        REQUIRE(false_result.has_value());
        REQUIRE(false_result.value() == false);
    }

    SECTION("to_expected with pointer value")
    {
        int value = 42;
        auto result = fxt::optional<int*>{&value} | fxt::to_expected<std::string>("null pointer");

        REQUIRE(result.has_value());
        REQUIRE(*result.value() == 42);
    }

    SECTION("multiple to_expected conversions")
    {
        auto opt1 = fxt::optional<int>{10};
        auto opt2 = fxt::optional<int>{20};
        auto opt3 = fxt::optional<int>{fxt::nullopt};

        auto exp1 = opt1 | fxt::to_expected<std::string>("error");
        auto exp2 = opt2 | fxt::to_expected<std::string>("error");
        auto exp3 = opt3 | fxt::to_expected<std::string>("error");

        REQUIRE(exp1.has_value());
        REQUIRE(exp2.has_value());
        REQUIRE(!exp3.has_value());
        REQUIRE(exp1.value() == 10);
        REQUIRE(exp2.value() == 20);
        REQUIRE(exp3.error() == "error");
    }

    SECTION("to_expected with floating point")
    {
        auto result = fxt::optional<double>{3.14159} | fxt::to_expected<std::string>("no value");

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 3.14159);
    }

    SECTION("to_expected with or_else")
    {
        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::to_expected<std::string>("original error")
                    | fxt::or_else([](const std::string& err) {
                        return fxt::expected<int, std::string>{42};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
    }

    SECTION("to_expected with transform_error")
    {
        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::to_expected<int>(404)
                    | fxt::transform_error([](int code) {
                        return "Error code: " + std::to_string(code);
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error code: 404");
    }

    SECTION("to_expected then to_optional roundtrip with value")
    {
        auto original = fxt::optional<int>{42};
        auto result = original
                    | fxt::to_expected<std::string>("error")
                    | fxt::to_optional();

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
    }

    SECTION("to_expected then to_optional roundtrip with empty")
    {
        auto original = fxt::optional<int>{fxt::nullopt};
        auto result = original
                    | fxt::to_expected<std::string>("error")
                    | fxt::to_optional();

        REQUIRE(!result.has_value());
    }

    SECTION("to_expected with different error messages")
    {
        auto err1 = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("File not found");
        auto err2 = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("Access denied");
        auto err3 = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("Timeout");

        REQUIRE(!err1.has_value());
        REQUIRE(!err2.has_value());
        REQUIRE(!err3.has_value());
        REQUIRE(err1.error() == "File not found");
        REQUIRE(err2.error() == "Access denied");
        REQUIRE(err3.error() == "Timeout");
    }

    SECTION("to_expected in conditional expression")
    {
        auto maybe_value = fxt::optional<int>{42} | fxt::to_expected<std::string>("error");

        if (maybe_value.has_value())
        {
            REQUIRE(maybe_value.value() == 42);
        }
        else
        {
            REQUIRE(false);  // Should not reach here
        }
    }

    SECTION("to_expected with const error value")
    {
        auto result = fxt::optional<int>{fxt::nullopt} | fxt::to_expected<std::string>("constant error");

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "constant error");
    }

    SECTION("to_expected enables error-specific handling")
    {
        auto process = [](const fxt::optional<int>& opt) {
            return opt | fxt::to_expected<std::string>("Processing failed");
        };

        auto success = process(fxt::optional<int>{42});
        auto failure = process(fxt::optional<int>{fxt::nullopt});

        REQUIRE(success.has_value());
        REQUIRE(!failure.has_value());
        REQUIRE(failure.error() == "Processing failed");
    }
}

TEST_CASE("to_expected with error factory (lazy overload)", "[to_expected][optional][factory]")
{
    SECTION("factory overload: value path — factory is never called")
    {
        bool factory_called = false;
        auto result = fxt::optional<int>{42}
                    | fxt::to_expected([&]{ factory_called = true; return std::string{"no value"}; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
        REQUIRE_FALSE(factory_called);
    }

    SECTION("factory overload: empty optional — factory is called once")
    {
        int call_count = 0;
        auto result = fxt::optional<int>{}
                    | fxt::to_expected([&]{ ++call_count; return std::string{"no value"}; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "no value");
        REQUIRE(call_count == 1);
    }

    SECTION("factory return type becomes the error type")
    {
        auto result = fxt::optional<int>{}
                    | fxt::to_expected([]{ return 404; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == 404);
    }

    SECTION("factory overload chains with transform on success")
    {
        auto result = fxt::optional<int>{10}
                    | fxt::to_expected([]{ return std::string{"missing"}; })
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("factory overload chains with transform on failure")
    {
        auto result = fxt::optional<int>{}
                    | fxt::to_expected([]{ return std::string{"missing"}; })
                    | fxt::transform([](int x) { return x * 2; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "missing");
    }

    SECTION("stored factory adaptor can be applied multiple times")
    {
        int call_count = 0;
        auto adaptor = fxt::to_expected([&]{ ++call_count; return std::string{"err"}; });

        auto r1 = fxt::optional<int>{1}  | adaptor;
        auto r2 = fxt::optional<int>{}   | adaptor;
        auto r3 = fxt::optional<int>{3}  | adaptor;
        auto r4 = fxt::optional<int>{}   | adaptor;

        REQUIRE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());
        REQUIRE(r3.has_value());
        REQUIRE_FALSE(r4.has_value());
        REQUIRE(call_count == 2);
    }

    SECTION("rvalue optional moves its value — factory overload")
    {
        auto make_opt = []{ return fxt::optional<int>{99}; };
        auto result = make_opt() | fxt::to_expected([]{ return std::string{"err"}; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }
}
