//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

TEST_CASE("drop with various tuple sizes", "[drop]") {
    SECTION("drop from 1-element tuple") {
        auto t = fxt::make_tuple(42);
        auto result = fxt::tuple_drop<0>(t);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::tuple_drop<1>(t);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop from 2-element tuple") {
        auto t = fxt::make_tuple(10, 20);

        auto drop0 = fxt::tuple_drop<0>(t);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);

        auto drop1 = fxt::tuple_drop<1>(t);
        REQUIRE(fxt::get<0>(drop1) == 20);

        auto drop2 = fxt::tuple_drop<2>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop from 3-element tuple") {
        auto t = fxt::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::tuple_drop<0>(t);
        REQUIRE(fxt::get<0>(drop0) == 1);
        REQUIRE(fxt::get<1>(drop0) == 2.5);
        REQUIRE(fxt::get<2>(drop0) == "three");

        auto drop1 = fxt::tuple_drop<1>(t);
        REQUIRE(fxt::get<0>(drop1) == 2.5);
        REQUIRE(fxt::get<1>(drop1) == "three");

        auto drop2 = fxt::tuple_drop<2>(t);
        REQUIRE(fxt::get<0>(drop2) == "three");

        auto drop3 = fxt::tuple_drop<3>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop from 5-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::tuple_drop<2>(t);
        REQUIRE(fxt::get<0>(drop2) == 3);
        REQUIRE(fxt::get<1>(drop2) == 4);
        REQUIRE(fxt::get<2>(drop2) == 5);

        auto drop4 = fxt::tuple_drop<4>(t);
        REQUIRE(fxt::get<0>(drop4) == 5);
    }

    SECTION("drop from 10-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::tuple_drop<5>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(fxt::get<0>(drop5) == 6);
        REQUIRE(fxt::get<4>(drop5) == 10);

        auto drop9 = fxt::tuple_drop<9>(t);
        REQUIRE(fxt::get<0>(drop9) == 10);
    }
}

TEST_CASE("drop with different types", "[drop]") {
    SECTION("drop with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::tuple_drop<2>(t);
        REQUIRE(fxt::get<0>(result) == "hello");
        REQUIRE(fxt::get<1>(result) == 'c');
        REQUIRE(fxt::get<2>(result) == true);
    }

    SECTION("drop with all same type") {
        auto t = fxt::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::tuple_drop<2>(t);

        REQUIRE(fxt::get<0>(result) == 30);
        REQUIRE(fxt::get<1>(result) == 40);
        REQUIRE(fxt::get<2>(result) == 50);
    }

    SECTION("drop with string types") {
        auto t = fxt::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::tuple_drop<1>(t);

        REQUIRE(fxt::get<0>(result) == "second");
        REQUIRE(fxt::get<1>(result) == "third");
        REQUIRE(fxt::get<2>(result) == "fourth");
    }
}

TEST_CASE("drop with pipe operator on fxt::tuple", "[drop][tuple][pipe]") {
    SECTION("basic pipe with drop") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = t | fxt::tuple_drop<2>();

        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 5);
    }

    SECTION("pipe with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("test"), 'x', true);
        auto result = t | fxt::tuple_drop<2>();

        REQUIRE(fxt::get<0>(result) == "test");
        REQUIRE(fxt::get<1>(result) == 'x');
        REQUIRE(fxt::get<2>(result) == true);
    }

    SECTION("pipe with rvalue tuple") {
        auto result = fxt::make_tuple(100, 200, 300, 400) | fxt::tuple_drop<2>();

        REQUIRE(fxt::get<0>(result) == 300);
        REQUIRE(fxt::get<1>(result) == 400);
    }

    SECTION("chained pipes") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto result = t | fxt::tuple_drop<2>() | fxt::tuple_drop<2>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 6);
        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<5>(result) == 10);
    }
}

