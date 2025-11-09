//
// Created by GitHub Copilot
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <vector>

TEST_CASE("tap_error on fxt::expected", "[tap_error][expected]")
{
    SECTION("tap_error executes on error")
    {
        bool error_logged = false;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee_error([&error_logged](const std::string& err) {
                        error_logged = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "error");
        REQUIRE(error_logged);
    }

    SECTION("tap_error does not execute on success")
    {
        bool executed = false;

        auto result = fxt::expected<int, std::string>{42}
                    | fxt::tee_error([&executed](const std::string& err) {
                        executed = true;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
        REQUIRE(!executed);
    }

    SECTION("tap_error does not modify the error")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected("original error")}
                    | fxt::tee_error([](const std::string& err) {
                        // Even if we try to modify, it won't affect the result
                        std::string modified = err + " modified";
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "original error");
    }

    SECTION("multiple tap_error in sequence")
    {
        std::vector<std::string> logs;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("failure")}
                    | fxt::tee_error([&logs](const std::string& err) {
                        logs.push_back("Log 1: " + err);
                    })
                    | fxt::tee_error([&logs](const std::string& err) {
                        logs.push_back("Log 2: " + err);
                    })
                    | fxt::tee_error([&logs](const std::string& err) {
                        logs.push_back("Log 3: " + err);
                    });

        REQUIRE(!result.has_value());
        REQUIRE(logs.size() == 3);
        REQUIRE(logs[0] == "Log 1: failure");
        REQUIRE(logs[1] == "Log 2: failure");
        REQUIRE(logs[2] == "Log 3: failure");
    }

    SECTION("tap_error with integer error codes")
    {
        int logged_code = 0;

        auto result = fxt::expected<std::string, int>{fxt::unexpected(404)}
                    | fxt::tee_error([&logged_code](int code) {
                        logged_code = code;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == 404);
        REQUIRE(logged_code == 404);
    }

    SECTION("tap_error with complex error types")
    {
        struct ErrorInfo {
            int code;
            std::string message;
        };

        ErrorInfo logged_error{0, ""};

        auto result = fxt::expected<int, ErrorInfo>{fxt::unexpected(ErrorInfo{500, "Server Error"})}
                    | fxt::tee_error([&logged_error](const ErrorInfo& err) {
                        logged_error = err;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 500);
        REQUIRE(logged_error.code == 500);
        REQUIRE(logged_error.message == "Server Error");
    }

    SECTION("tap_error for counting errors")
    {
        int error_count = 0;

        auto test_values = {
            fxt::expected<int, std::string>{10},
            fxt::expected<int, std::string>{fxt::unexpected("err1")},
            fxt::expected<int, std::string>{20},
            fxt::expected<int, std::string>{fxt::unexpected("err2")},
            fxt::expected<int, std::string>{30}
        };

        for (const auto& val : test_values)
        {
            auto result = val | fxt::tee_error([&error_count](const std::string& err) {
                error_count++;
            });
        }

        REQUIRE(error_count == 2);
    }

    SECTION("combining tap and tap_error")
    {
        bool value_logged = false;
        bool error_logged = false;

        auto result1 = fxt::expected<int, std::string>{42}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_error([&error_logged](const std::string& err) { error_logged = true; });

        REQUIRE(result1.has_value());
        REQUIRE(value_logged);
        REQUIRE(!error_logged);

        value_logged = false;
        error_logged = false;

        auto result2 = fxt::expected<int, std::string>{fxt::unexpected("error")}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_error([&error_logged](const std::string& err) { error_logged = true; });

        REQUIRE(!result2.has_value());
        REQUIRE(!value_logged);
        REQUIRE(error_logged);
    }

    SECTION("tap_error in pipeline with or_else")
    {
        bool error_logged = false;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee_error([&error_logged](const std::string& err) {
                        error_logged = true;
                    })
                    | fxt::or_else([](const std::string& err) {
                        return fxt::expected<int, std::string>{0};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 0);
        REQUIRE(error_logged);
    }

    SECTION("tap_error with transform_error")
    {
        std::string original_error;
        std::string transformed_error;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee_error([&original_error](const std::string& err) {
                        original_error = err;
                    })
                    | fxt::transform_error([](const std::string& err) {
                        return "[WRAPPED] " + err;
                    })
                    | fxt::tee_error([&transformed_error](const std::string& err) {
                        transformed_error = err;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(original_error == "error");
        REQUIRE(transformed_error == "[WRAPPED] error");
    }

    SECTION("tap_error preserves lvalue reference")
    {
        std::string logged_error;
        auto original = fxt::expected<int, std::string>{fxt::unexpected("test error")};

        auto result = original | fxt::tee_error([&logged_error](const std::string& err) {
            logged_error = err;
        });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "test error");
        REQUIRE(!original.has_value());
        REQUIRE(original.error() == "test error");
        REQUIRE(logged_error == "test error");
    }

    SECTION("tap_error on rvalue expected")
    {
        std::string logged_error;

        auto make_error = []() {
            return fxt::expected<int, std::string>{fxt::unexpected("rvalue error")};
        };

        auto result = make_error() | fxt::tee_error([&logged_error](const std::string& err) {
            logged_error = err;
        });

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "rvalue error");
        REQUIRE(logged_error == "rvalue error");
    }

    SECTION("tap_error with mutable lambda capture")
    {
        int error_accumulator = 0;

        auto result = fxt::expected<int, int>{fxt::unexpected(10)}
                    | fxt::tee_error([&error_accumulator](int code) {
                        error_accumulator += code;
                    })
                    | fxt::transform_error([](int code) { return code * 2; })
                    | fxt::tee_error([&error_accumulator](int code) {
                        error_accumulator += code;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(error_accumulator == 30);  // 10 + 20
    }
}

TEST_CASE("tap_none on fxt::optional", "[tap_none][optional]")
{
    SECTION("tap_none executes on empty optional")
    {
        bool none_logged = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(none_logged);
    }

    SECTION("tap_none does not execute when optional has value")
    {
        bool executed = false;

        auto result = fxt::optional<int>{42}
                    | fxt::tee_none([&executed]() {
                        executed = true;
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 42);
        REQUIRE(!executed);
    }

    SECTION("multiple tap_none in sequence")
    {
        std::vector<std::string> logs;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee_none([&logs]() {
                        logs.push_back("Log 1");
                    })
                    | fxt::tee_none([&logs]() {
                        logs.push_back("Log 2");
                    })
                    | fxt::tee_none([&logs]() {
                        logs.push_back("Log 3");
                    });

        REQUIRE(!result.has_value());
        REQUIRE(logs.size() == 3);
        REQUIRE(logs[0] == "Log 1");
        REQUIRE(logs[1] == "Log 2");
        REQUIRE(logs[2] == "Log 3");
    }

    SECTION("tap_none with string optional")
    {
        bool none_detected = false;

        auto result = fxt::optional<std::string>{fxt::nullopt}
                    | fxt::tee_none([&none_detected]() {
                        none_detected = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(none_detected);
    }

    SECTION("tap_none for counting empty optionals")
    {
        int empty_count = 0;

        auto test_values = {
            fxt::optional<int>{10},
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{20},
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{30}
        };

        for (const auto& val : test_values)
        {
            auto result = val | fxt::tee_none([&empty_count]() {
                empty_count++;
            });
        }

        REQUIRE(empty_count == 2);
    }

    SECTION("combining tap and tap_none")
    {
        bool value_logged = false;
        bool none_logged = false;

        auto result1 = fxt::optional<int>{42}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_none([&none_logged]() { none_logged = true; });

        REQUIRE(result1.has_value());
        REQUIRE(value_logged);
        REQUIRE(!none_logged);

        value_logged = false;
        none_logged = false;

        auto result2 = fxt::optional<int>{fxt::nullopt}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_none([&none_logged]() { none_logged = true; });

        REQUIRE(!result2.has_value());
        REQUIRE(!value_logged);
        REQUIRE(none_logged);
    }

    SECTION("tap_none in pipeline with or_else")
    {
        bool none_logged = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    })
                    | fxt::or_else([]() {
                        return fxt::optional<int>{-1};
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == -1);
        REQUIRE(none_logged);
    }

    SECTION("tap_none with value_or")
    {
        bool none_logged = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    })
                    | fxt::value_or(0);

        REQUIRE(result == 0);
        REQUIRE(none_logged);
    }

    SECTION("tap_none preserves lvalue reference")
    {
        bool none_logged = false;
        auto original = fxt::optional<int>{fxt::nullopt};

        auto result = original | fxt::tee_none([&none_logged]() {
            none_logged = true;
        });

        REQUIRE(!result.has_value());
        REQUIRE(!original.has_value());
        REQUIRE(none_logged);
    }

    SECTION("tap_none on rvalue optional")
    {
        bool none_logged = false;

        auto make_empty = []() {
            return fxt::optional<int>{fxt::nullopt};
        };

        auto result = make_empty() | fxt::tee_none([&none_logged]() {
            none_logged = true;
        });

        REQUIRE(!result.has_value());
        REQUIRE(none_logged);
    }

    SECTION("tap_none with complex types")
    {
        struct Data {
            int id;
            std::string name;
        };

        bool none_logged = false;

        auto result = fxt::optional<Data>{fxt::nullopt}
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(none_logged);
    }

    SECTION("tap_none with to_expected conversion")
    {
        bool none_logged = false;

        auto result = fxt::optional<int>{fxt::nullopt}
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    })
                    | fxt::to_expected<std::string>("no value");

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "no value");
        REQUIRE(none_logged);
    }

    SECTION("tap_none with mutable lambda capture")
    {
        int empty_counter = 0;

        auto result1 = fxt::optional<int>{fxt::nullopt}
                     | fxt::tee_none([&empty_counter]() {
                         empty_counter++;
                     });

        auto result2 = fxt::optional<int>{fxt::nullopt}
                     | fxt::tee_none([&empty_counter]() {
                         empty_counter++;
                     });

        auto result3 = fxt::optional<int>{42}
                     | fxt::tee_none([&empty_counter]() {
                         empty_counter++;
                     });

        REQUIRE(empty_counter == 2);
    }
}

TEST_CASE("tap_error and tap_none combined scenarios", "[tap_error][tap_none][expected][optional]")
{
    SECTION("expected to optional with tap_error and tap_none")
    {
        bool error_logged = false;
        bool none_logged = false;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("error")}
                    | fxt::tee_error([&error_logged](const std::string& err) {
                        error_logged = true;
                    })
                    | fxt::to_optional()
                    | fxt::tee_none([&none_logged]() {
                        none_logged = true;
                    });

        REQUIRE(!result.has_value());
        REQUIRE(error_logged);
        REQUIRE(none_logged);
    }

    SECTION("tracking error propagation through conversions")
    {
        std::vector<std::string> trace;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("initial error")}
                    | fxt::tee_error([&trace](const std::string& err) {
                        trace.push_back("Error in expected: " + err);
                    })
                    | fxt::transform_error([](const std::string& err) {
                        return "[WRAPPED] " + err;
                    })
                    | fxt::tee_error([&trace](const std::string& err) {
                        trace.push_back("Transformed error: " + err);
                    })
                    | fxt::to_optional()
                    | fxt::tee_none([&trace]() {
                        trace.push_back("Converted to empty optional");
                    });

        REQUIRE(!result.has_value());
        REQUIRE(trace.size() == 3);
        REQUIRE(trace[0] == "Error in expected: initial error");
        REQUIRE(trace[1] == "Transformed error: [WRAPPED] initial error");
        REQUIRE(trace[2] == "Converted to empty optional");
    }

    SECTION("all three tap variants in one pipeline")
    {
        bool value_logged = false;
        bool error_logged = false;
        bool none_logged = false;

        // Success case
        auto result1 = fxt::expected<int, std::string>{42}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_error([&error_logged](const std::string& err) { error_logged = true; })
                     | fxt::to_optional()
                     | fxt::tee_none([&none_logged]() { none_logged = true; });

        REQUIRE(result1.has_value());
        REQUIRE(value_logged);
        REQUIRE(!error_logged);
        REQUIRE(!none_logged);

        // Reset flags
        value_logged = false;
        error_logged = false;
        none_logged = false;

        // Error case
        auto result2 = fxt::expected<int, std::string>{fxt::unexpected("error")}
                     | fxt::tee([&value_logged](int x) { value_logged = true; })
                     | fxt::tee_error([&error_logged](const std::string& err) { error_logged = true; })
                     | fxt::to_optional()
                     | fxt::tee_none([&none_logged]() { none_logged = true; });

        REQUIRE(!result2.has_value());
        REQUIRE(!value_logged);
        REQUIRE(error_logged);
        REQUIRE(none_logged);
    }

    SECTION("conditional logging based on tap_none execution")
    {
        struct Stats {
            int found = 0;
            int not_found = 0;
        };

        Stats stats;

        auto process = [&stats](const fxt::optional<int>& opt) {
            return opt
                 | fxt::tee([&stats](int x) { stats.found++; })
                 | fxt::tee_none([&stats]() { stats.not_found++; });
        };

        process(fxt::optional<int>{10});
        process(fxt::optional<int>{fxt::nullopt});
        process(fxt::optional<int>{20});
        process(fxt::optional<int>{fxt::nullopt});
        process(fxt::optional<int>{30});

        REQUIRE(stats.found == 3);
        REQUIRE(stats.not_found == 2);
    }

    SECTION("error recovery with tap_error logging")
    {
        std::vector<std::string> recovery_log;

        auto result = fxt::expected<int, std::string>{fxt::unexpected("network error")}
                    | fxt::tee_error([&recovery_log](const std::string& err) {
                        recovery_log.push_back("Detected: " + err);
                    })
                    | fxt::or_else([&recovery_log](const std::string& err) {
                        recovery_log.push_back("Recovering from: " + err);
                        return fxt::expected<int, std::string>{0};
                    })
                    | fxt::tee([&recovery_log](int x) {
                        recovery_log.push_back("Using default: " + std::to_string(x));
                    });

        REQUIRE(result.has_value());
        REQUIRE(result.value() == 0);
        REQUIRE(recovery_log.size() == 3);
        REQUIRE(recovery_log[0] == "Detected: network error");
        REQUIRE(recovery_log[1] == "Recovering from: network error");
        REQUIRE(recovery_log[2] == "Using default: 0");
    }
}

