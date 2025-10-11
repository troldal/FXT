//
// Created by kenne on 11-10-2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

TEST_CASE("transform_tuple with fxt::tuple - direct call", "[transform_tuple]") {
    SECTION("transform with same type") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, t);

        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 6);
    }

    SECTION("transform with different return type") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::transform_tuple([](auto x) { return std::to_string(x); }, t);

        REQUIRE(fxt::get<0>(result) == "1");
        REQUIRE(fxt::get<1>(result) == "2");
        REQUIRE(fxt::get<2>(result) == "3");
    }

    SECTION("transform with mixed types") {
        auto t = fxt::make_tuple(1, 2.5, 3);
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, t);

        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 5.0);
        REQUIRE(fxt::get<2>(result) == 6);
    }

    SECTION("transform single element tuple") {
        auto t = fxt::make_tuple(42);
        auto result = fxt::transform_tuple([](auto x) { return x + 10; }, t);

        REQUIRE(fxt::get<0>(result) == 52);
    }

    SECTION("transform empty tuple") {
        auto t = fxt::make_tuple();
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("transform_tuple with fxt::tuple - pipe operator", "[transform_tuple]") {
    SECTION("transform with same type") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = t | fxt::transform_tuple([](auto x) { return x * 2; });

        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 6);
    }

    SECTION("transform with different return type") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto result = t | fxt::transform_tuple([](auto x) { return x / 10.0; });

        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("chain with other operations") {
        auto t = fxt::make_tuple(1, 2, 3, 4);
        auto result = t
            | fxt::transform_tuple([](auto x) { return x * 2; })
            | fxt::take<2>();

        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 4);
    }
}

TEST_CASE("transform_tuple with fxt::flat_tuple - direct call", "[transform_tuple]") {
    SECTION("transform with same type") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto result = fxt::transform_tuple([](auto x) { return x * 2.0; }, ft);

        REQUIRE(fxt::get<0>(result) == 2.0);
        REQUIRE(fxt::get<1>(result) == 4.0);
        REQUIRE(fxt::get<2>(result) == 6.0);
    }

    SECTION("transform with different return type") {
        auto ft = fxt::make_flat_tuple(1, 2, 3);
        auto result = fxt::transform_tuple([](auto x) { return x * 2.5; }, ft);

        REQUIRE(fxt::get<0>(result) == 2.5);
        REQUIRE(fxt::get<1>(result) == 5.0);
        REQUIRE(fxt::get<2>(result) == 7.5);
    }

    SECTION("transform single element flat_tuple") {
        auto ft = fxt::make_flat_tuple(100);
        auto result = fxt::transform_tuple([](auto x) { return x / 2; }, ft);

        REQUIRE(fxt::get<0>(result) == 50);
    }

    SECTION("transform empty flat_tuple") {
        auto ft = fxt::flat_tuple<>();
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("transform_tuple with fxt::flat_tuple - pipe operator", "[transform_tuple]") {
    SECTION("transform with same type") {
        auto ft = fxt::make_flat_tuple(5, 10, 15);
        auto result = ft | fxt::transform_tuple([](auto x) { return x + 1; });

        REQUIRE(fxt::get<0>(result) == 6);
        REQUIRE(fxt::get<1>(result) == 11);
        REQUIRE(fxt::get<2>(result) == 16);
    }

    SECTION("transform with type conversion") {
        auto ft = fxt::make_flat_tuple(1, 2, 3);
        auto result = ft | fxt::transform_tuple([](auto x) { return static_cast<double>(x) * 1.5; });

        REQUIRE(fxt::get<0>(result) == 1.5);
        REQUIRE(fxt::get<1>(result) == 3.0);
        REQUIRE(fxt::get<2>(result) == 4.5);
    }
}

TEST_CASE("mtransform_tuple with fxt::expected<fxt::tuple> - direct call", "[mtransform_tuple]") {
    struct Error { int code; };

    SECTION("transform with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 6);
    }

    SECTION("transform with error propagates error") {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{42}}};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 42);
    }

    SECTION("transform with different return type") {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(10, 20)};
        auto result = fxt::mtransform_tuple([](auto x) { return x / 10.0; }, exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1.0);
        REQUIRE(fxt::get<1>(result.value()) == 2.0);
    }
}

TEST_CASE("mtransform_tuple with fxt::expected<fxt::tuple> - pipe operator", "[mtransform_tuple]") {
    struct Error { int code; };

    SECTION("transform with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
        auto result = exp | fxt::mtransform_tuple([](auto x) { return x * 3; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 6);
        REQUIRE(fxt::get<2>(result.value()) == 9);
    }

    SECTION("transform with error propagates error") {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{99}}};
        auto result = exp | fxt::mtransform_tuple([](auto x) { return x * 3; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 99);
    }

    SECTION("chain with other monadic operations") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
        auto result = exp
            | fxt::mtransform_tuple([](auto x) { return x * 2; })
            | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 6);
    }
}