TEST_CASE("drop_last with various tuple sizes", "[drop_last]") {
    SECTION("drop_last from 1-element tuple") {
        auto t = fxt::make_tuple(42);
        auto result = fxt::tuple_drop_last<0>(t);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::tuple_drop_last<1>(t);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop_last from 2-element tuple") {
        auto t = fxt::make_tuple(10, 20);

        auto drop0 = fxt::tuple_drop_last<0>(t);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);

        auto drop1 = fxt::tuple_drop_last<1>(t);
        REQUIRE(fxt::get<0>(drop1) == 10);

        auto drop2 = fxt::tuple_drop_last<2>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop_last from 3-element tuple") {
        auto t = fxt::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::tuple_drop_last<0>(t);
        REQUIRE(fxt::get<0>(drop0) == 1);
        REQUIRE(fxt::get<1>(drop0) == 2.5);
        REQUIRE(fxt::get<2>(drop0) == "three");

        auto drop1 = fxt::tuple_drop_last<1>(t);
        REQUIRE(fxt::get<0>(drop1) == 1);
        REQUIRE(fxt::get<1>(drop1) == 2.5);

        auto drop2 = fxt::tuple_drop_last<2>(t);
        REQUIRE(fxt::get<0>(drop2) == 1);

        auto drop3 = fxt::tuple_drop_last<3>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop_last from 5-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::tuple_drop_last<2>(t);
        REQUIRE(fxt::get<0>(drop2) == 1);
        REQUIRE(fxt::get<1>(drop2) == 2);
        REQUIRE(fxt::get<2>(drop2) == 3);

        auto drop4 = fxt::tuple_drop_last<4>(t);
        REQUIRE(fxt::get<0>(drop4) == 1);
    }

    SECTION("drop_last from 10-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::tuple_drop_last<5>(t);
        REQUIRE(fxt::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(fxt::get<0>(drop5) == 1);
        REQUIRE(fxt::get<4>(drop5) == 5);

        auto drop9 = fxt::tuple_drop_last<9>(t);
        REQUIRE(fxt::get<0>(drop9) == 1);
    }
}

TEST_CASE("drop_last with different types", "[drop_last]") {
    SECTION("drop_last with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::tuple_drop_last<2>(t);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 3.14);
        REQUIRE(fxt::get<2>(result) == "hello");
    }

    SECTION("drop_last with all same type") {
        auto t = fxt::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::tuple_drop_last<2>(t);

        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30);
    }

    SECTION("drop_last with string types") {
        auto t = fxt::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::tuple_drop_last<1>(t);

        REQUIRE(fxt::get<0>(result) == "first");
        REQUIRE(fxt::get<1>(result) == "second");
        REQUIRE(fxt::get<2>(result) == "third");
    }
}

TEST_CASE("drop_last with pipe operator on fxt::tuple", "[drop_last][tuple][pipe]") {
    SECTION("basic pipe with drop_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = t | fxt::tuple_drop_last<2>();

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("pipe with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("test"), 'x', true);
        auto result = t | fxt::tuple_drop_last<2>();

        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 3.14);
        REQUIRE(fxt::get<2>(result) == "test");
    }

    SECTION("pipe with rvalue tuple") {
        auto result = fxt::make_tuple(100, 200, 300, 400) | fxt::tuple_drop_last<2>();

        REQUIRE(fxt::get<0>(result) == 100);
        REQUIRE(fxt::get<1>(result) == 200);
    }

    SECTION("chained pipes") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto result = t | fxt::tuple_drop_last<2>() | fxt::tuple_drop_last<2>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 6);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<5>(result) == 6);
    }
}

