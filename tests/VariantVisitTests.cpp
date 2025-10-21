//
// Created by kenne on 21/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::visit with fxt::variant", "[visit][variant]")
{
    SECTION("Visit variant holding int")
    {
        fxt::variant<int, double, std::string> v{42};

        auto result = fxt::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, v);

        REQUIRE(result == "42");
    }

    SECTION("Visit variant holding double")
    {
        fxt::variant<int, double, std::string> v{3.14};

        auto result = fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, v);

        REQUIRE(result.substr(0, 4) == "3.14");
    }

    SECTION("Visit variant holding string")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};

        auto result = fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, v);

        REQUIRE(result == "hello");
    }

    SECTION("Type-specific visitor")
    {
        auto type_visitor = [](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                return "int";
            } else if constexpr (std::is_same_v<T, double>) {
                return "double";
            } else {
                return "string";
            }
        };

        fxt::variant<int, double, std::string> v1{42};
        fxt::variant<int, double, std::string> v2{3.14};
        fxt::variant<int, double, std::string> v3{std::string{"test"}};

        REQUIRE(fxt::visit(type_visitor, v1) == "int");
        REQUIRE(fxt::visit(type_visitor, v2) == "double");
        REQUIRE(fxt::visit(type_visitor, v3) == "string");
    }

    SECTION("Visitor that returns different types")
    {
        fxt::variant<int, double, std::string> v{42};

        auto result = fxt::visit([](auto&& val) -> int {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return static_cast<int>(val);
            } else {
                return static_cast<int>(val.length());
            }
        }, v);

        REQUIRE(result == 42);
    }
}

TEST_CASE("fxt::visit with pipe operator", "[visit][variant][pipe]")
{
    SECTION("Visit using pipe operator - int")
    {
        fxt::variant<int, double, std::string> v{42};

        auto result = v | fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE(result == "42");
    }

    SECTION("Visit using pipe operator - double")
    {
        fxt::variant<int, double, std::string> v{3.14};

        auto result = v | fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE(result.substr(0, 4) == "3.14");
    }

    SECTION("Visit using pipe operator - string")
    {
        fxt::variant<int, double, std::string> v{std::string{"world"}};

        auto result = v | fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE(result == "world");
    }

    SECTION("Pipe operator with const variant")
    {
        const fxt::variant<int, double, std::string> v{42};

        auto result = v | fxt::visit([](auto&& val) -> int {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return static_cast<int>(val);
            } else {
                return static_cast<int>(val.length());
            }
        });

        REQUIRE(result == 42);
    }
}

TEST_CASE("fxt::mvisit with fxt::optional<fxt::variant>", "[mvisit][optional][variant]")
{
    SECTION("Visit optional containing variant - direct call")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};

        auto result = fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, opt);

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("Visit optional with different variant alternatives")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt1{fxt::variant<int, double, std::string>{42}};
        fxt::optional<fxt::variant<int, double, std::string>> opt2{fxt::variant<int, double, std::string>{3.14}};
        fxt::optional<fxt::variant<int, double, std::string>> opt3{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto visitor = [](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                return "int: " + std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                return "double";
            } else {
                return "string: " + val;
            }
        };

        auto result1 = fxt::mvisit(visitor, opt1);
        auto result2 = fxt::mvisit(visitor, opt2);
        auto result3 = fxt::mvisit(visitor, opt3);

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == "int: 42");

        REQUIRE(result2.has_value());
        REQUIRE(*result2 == "double");

        REQUIRE(result3.has_value());
        REQUIRE(*result3 == "string: test");
    }

    SECTION("Empty optional returns empty optional")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};

        auto result = fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, opt);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("fxt::mvisit with fxt::optional and pipe operator", "[mvisit][optional][variant][pipe]")
{
    SECTION("Visit optional using pipe operator")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};

        auto result = opt | fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE(result.has_value());
        REQUIRE(result->substr(0, 4) == "3.14");
    }

    SECTION("Empty optional with pipe operator returns empty")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};

        auto result = opt | fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain mvisit with transform")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{std::string{"hello"}}};

        auto result = opt
                    | fxt::mvisit([](auto&& val) -> int {
                          using T = std::decay_t<decltype(val)>;
                          if constexpr (std::is_arithmetic_v<T>) {
                              return static_cast<int>(val);
                          } else {
                              return static_cast<int>(val.length());
                          }
                      })
                    | fxt::transform([](int len) { return len * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);  // "hello".length() * 2 = 5 * 2
    }
}

TEST_CASE("fxt::mvisit with fxt::expected<fxt::variant, E>", "[mvisit][expected][variant]")
{
    SECTION("Visit expected containing variant - direct call")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{42}};

        auto result = fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, exp);

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("Visit expected with different variant alternatives")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp1{fxt::variant<int, double, std::string>{42}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp2{fxt::variant<int, double, std::string>{3.14}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp3{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto visitor = [](auto&& val) -> int {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return static_cast<int>(val);
            } else {
                return static_cast<int>(val.length());
            }
        };

        auto result1 = fxt::mvisit(visitor, exp1);
        auto result2 = fxt::mvisit(visitor, exp2);
        auto result3 = fxt::mvisit(visitor, exp3);

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 42);

        REQUIRE(result2.has_value());
        REQUIRE(*result2 == 3);

        REQUIRE(result3.has_value());
        REQUIRE(*result3 == 4);
    }

    SECTION("Expected with error returns expected with error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("error occurred")};

        auto result = fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        }, exp);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error occurred");
    }
}

