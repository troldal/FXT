//
// Created by kenne on 12-10-2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

// Error type for expected tests
struct TestError {
    std::string message;
};

// ============================================================================
// fxt::tuple_cat with fxt::tuple
// ============================================================================

TEST_CASE("tuple_cat with fxt::tuple - direct call", "[tuple_cat]") {
    SECTION("concatenate two tuples") {
        auto t1 = fxt::make_tuple(1, 2);
        auto t2 = fxt::make_tuple(3.0, 4.0);
        auto result = fxt::tuple_cat(t1, t2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3.0);
        REQUIRE(fxt::get<3>(result) == 4.0);
    }

    SECTION("concatenate with different types") {
        auto t1 = fxt::make_tuple(1, 2, 3);
        auto t2 = fxt::make_tuple(4.5, std::string("hello"));
        auto result = fxt::tuple_cat(t1, t2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4.5);
        REQUIRE(fxt::get<4>(result) == "hello");
    }

    SECTION("concatenate with empty tuple") {
        auto t1 = fxt::make_tuple(1, 2);
        auto t2 = fxt::make_tuple();
        auto result = fxt::tuple_cat(t1, t2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
    }

    SECTION("concatenate single element tuples") {
        auto t1 = fxt::make_tuple(42);
        auto t2 = fxt::make_tuple(99);
        auto result = fxt::tuple_cat(t1, t2);

        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 99);
    }
}

TEST_CASE("tuple_cat with fxt::tuple - pipe operator", "[tuple_cat]") {
    SECTION("pipe two tuples") {
        auto t1 = fxt::make_tuple(10, 20);
        auto t2 = fxt::make_tuple(30.5, 40.5);
        auto result = t1 | fxt::tuple_cat(t2);

        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30.5);
        REQUIRE(fxt::get<3>(result) == 40.5);
    }

    SECTION("pipe with mixed types") {
        auto t1 = fxt::make_tuple(1, 2.5);
        auto t2 = fxt::make_tuple(std::string("test"), 3);
        auto result = t1 | fxt::tuple_cat(t2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == "test");
        REQUIRE(fxt::get<3>(result) == 3);
    }
}

// ============================================================================
// fxt::tuple_cat with fxt::flat_tuple
// ============================================================================

TEST_CASE("tuple_cat with fxt::flat_tuple - direct call", "[tuple_cat]") {
    SECTION("concatenate two flat_tuples") {
        auto ft1 = fxt::make_flat_tuple(100, 200);
        auto ft2 = fxt::make_flat_tuple(3.14, 2.71);
        auto result = fxt::tuple_cat(ft1, ft2);

        REQUIRE(fxt::get<0>(result) == 100);
        REQUIRE(fxt::get<1>(result) == 200);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == 2.71);
    }

    SECTION("concatenate with different types") {
        auto ft1 = fxt::make_flat_tuple(1.1, 2.2);
        auto ft2 = fxt::make_flat_tuple(3, 4, 5);
        auto result = fxt::tuple_cat(ft1, ft2);

        REQUIRE(fxt::get<0>(result) == 1.1);
        REQUIRE(fxt::get<1>(result) == 2.2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }

    SECTION("concatenate with empty flat_tuple") {
        auto ft1 = fxt::make_flat_tuple(1, 2);
        auto ft2 = fxt::make_flat_tuple();
        auto result = fxt::tuple_cat(ft1, ft2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
    }
}

TEST_CASE("tuple_cat with fxt::flat_tuple - pipe operator", "[tuple_cat]") {
    SECTION("pipe two flat_tuples") {
        auto ft1 = fxt::make_flat_tuple(1.5, 2.5);
        auto ft2 = fxt::make_flat_tuple(3.5, 4.5);
        auto result = ft1 | fxt::tuple_cat(ft2);

        REQUIRE(fxt::get<0>(result) == 1.5);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == 3.5);
        REQUIRE(fxt::get<3>(result) == 4.5);
    }

    SECTION("pipe with mixed types") {
        auto ft1 = fxt::make_flat_tuple(1, 2);
        auto ft2 = fxt::make_flat_tuple(3.14, 2.71, 1.41);
        auto result = ft1 | fxt::tuple_cat(ft2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == 2.71);
        REQUIRE(fxt::get<4>(result) == 1.41);
    }
}