TEST_CASE("drop and drop_last edge cases", "[drop][drop_last]") {
    SECTION("drop 0 elements returns same tuple") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::tuple_drop<0>(t);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop_last 0 elements returns same tuple") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::tuple_drop_last<0>(t);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop all elements returns empty tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4);
        auto result = fxt::tuple_drop<4>(t);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("drop_last all elements returns empty tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4);
        auto result = fxt::tuple_drop_last<4>(t);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("drop and drop_last with move-only types", "[drop][drop_last]") {
    SECTION("drop with unique_ptr") {
        auto t = fxt::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::tuple_drop<1>(std::move(t));
        REQUIRE(*fxt::get<0>(result) == 100);
        REQUIRE(*fxt::get<1>(result) == 200);
    }

    SECTION("drop_last with unique_ptr") {
        auto t = fxt::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::tuple_drop_last<1>(std::move(t));
        REQUIRE(*fxt::get<0>(result) == 42);
        REQUIRE(*fxt::get<1>(result) == 100);
    }
}

TEST_CASE("drop and drop_last combined", "[drop][drop_last]") {
    SECTION("drop then drop_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_first = fxt::tuple_drop<2>(t);  // {3, 4, 5, 6, 7}
        auto dropped_both = fxt::tuple_drop_last<2>(dropped_first);  // {3, 4, 5}

        REQUIRE(fxt::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(fxt::get<0>(dropped_both) == 3);
        REQUIRE(fxt::get<1>(dropped_both) == 4);
        REQUIRE(fxt::get<2>(dropped_both) == 5);
    }

    SECTION("drop_last then drop") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_last = fxt::tuple_drop_last<2>(t);  // {1, 2, 3, 4, 5}
        auto dropped_both = fxt::tuple_drop<2>(dropped_last);  // {3, 4, 5}

        REQUIRE(fxt::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(fxt::get<0>(dropped_both) == 3);
        REQUIRE(fxt::get<1>(dropped_both) == 4);
        REQUIRE(fxt::get<2>(dropped_both) == 5);
    }

    SECTION("symmetry: drop then drop_last vs drop_last then drop") {
        auto t1 = fxt::make_tuple(10, 20, 30, 40, 50);
        auto t2 = fxt::make_tuple(10, 20, 30, 40, 50);

        auto result1 = fxt::tuple_drop_last<1>(fxt::tuple_drop<1>(t1));
        auto result2 = fxt::tuple_drop<1>(fxt::tuple_drop_last<1>(t2));

        REQUIRE(fxt::get<0>(result1) == fxt::get<0>(result2));
        REQUIRE(fxt::get<1>(result1) == fxt::get<1>(result2));
        REQUIRE(fxt::get<2>(result1) == fxt::get<2>(result2));
    }
}

TEST_CASE("drop and drop_last with large tuples", "[drop][drop_last]") {
    SECTION("drop from 15-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::tuple_drop<10>(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 11);
        REQUIRE(fxt::get<4>(result) == 15);
    }

    SECTION("drop_last from 15-element tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::tuple_drop_last<10>(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<4>(result) == 5);
    }
}

// ===== NEW TESTS: fxt::flat_tuple with drop and drop_last =====

TEST_CASE("drop with fxt::flat_tuple", "[drop][flat_tuple]") {
    SECTION("drop from 1-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(42);
        auto result = fxt::tuple_drop<0>(ft);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::tuple_drop<1>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop from 3-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);

        auto drop0 = fxt::tuple_drop<0>(ft);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);
        REQUIRE(fxt::get<2>(drop0) == 30);

        auto drop1 = fxt::tuple_drop<1>(ft);
        REQUIRE(fxt::get<0>(drop1) == 20);
        REQUIRE(fxt::get<1>(drop1) == 30);

        auto drop2 = fxt::tuple_drop<2>(ft);
        REQUIRE(fxt::get<0>(drop2) == 30);

        auto drop3 = fxt::tuple_drop<3>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop from 5-element flat_tuple with mixed types") {
        auto ft = fxt::make_flat_tuple(1, 2.5, std::string("test"), 'x', true);

        auto result = fxt::tuple_drop<2>(ft);
        REQUIRE(fxt::get<0>(result) == "test");
        REQUIRE(fxt::get<1>(result) == 'x');
        REQUIRE(fxt::get<2>(result) == true);
    }

    SECTION("drop from rvalue flat_tuple") {
        auto result = fxt::tuple_drop<2>(fxt::make_flat_tuple(100, 200, 300, 400));
        REQUIRE(fxt::get<0>(result) == 300);
        REQUIRE(fxt::get<1>(result) == 400);
    }
}

TEST_CASE("drop_last with fxt::flat_tuple", "[drop_last][flat_tuple]") {
    SECTION("drop_last from 1-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(42);
        auto result = fxt::tuple_drop_last<0>(ft);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::tuple_drop_last<1>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop_last from 3-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);

        auto drop0 = fxt::tuple_drop_last<0>(ft);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);
        REQUIRE(fxt::get<2>(drop0) == 30);

        auto drop1 = fxt::tuple_drop_last<1>(ft);
        REQUIRE(fxt::get<0>(drop1) == 10);
        REQUIRE(fxt::get<1>(drop1) == 20);

        auto drop2 = fxt::tuple_drop_last<2>(ft);
        REQUIRE(fxt::get<0>(drop2) == 10);

        auto drop3 = fxt::tuple_drop_last<3>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop_last from 5-element flat_tuple with mixed types") {
        auto ft = fxt::make_flat_tuple(1, 2.5, std::string("test"), 'x', true);

        auto result = fxt::tuple_drop_last<2>(ft);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == "test");
    }

    SECTION("drop_last from rvalue flat_tuple") {
        auto result = fxt::tuple_drop_last<2>(fxt::make_flat_tuple(100, 200, 300, 400));
        REQUIRE(fxt::get<0>(result) == 100);
        REQUIRE(fxt::get<1>(result) == 200);
    }
}

