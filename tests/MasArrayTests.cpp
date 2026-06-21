/*
    MasArrayTests - Unit tests for fxt::mtuple_as_array
*/

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>

TEST_CASE("mas_array - Optional with non-monadic tuple elements", "[mas_array][optional]")
{
    SECTION("Convert optional<tuple<int, int, int>> to optional<array<double, 3>>")
    {
        std::optional<fxt::tuple<int, int, int>> opt = fxt::make_tuple(1, 2, 3);
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0] == 1.0);
        REQUIRE((*result)[1] == 2.0);
        REQUIRE((*result)[2] == 3.0);
    }

    SECTION("Convert optional<tuple<mixed types>> with type conversion")
    {
        std::optional<fxt::tuple<int, double, float>> opt = fxt::make_tuple(1, 2.5, 3.7f);
        auto result = opt | fxt::mtuple_as_array<int>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0] == 1);
        REQUIRE((*result)[1] == 2);
        REQUIRE((*result)[2] == 3);
    }

    SECTION("Handle empty optional")
    {
        std::optional<fxt::tuple<int, int, int>> opt;
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mas_array - Expected with non-monadic tuple elements", "[mas_array][expected]")
{
    SECTION("Convert expected<tuple> to expected<array>")
    {
        fxt::expected<fxt::tuple<int, int, int>, std::string> exp = fxt::make_tuple(10, 20, 30);
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0] == 10.0);
        REQUIRE((*result)[1] == 20.0);
        REQUIRE((*result)[2] == 30.0);
    }

    SECTION("Handle expected with error")
    {
        fxt::expected<fxt::tuple<int, int, int>, std::string> exp =
            std::unexpected("error");
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }
}

TEST_CASE("mas_array - Optional with monadic tuple elements", "[mas_array][optional][monadic]")
{
    using OptInt = std::optional<int>;

    SECTION("Convert optional<tuple<optional...>> to optional<array<optional...>>")
    {
        std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt =
            fxt::make_tuple(OptInt{1}, OptInt{2}, OptInt{3});
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0].has_value());
        REQUIRE(*(*result)[0] == 1.0);
        REQUIRE((*result)[1].has_value());
        REQUIRE(*(*result)[1] == 2.0);
        REQUIRE((*result)[2].has_value());
        REQUIRE(*(*result)[2] == 3.0);
    }

    SECTION("Handle optional elements with nullopt")
    {
        std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt =
            fxt::make_tuple(OptInt{1}, OptInt{}, OptInt{3});
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0].has_value());
        REQUIRE_FALSE((*result)[1].has_value());
        REQUIRE((*result)[2].has_value());
    }

    SECTION("Handle outer optional being empty")
    {
        std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt;
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mas_array - Expected with monadic tuple elements", "[mas_array][expected][monadic]")
{
    using ExpInt = fxt::expected<int, std::string>;

    SECTION("Convert expected<tuple<expected...>> to expected<array<expected...>>")
    {
        fxt::expected<fxt::tuple<ExpInt, ExpInt, ExpInt>, std::string> exp =
            fxt::make_tuple(ExpInt{100}, ExpInt{200}, ExpInt{300});
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0].has_value());
        REQUIRE(*(*result)[0] == 100.0);
        REQUIRE((*result)[1].has_value());
        REQUIRE(*(*result)[1] == 200.0);
        REQUIRE((*result)[2].has_value());
        REQUIRE(*(*result)[2] == 300.0);
    }

    SECTION("Handle expected elements with errors")
    {
        fxt::expected<fxt::tuple<ExpInt, ExpInt, ExpInt>, std::string> exp =
            fxt::make_tuple(ExpInt{100}, std::unexpected<std::string>("err"), ExpInt{300});
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0].has_value());
        REQUIRE_FALSE((*result)[1].has_value());
        REQUIRE((*result)[2].has_value());
    }

    SECTION("Handle outer expected with error")
    {
        fxt::expected<fxt::tuple<ExpInt, ExpInt, ExpInt>, std::string> exp =
            std::unexpected("outer error");
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "outer error");
    }
}

TEST_CASE("mas_array - flat_tuple support", "[mas_array][flat_tuple]")
{
    SECTION("Convert optional<flat_tuple> to optional<array>")
    {
        std::optional<fxt::flat_tuple<double, double, double>> opt =
            fxt::make_flat_tuple(1.1, 2.2, 3.3);
        auto result = opt | fxt::mtuple_as_array<int>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0] == 1);
        REQUIRE((*result)[1] == 2);
        REQUIRE((*result)[2] == 3);
    }

    SECTION("Convert expected<flat_tuple> to expected<array>")
    {
        fxt::expected<fxt::flat_tuple<int, int, int>, std::string> exp =
            fxt::make_flat_tuple(5, 10, 15);
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 3);
        REQUIRE((*result)[0] == 5.0);
        REQUIRE((*result)[1] == 10.0);
        REQUIRE((*result)[2] == 15.0);
    }
}

TEST_CASE("mas_array - Type conversions", "[mas_array][conversion]")
{
    SECTION("Convert to larger type")
    {
        std::optional<fxt::tuple<int, int, int>> opt = fxt::make_tuple(1, 2, 3);
        auto result = opt | fxt::mtuple_as_array<long long>();

        REQUIRE(result.has_value());
        REQUIRE((*result)[0] == 1LL);
        REQUIRE((*result)[1] == 2LL);
        REQUIRE((*result)[2] == 3LL);
    }

    SECTION("Convert floating point to integral with truncation")
    {
        fxt::expected<fxt::tuple<double, double, double>, int> exp =
            fxt::make_tuple(1.7, 2.3, 3.9);
        auto result = exp | fxt::mtuple_as_array<int>();

        REQUIRE(result.has_value());
        REQUIRE((*result)[0] == 1);
        REQUIRE((*result)[1] == 2);
        REQUIRE((*result)[2] == 3);
    }
}

TEST_CASE("mas_array - Chaining operations", "[mas_array][chain]")
{
    SECTION("Chain with transform")
    {
        auto result = std::optional{fxt::make_tuple(1, 2, 3)}
            | fxt::mtuple_as_array<double>()
            | fxt::transform([](const auto& arr) {
                double sum = 0;
                for (const auto& val : arr) {
                    sum += val;
                }
                return sum;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 6.0);
    }

    SECTION("Chain multiple operations")
    {
        auto result = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::make_tuple(2, 4, 6, 8)}
            | fxt::mtuple_as_array<double>()
            | fxt::transform([](const auto& arr) {
                return arr.size();
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 4);
    }
}


TEST_CASE("mas_array - Single element", "[mas_array][single]")
{
    SECTION("Convert optional with single element tuple")
    {
        std::optional<fxt::tuple<int>> opt = fxt::make_tuple(42);
        auto result = opt | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 1);
        REQUIRE((*result)[0] == 42.0);
    }

    SECTION("Convert expected with single monadic element")
    {
        using OptInt = std::optional<int>;
        fxt::expected<fxt::tuple<OptInt>, std::string> exp =
            fxt::make_tuple(OptInt{99});
        auto result = exp | fxt::mtuple_as_array<double>();

        REQUIRE(result.has_value());
        REQUIRE((*result).size() == 1);
        REQUIRE((*result)[0].has_value());
        REQUIRE(*(*result)[0] == 99.0);
    }
}