TEST_CASE("fxt::mvisit with fxt::expected and pipe operator", "[mvisit][expected][variant][pipe]")
{
    SECTION("Visit expected using pipe operator")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{3.14}};

        auto result = exp | fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE(result.has_value());
        REQUIRE(result->substr(0, 4) == "3.14");
    }

    SECTION("Expected with error using pipe operator preserves error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("error")};

        auto result = exp | fxt::mvisit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("Chain mvisit with transform")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto result = exp
                    | fxt::mvisit([](auto&& val) -> int {
                          using T = std::decay_t<decltype(val)>;
                          if constexpr (std::is_arithmetic_v<T>) {
                              return static_cast<int>(val);
                          } else {
                              return static_cast<int>(val.length());
                          }
                      })
                    | fxt::transform([](int len) { return "length: " + std::to_string(len); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "length: 4");
    }

    SECTION("Chain mvisit with transform - error propagates")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("initial error")};

        auto result = exp
                    | fxt::mvisit([](auto&& val) -> int {
                          using T = std::decay_t<decltype(val)>;
                          if constexpr (std::is_arithmetic_v<T>) {
                              return static_cast<int>(val);
                          } else {
                              return static_cast<int>(val.length());
                          }
                      })
                    | fxt::transform([](int len) { return "length: " + std::to_string(len); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("fxt::visit with binary variant", "[visit][variant]")
{
    SECTION("Binary variant - first alternative")
    {
        fxt::variant<int, double> v{42};

        auto result = fxt::visit([](auto&& val) -> double {
            return static_cast<double>(val);
        }, v);

        REQUIRE(result == 42.0);
    }

    SECTION("Binary variant - second alternative")
    {
        fxt::variant<int, double> v{3.14};

        auto result = v | fxt::visit([](auto&& val) -> double {
            return static_cast<double>(val);
        });

        REQUIRE(result == 3.14);
    }
}

TEST_CASE("fxt::visit complex chaining scenarios", "[visit][variant][pipe]")
{
    SECTION("Visit variant and chain transformations")
    {
        fxt::variant<int, double, std::string> v{42};

        // This doesn't chain directly since visit returns the result, not a monad
        auto result = fxt::visit([](auto&& val) -> int {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return static_cast<int>(val);
            } else {
                return static_cast<int>(val.length());
            }
        }, v);

        REQUIRE(result == 42);
    }

    SECTION("Visit in optional chain")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};

        auto result = opt
                    | fxt::mvisit([](auto&& val) -> double {
                          using T = std::decay_t<decltype(val)>;
                          if constexpr (std::is_arithmetic_v<T>) {
                              return static_cast<double>(val);
                          } else {
                              return static_cast<double>(val.length());
                          }
                      })
                    | fxt::transform([](double d) { return d * 2; })
                    | fxt::transform([](double d) { return static_cast<int>(d); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }
}

TEST_CASE("fxt::visit type deduction", "[visit][variant]")
{
    SECTION("Visit returns visitor's return type")
    {
        fxt::variant<int, double, std::string> v{42};

        auto result = fxt::visit([](auto&& val) -> std::string {
            return "test";
        }, v);

        STATIC_REQUIRE(std::is_same_v<decltype(result), std::string>);
    }

    SECTION("Pipe operator visit returns visitor's return type")
    {
        fxt::variant<int, double, std::string> v{42};

        auto result = v | fxt::visit([](auto&& val) -> int {
            return 42;
        });

        STATIC_REQUIRE(std::is_same_v<decltype(result), int>);
    }
}

TEST_CASE("fxt::mvisit type deduction", "[mvisit][variant]")
{
    SECTION("mvisit with optional returns optional<R> where R is visitor return type")
    {
        fxt::optional<fxt::variant<int, double>> opt{fxt::variant<int, double>{42}};

        auto result = fxt::mvisit([](auto&& val) -> std::string {
            return "test";
        }, opt);

        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::optional<std::string>>);
    }

    SECTION("mvisit with expected returns expected<R, E> where R is visitor return type")
    {
        fxt::expected<fxt::variant<int, double>, std::string> exp{fxt::variant<int, double>{42}};

        auto result = fxt::mvisit([](auto&& val) -> int {
            return 42;
        }, exp);

        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::expected<int, std::string>>);
    }
}

TEST_CASE("fxt::visit with modifying visitor", "[visit][variant]")
{
    SECTION("Visitor can modify variant value")
    {
        fxt::variant<int, double, std::string> v{42};

        fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, int>) {
                val = 100;
            }
        }, v);

        REQUIRE(std::get<int>(v) == 100);
    }

    SECTION("Visitor can modify string in variant")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};

        v | fxt::visit([](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                val += " world";
            }
        });

        REQUIRE(std::get<std::string>(v) == "hello world");
    }
}