// ===== NEW TESTS: comparing drop behavior with fxt::tuple and fxt::flat_tuple =====

TEST_CASE("comparing drop behavior with fxt::tuple and fxt::flat_tuple", "[drop][tuple][flat_tuple][comparison]") {
    SECTION("drop produces same result for both tuple types") {
        auto t = fxt::make_tuple(5, 10, 15, 20, 25);
        auto ft = fxt::make_flat_tuple(5, 10, 15, 20, 25);

        auto drop_result = fxt::tuple_drop<2>(t);
        auto drop_flat_result = fxt::tuple_drop<2>(ft);

        REQUIRE(std::get<0>(drop_result) == fxt::get<0>(drop_flat_result));
        REQUIRE(std::get<1>(drop_result) == fxt::get<1>(drop_flat_result));
        REQUIRE(std::get<2>(drop_result) == fxt::get<2>(drop_flat_result));
    }

    SECTION("drop_last produces same result for both tuple types") {
        auto t = fxt::make_tuple(5, 10, 15, 20, 25);
        auto ft = fxt::make_flat_tuple(5, 10, 15, 20, 25);

        auto drop_last_result = fxt::tuple_drop_last<2>(t);
        auto drop_last_flat_result = fxt::tuple_drop_last<2>(ft);

        REQUIRE(std::get<0>(drop_last_result) == fxt::get<0>(drop_last_flat_result));
        REQUIRE(std::get<1>(drop_last_result) == fxt::get<1>(drop_last_flat_result));
        REQUIRE(std::get<2>(drop_last_result) == fxt::get<2>(drop_last_flat_result));
    }
}

// ===== TESTS: fxt::mdrop with fxt::expected containing tuples =====