TEST_CASE("mtransform_tuple with fxt::expected<fxt::flat_tuple> - direct call", "[mtransform_tuple]") {
    struct Error { int code; };

    SECTION("transform with value") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double>, Error>{fxt::make_flat_tuple(1.0, 2.0)};
        auto result = fxt::mtransform_tuple([](auto x) { return x + 1.0; }, exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2.0);
        REQUIRE(fxt::get<1>(result.value()) == 3.0);
    }

    SECTION("transform with error propagates error") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, Error>{fxt::unexpected{Error{123}}};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 123);
    }
}

TEST_CASE("mtransform_tuple with fxt::expected<fxt::flat_tuple> - pipe operator", "[mtransform_tuple]") {
    struct Error { int code; };

    SECTION("transform with value") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int>, Error>{fxt::make_flat_tuple(5, 10, 15)};
        auto result = exp | fxt::mtransform_tuple([](auto x) { return x / 5; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 3);
    }

    SECTION("transform with error") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double>, Error>{fxt::unexpected{Error{456}}};
        auto result = exp | fxt::mtransform_tuple([](auto x) { return x * 2.0; });

        REQUIRE(!result.has_value());
        REQUIRE(result.error().code == 456);
    }
}

TEST_CASE("mtransform_tuple with fxt::optional<fxt::tuple> - direct call", "[mtransform_tuple]") {
    SECTION("transform with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int>>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 6);
    }

    SECTION("transform with nullopt propagates nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int>>{};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, opt);

        REQUIRE(!result.has_value());
    }

    SECTION("transform with different return type") {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(5, 10)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 0.5; }, opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2.5);
        REQUIRE(fxt::get<1>(result.value()) == 5.0);
    }
}

TEST_CASE("mtransform_tuple with fxt::optional<fxt::tuple> - pipe operator", "[mtransform_tuple]") {
    SECTION("transform with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int>>{fxt::make_tuple(10, 20, 30)};
        auto result = opt | fxt::mtransform_tuple([](auto x) { return x / 10; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 3);
    }

    SECTION("transform with nullopt propagates nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int>>{};
        auto result = opt | fxt::mtransform_tuple([](auto x) { return x * 2; });

        REQUIRE(!result.has_value());
    }

    SECTION("chain with other monadic operations") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{fxt::make_tuple(2, 4, 6, 8)};
        auto result = opt
            | fxt::mtransform_tuple([](auto x) { return x / 2; })
            | fxt::mselect<1, 3>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 4);
    }
}

TEST_CASE("mtransform_tuple with fxt::optional<fxt::flat_tuple> - direct call", "[mtransform_tuple]") {
    SECTION("transform with value") {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
        auto result = fxt::mtransform_tuple([](auto x) { return x + 5.0; }, opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 6.0);
        REQUIRE(fxt::get<1>(result.value()) == 7.0);
    }

    SECTION("transform with nullopt propagates nullopt") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int>>{};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, opt);

        REQUIRE(!result.has_value());
    }
}

TEST_CASE("mtransform_tuple with fxt::optional<fxt::flat_tuple> - pipe operator", "[mtransform_tuple]") {
    SECTION("transform with value") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int, int>>{fxt::make_flat_tuple(3, 6, 9)};
        auto result = opt | fxt::mtransform_tuple([](auto x) { return x / 3; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 3);
    }

    SECTION("transform with nullopt") {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{};
        auto result = opt | fxt::mtransform_tuple([](auto x) { return x * 2.0; });

        REQUIRE(!result.has_value());
    }
}

TEST_CASE("transform_tuple preserves tuple type", "[transform_tuple]") {
    SECTION("fxt::tuple remains fxt::tuple") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, t);

        static_assert(fxt::impl::is_fxt_tuple_v<decltype(result)>, "Result should be fxt::tuple");
    }

    SECTION("fxt::flat_tuple remains fxt::flat_tuple") {
        auto ft = fxt::make_flat_tuple(1, 2, 3);
        auto result = fxt::transform_tuple([](auto x) { return x * 2; }, ft);

        static_assert(fxt::impl::is_flat_tuple_v<decltype(result)>, "Result should be fxt::flat_tuple");
    }
}

TEST_CASE("mtransform_tuple preserves monad and tuple types", "[mtransform_tuple]") {
    struct Error { int code; };

    SECTION("expected<tuple> remains expected<tuple>") {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);

        static_assert(fxt::expected_like<decltype(result)>, "Result should be fxt::expected");
    }

    SECTION("expected<flat_tuple> remains expected<flat_tuple>") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, Error>{fxt::make_flat_tuple(1, 2)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);

        static_assert(fxt::expected_like<decltype(result)>, "Result should be fxt::expected");
    }

    SECTION("optional<tuple> remains optional<tuple>") {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, opt);

        static_assert(fxt::optional_like<decltype(result)>, "Result should be fxt::optional");
    }

    SECTION("optional<flat_tuple> remains optional<flat_tuple>") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
        auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, opt);

        static_assert(fxt::optional_like<decltype(result)>, "Result should be fxt::optional");
    }
}