// ============================================================================
// fxt::mtuple_cat with fxt::expected<fxt::tuple>
// ============================================================================

TEST_CASE("mtuple_cat with fxt::expected<fxt::tuple> and plain tuple - direct call", "[mtuple_cat]") {
    SECTION("concatenate expected<tuple> with tuple") {
        auto exp = fxt::expected<fxt::tuple<int, int>, TestError>{fxt::make_tuple(1, 2)};
        auto t = fxt::make_tuple(3.0, 4.0);
        auto result = fxt::mtuple_cat(exp, t);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1);
        REQUIRE(fxt::get<1>(val) == 2);
        REQUIRE(fxt::get<2>(val) == 3.0);
        REQUIRE(fxt::get<3>(val) == 4.0);
    }

    SECTION("error propagation") {
        auto exp_err = fxt::expected<fxt::tuple<int, int>, TestError>{
            fxt::unexpected{TestError{"error"}}
        };
        auto t = fxt::make_tuple(3.0, 4.0);
        auto result = fxt::mtuple_cat(exp_err, t);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message == "error");
    }
}

TEST_CASE("mtuple_cat with fxt::expected<fxt::tuple> and plain tuple - pipe operator", "[mtuple_cat]") {
    SECTION("pipe expected<tuple> with tuple") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, TestError>{fxt::make_tuple(10, 20, 30)};
        auto t = fxt::make_tuple(40.5, 50.5);
        auto result = exp | fxt::mtuple_cat(t);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 10);
        REQUIRE(fxt::get<1>(val) == 20);
        REQUIRE(fxt::get<2>(val) == 30);
        REQUIRE(fxt::get<3>(val) == 40.5);
        REQUIRE(fxt::get<4>(val) == 50.5);
    }

    SECTION("pipe with error propagation") {
        auto exp_err = fxt::expected<fxt::tuple<int, int>, TestError>{
            fxt::unexpected{TestError{"error"}}
        };
        auto t = fxt::make_tuple(1.0, 2.0);
        auto result = exp_err | fxt::mtuple_cat(t);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message == "error");
    }
}

TEST_CASE("mtuple_cat with two fxt::expected<fxt::tuple> - direct call", "[mtuple_cat]") {
    SECTION("concatenate two expected<tuple>") {
        auto exp1 = fxt::expected<fxt::tuple<int, int>, TestError>{fxt::make_tuple(100, 200)};
        auto exp2 = fxt::expected<fxt::tuple<double, double>, TestError>{fxt::make_tuple(3.14, 2.71)};
        auto result = fxt::mtuple_cat(exp1, exp2);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 100);
        REQUIRE(fxt::get<1>(val) == 200);
        REQUIRE(fxt::get<2>(val) == 3.14);
        REQUIRE(fxt::get<3>(val) == 2.71);
    }

    SECTION("first expected has error") {
        auto exp1_err = fxt::expected<fxt::tuple<int, int>, TestError>{
            fxt::unexpected{TestError{"error1"}}
        };
        auto exp2 = fxt::expected<fxt::tuple<double, double>, TestError>{fxt::make_tuple(3.14, 2.71)};
        auto result = fxt::mtuple_cat(exp1_err, exp2);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message == "error1");
    }

    SECTION("second expected has error") {
        auto exp1 = fxt::expected<fxt::tuple<int, int>, TestError>{fxt::make_tuple(100, 200)};
        auto exp2_err = fxt::expected<fxt::tuple<double, double>, TestError>{
            fxt::unexpected{TestError{"error2"}}
        };
        auto result = fxt::mtuple_cat(exp1, exp2_err);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().message == "error2");
    }
}