TEST_CASE("mdrop with fxt::expected containing fxt::tuple", "[mdrop][expected][tuple]") {
    SECTION("mdrop from expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::make_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
    }

    SECTION("mdrop from expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::unexpected{"Error"}};
        auto result = fxt::mtuple_drop<2>(exp);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error");
    }

    SECTION("mdrop with pipe operator on expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{fxt::make_tuple(10, 20, 30, 40, 50)};
        auto result = exp | fxt::mtuple_drop<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 30);
        REQUIRE(fxt::get<1>(result.value()) == 40);
        REQUIRE(fxt::get<2>(result.value()) == 50);
    }

    SECTION("mdrop with pipe operator on expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::unexpected{"Error"}};
        auto result = exp | fxt::mtuple_drop<2>();

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error");
    }

    SECTION("mdrop all elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtuple_drop<3>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 0);
    }

    SECTION("mdrop zero elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtuple_drop<0>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 3);
    }

    SECTION("chained mdrop operations on expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int>, std::string>{fxt::make_tuple(1, 2, 3, 4, 5, 6)};
        auto result = exp | fxt::mtuple_drop<1>() | fxt::mtuple_drop<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 4);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<3>(result.value()) == 6);
    }
}

TEST_CASE("mdrop with fxt::expected containing fxt::flat_tuple", "[mdrop][expected][flat_tuple]") {
    SECTION("mdrop from expected with flat_tuple value") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int, int>, std::string>{fxt::make_flat_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
    }

    SECTION("mdrop from expected with flat_tuple and mixed types") {
        auto exp = fxt::expected<fxt::flat_tuple<int, double, std::string, char, bool>, std::string>{
            fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'x', true)
        };
        auto result = fxt::mtuple_drop<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == "hello");
        REQUIRE(fxt::get<1>(result.value()) == 'x');
        REQUIRE(fxt::get<2>(result.value()) == true);
    }

    SECTION("mdrop with pipe operator on expected with flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double, double>, std::string>{
            fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0)
        };
        auto result = exp | fxt::mtuple_drop<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2.0);
        REQUIRE(fxt::get<1>(result.value()) == 3.0);
        REQUIRE(fxt::get<2>(result.value()) == 4.0);
    }
}

TEST_CASE("mdrop with fxt::optional containing fxt::tuple", "[mdrop][optional][tuple]") {
    SECTION("mdrop from optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{fxt::make_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop<2>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
    }

    SECTION("mdrop from optional with nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{std::nullopt};
        auto result = fxt::mtuple_drop<2>(opt);

        REQUIRE(!result.has_value());
    }

    SECTION("mdrop with pipe operator on optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int>>{fxt::make_tuple(10, 20, 30, 40, 50)};
        auto result = opt | fxt::mtuple_drop<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 30);
        REQUIRE(fxt::get<1>(result.value()) == 40);
        REQUIRE(fxt::get<2>(result.value()) == 50);
    }

    SECTION("mdrop with pipe operator on optional with nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{std::nullopt};
        auto result = opt | fxt::mtuple_drop<2>();

        REQUIRE(!result.has_value());
    }

    SECTION("chained mdrop operations on optional") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int, int>>{fxt::make_tuple(1, 2, 3, 4, 5, 6)};
        auto result = opt | fxt::mtuple_drop<1>() | fxt::mtuple_drop<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 4);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<3>(result.value()) == 6);
    }
}

TEST_CASE("mdrop with fxt::optional containing fxt::flat_tuple", "[mdrop][optional][flat_tuple]") {
    SECTION("mdrop from optional with flat_tuple value") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int, int, int>>{fxt::make_flat_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop<2>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
    }

    SECTION("mdrop from optional with flat_tuple and mixed types") {
        auto opt = fxt::optional<fxt::flat_tuple<int, double, std::string, char>>{
            fxt::make_flat_tuple(42, 3.14, std::string("test"), 'y')
        };
        auto result = fxt::mtuple_drop<1>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 3.14);
        REQUIRE(fxt::get<1>(result.value()) == "test");
        REQUIRE(fxt::get<2>(result.value()) == 'y');
    }

    SECTION("mdrop with pipe operator on optional with flat_tuple") {
        auto opt = fxt::optional<fxt::flat_tuple<double, double, double, double>>{
            fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0)
        };
        auto result = opt | fxt::mtuple_drop<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2.0);
        REQUIRE(fxt::get<1>(result.value()) == 3.0);
        REQUIRE(fxt::get<2>(result.value()) == 4.0);
    }
}