TEST_CASE("mtuple_cat with two fxt::expected<fxt::tuple> - pipe operator", "[mtuple_cat]") {
    SECTION("pipe two expected<tuple>") {
        auto exp1 = fxt::expected<fxt::tuple<int, int>, TestError>{fxt::make_tuple(5, 6)};
        auto exp2 = fxt::expected<fxt::tuple<int, int, int>, TestError>{fxt::make_tuple(7, 8, 9)};
        auto result = exp1 | fxt::mtuple_cat(exp2);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 5);
        REQUIRE(fxt::get<1>(val) == 6);
        REQUIRE(fxt::get<2>(val) == 7);
        REQUIRE(fxt::get<3>(val) == 8);
        REQUIRE(fxt::get<4>(val) == 9);
    }
}

// ============================================================================
// fxt::mtuple_cat with fxt::optional<fxt::tuple>
// ============================================================================

TEST_CASE("mtuple_cat with fxt::optional<fxt::tuple> and plain tuple - direct call", "[mtuple_cat]") {
    SECTION("concatenate optional<tuple> with tuple") {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto t = fxt::make_tuple(3.0, 4.0, 5.0);
        auto result = fxt::mtuple_cat(opt, t);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1);
        REQUIRE(fxt::get<1>(val) == 2);
        REQUIRE(fxt::get<2>(val) == 3.0);
        REQUIRE(fxt::get<3>(val) == 4.0);
        REQUIRE(fxt::get<4>(val) == 5.0);
    }

    SECTION("empty optional propagation") {
        auto opt_empty = fxt::optional<fxt::tuple<int, int>>{};
        auto t = fxt::make_tuple(3.0, 4.0);
        auto result = fxt::mtuple_cat(opt_empty, t);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mtuple_cat with fxt::optional<fxt::tuple> and plain tuple - pipe operator", "[mtuple_cat]") {
    SECTION("pipe optional<tuple> with tuple") {
        auto opt = fxt::optional<fxt::tuple<double, double>>{fxt::make_tuple(1.1, 2.2)};
        auto t = fxt::make_tuple(3, 4);
        auto result = opt | fxt::mtuple_cat(t);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1.1);
        REQUIRE(fxt::get<1>(val) == 2.2);
        REQUIRE(fxt::get<2>(val) == 3);
        REQUIRE(fxt::get<3>(val) == 4);
    }

    SECTION("pipe with empty optional") {
        auto opt_empty = fxt::optional<fxt::tuple<int, int>>{};
        auto t = fxt::make_tuple(1.0, 2.0);
        auto result = opt_empty | fxt::mtuple_cat(t);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mtuple_cat with two fxt::optional<fxt::tuple> - pipe operator", "[mtuple_cat]") {
    SECTION("pipe two optional<tuple>") {
        auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(10, 20)};
        auto opt2 = fxt::optional<fxt::tuple<double, double>>{fxt::make_tuple(30.5, 40.5)};
        auto result = opt1 | fxt::mtuple_cat(opt2);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 10);
        REQUIRE(fxt::get<1>(val) == 20);
        REQUIRE(fxt::get<2>(val) == 30.5);
        REQUIRE(fxt::get<3>(val) == 40.5);
    }

    SECTION("first optional is empty") {
        auto opt1_empty = fxt::optional<fxt::tuple<int, int>>{};
        auto opt2 = fxt::optional<fxt::tuple<double, double>>{fxt::make_tuple(30.5, 40.5)};
        auto result = opt1_empty | fxt::mtuple_cat(opt2);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("second optional is empty") {
        auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(10, 20)};
        auto opt2_empty = fxt::optional<fxt::tuple<double, double>>{};
        auto result = opt1 | fxt::mtuple_cat(opt2_empty);

        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// fxt::mtuple_cat with fxt::expected<fxt::flat_tuple>
// ============================================================================

TEST_CASE("mtuple_cat with fxt::expected<fxt::flat_tuple> - direct call", "[mtuple_cat]") {
    SECTION("concatenate expected<flat_tuple> with flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, TestError>{fxt::make_flat_tuple(1, 2)};
        auto ft = fxt::make_flat_tuple(3.0, 4.0);
        auto result = fxt::mtuple_cat(exp, ft);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1);
        REQUIRE(fxt::get<1>(val) == 2);
        REQUIRE(fxt::get<2>(val) == 3.0);
        REQUIRE(fxt::get<3>(val) == 4.0);
    }
}

TEST_CASE("mtuple_cat with fxt::expected<fxt::flat_tuple> - pipe operator", "[mtuple_cat]") {
    SECTION("pipe expected<flat_tuple> with flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, TestError>{fxt::make_flat_tuple(1, 2)};
        auto ft = fxt::make_flat_tuple(3.0, 4.0);
        auto result = exp | fxt::mtuple_cat(ft);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1);
        REQUIRE(fxt::get<1>(val) == 2);
        REQUIRE(fxt::get<2>(val) == 3.0);
        REQUIRE(fxt::get<3>(val) == 4.0);
    }

    SECTION("pipe two expected<flat_tuple>") {
        auto exp1 = fxt::expected<fxt::flat_tuple<int, int>, TestError>{fxt::make_flat_tuple(100, 200)};
        auto exp2 = fxt::expected<fxt::flat_tuple<int, int, int>, TestError>{fxt::make_flat_tuple(300, 400, 500)};
        auto result = exp1 | fxt::mtuple_cat(exp2);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 100);
        REQUIRE(fxt::get<1>(val) == 200);
        REQUIRE(fxt::get<2>(val) == 300);
        REQUIRE(fxt::get<3>(val) == 400);
        REQUIRE(fxt::get<4>(val) == 500);
    }
}

// ============================================================================
// fxt::mtuple_cat with fxt::optional<fxt::flat_tuple>
// ============================================================================

TEST_CASE("mtuple_cat with fxt::optional<fxt::flat_tuple> - pipe operator", "[mtuple_cat]") {
    SECTION("pipe optional<flat_tuple> with flat_tuple") {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.5, 2.5)};
        auto ft = fxt::make_flat_tuple(3.5, 4.5, 5.5);
        auto result = opt | fxt::mtuple_cat(ft);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1.5);
        REQUIRE(fxt::get<1>(val) == 2.5);
        REQUIRE(fxt::get<2>(val) == 3.5);
        REQUIRE(fxt::get<3>(val) == 4.5);
        REQUIRE(fxt::get<4>(val) == 5.5);
    }

    SECTION("pipe two optional<flat_tuple>") {
        auto opt1 = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
        auto opt2 = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(3.0, 4.0)};
        auto result = opt1 | fxt::mtuple_cat(opt2);

        REQUIRE(result.has_value());
        auto val = result.value();
        REQUIRE(fxt::get<0>(val) == 1);
        REQUIRE(fxt::get<1>(val) == 2);
        REQUIRE(fxt::get<2>(val) == 3.0);
        REQUIRE(fxt::get<3>(val) == 4.0);
    }
}

// ============================================================================
// Mixed type tests
// ============================================================================

TEST_CASE("tuple_cat with mixed types", "[tuple_cat]") {
    SECTION("concatenate tuple with strings") {
        auto t1 = fxt::make_tuple(1, std::string("hello"));
        auto t2 = fxt::make_tuple(2.5, std::string("world"));
        auto result = fxt::tuple_cat(t1, t2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == "hello");
        REQUIRE(fxt::get<2>(result) == 2.5);
        REQUIRE(fxt::get<3>(result) == "world");
    }

    SECTION("concatenate flat_tuple with various types") {
        auto ft1 = fxt::make_flat_tuple(1, 2.5);
        auto ft2 = fxt::make_flat_tuple(3, 4.5, 5);
        auto result = ft1 | fxt::tuple_cat(ft2);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4.5);
        REQUIRE(fxt::get<4>(result) == 5);
    }
}