// ===== TESTS: fxt::mdrop_last with fxt::expected containing tuples =====

TEST_CASE("mdrop_last with fxt::expected containing fxt::tuple", "[mdrop_last][expected][tuple]") {
    SECTION("mdrop_last from expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::make_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop_last<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
    }

    SECTION("mdrop_last from expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::unexpected{"Error"}};
        auto result = fxt::mtuple_drop_last<2>(exp);

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error");
    }

    SECTION("mdrop_last with pipe operator on expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{fxt::make_tuple(10, 20, 30, 40, 50)};
        auto result = exp | fxt::mtuple_drop_last<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 10);
        REQUIRE(fxt::get<1>(result.value()) == 20);
        REQUIRE(fxt::get<2>(result.value()) == 30);
    }

    SECTION("mdrop_last with pipe operator on expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::unexpected{"Error"}};
        auto result = exp | fxt::mtuple_drop_last<2>();

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error");
    }

    SECTION("mdrop_last all elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtuple_drop_last<3>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 0);
    }

    SECTION("mdrop_last zero elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(1, 2, 3)};
        auto result = fxt::mtuple_drop_last<0>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 3);
    }

    SECTION("chained mdrop_last operations on expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int>, std::string>{fxt::make_tuple(1, 2, 3, 4, 5, 6)};
        auto result = exp | fxt::mtuple_drop_last<1>() | fxt::mtuple_drop_last<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 4);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<3>(result.value()) == 4);
    }
}

TEST_CASE("mdrop_last with fxt::expected containing fxt::flat_tuple", "[mdrop_last][expected][flat_tuple]") {
    SECTION("mdrop_last from expected with flat_tuple value") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int, int>, std::string>{fxt::make_flat_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop_last<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
    }

    SECTION("mdrop_last from expected with flat_tuple and mixed types") {
        auto exp = fxt::expected<fxt::flat_tuple<int, double, std::string, char, bool>, std::string>{
            fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'x', true)
        };
        auto result = fxt::mtuple_drop_last<2>(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 42);
        REQUIRE(fxt::get<1>(result.value()) == 3.14);
        REQUIRE(fxt::get<2>(result.value()) == "hello");
    }

    SECTION("mdrop_last with pipe operator on expected with flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double, double>, std::string>{
            fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0)
        };
        auto result = exp | fxt::mtuple_drop_last<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1.0);
        REQUIRE(fxt::get<1>(result.value()) == 2.0);
        REQUIRE(fxt::get<2>(result.value()) == 3.0);
    }
}

TEST_CASE("mdrop_last with fxt::optional containing fxt::tuple", "[mdrop_last][optional][tuple]") {
    SECTION("mdrop_last from optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{fxt::make_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop_last<2>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
    }

    SECTION("mdrop_last from optional with nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{std::nullopt};
        auto result = fxt::mtuple_drop_last<2>(opt);

        REQUIRE(!result.has_value());
    }

    SECTION("mdrop_last with pipe operator on optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int>>{fxt::make_tuple(10, 20, 30, 40, 50)};
        auto result = opt | fxt::mtuple_drop_last<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 10);
        REQUIRE(fxt::get<1>(result.value()) == 20);
        REQUIRE(fxt::get<2>(result.value()) == 30);
    }

    SECTION("mdrop_last with pipe operator on optional with nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{std::nullopt};
        auto result = opt | fxt::mtuple_drop_last<2>();

        REQUIRE(!result.has_value());
    }

    SECTION("chained mdrop_last operations on optional") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int, int>>{fxt::make_tuple(1, 2, 3, 4, 5, 6)};
        auto result = opt | fxt::mtuple_drop_last<1>() | fxt::mtuple_drop_last<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 4);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<3>(result.value()) == 4);
    }
}

TEST_CASE("mdrop_last with fxt::optional containing fxt::flat_tuple", "[mdrop_last][optional][flat_tuple]") {
    SECTION("mdrop_last from optional with flat_tuple value") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int, int, int>>{fxt::make_flat_tuple(1, 2, 3, 4)};
        auto result = fxt::mtuple_drop_last<2>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 1);
        REQUIRE(fxt::get<1>(result.value()) == 2);
    }

    SECTION("mdrop_last from optional with flat_tuple and mixed types") {
        auto opt = fxt::optional<fxt::flat_tuple<int, double, std::string, char>>{
            fxt::make_flat_tuple(42, 3.14, std::string("test"), 'y')
        };
        auto result = fxt::mtuple_drop_last<1>(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 42);
        REQUIRE(fxt::get<1>(result.value()) == 3.14);
        REQUIRE(fxt::get<2>(result.value()) == "test");
    }

    SECTION("mdrop_last with pipe operator on optional with flat_tuple") {
        auto opt = fxt::optional<fxt::flat_tuple<double, double, double, double>>{
            fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0)
        };
        auto result = opt | fxt::mtuple_drop_last<1>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 1.0);
        REQUIRE(fxt::get<1>(result.value()) == 2.0);
        REQUIRE(fxt::get<2>(result.value()) == 3.0);
    }
}

// ===== TESTS: Combining mdrop and mdrop_last on monadic containers =====

TEST_CASE("combining mdrop and mdrop_last on fxt::expected", "[mdrop][mdrop_last][expected]") {
    SECTION("mdrop then mdrop_last on expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };
        auto result = exp | fxt::mtuple_drop<2>() | fxt::mtuple_drop_last<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 5);
    }

    SECTION("mdrop_last then mdrop on expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };
        auto result = exp | fxt::mtuple_drop_last<2>() | fxt::mtuple_drop<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 5);
    }

    SECTION("mdrop then mdrop_last on expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int>, std::string>{
            fxt::unexpected{"Error"}
        };
        auto result = exp | fxt::mtuple_drop<2>() | fxt::mtuple_drop_last<2>();

        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error");
    }

    SECTION("extracting middle section with mdrop and mdrop_last") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(10, 20, 30, 40, 50, 60, 70, 80, 90)
        };
        auto result = exp | fxt::mtuple_drop<3>() | fxt::mtuple_drop_last<3>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 40);
        REQUIRE(fxt::get<1>(result.value()) == 50);
        REQUIRE(fxt::get<2>(result.value()) == 60);
    }
}

TEST_CASE("combining mdrop and mdrop_last on fxt::optional", "[mdrop][mdrop_last][optional]") {
    SECTION("mdrop then mdrop_last on optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int, int, int>>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };
        auto result = opt | fxt::mtuple_drop<2>() | fxt::mtuple_drop_last<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 5);
    }

    SECTION("mdrop_last then mdrop on optional with value") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int, int, int>>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };
        auto result = opt | fxt::mtuple_drop_last<2>() | fxt::mtuple_drop<2>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 3);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 5);
    }

    SECTION("mdrop then mdrop_last on optional with nullopt") {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int, int, int, int>>{std::nullopt};
        auto result = opt | fxt::mtuple_drop<2>() | fxt::mtuple_drop_last<2>();

        REQUIRE(!result.has_value());
    }

    SECTION("extracting middle section with mdrop and mdrop_last on flat_tuple") {
        auto opt = fxt::optional<fxt::flat_tuple<int, int, int, int, int, int, int, int, int>>{
            fxt::make_flat_tuple(10, 20, 30, 40, 50, 60, 70, 80, 90)
        };
        auto result = opt | fxt::mtuple_drop<3>() | fxt::mtuple_drop_last<3>();

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 40);
        REQUIRE(fxt::get<1>(result.value()) == 50);
        REQUIRE(fxt::get<2>(result.value()) == 60);
    }
}
