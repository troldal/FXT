//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

TEST_CASE("drop with various tuple sizes", "[drop]") {
    SECTION("drop from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::mdrop<0>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::mdrop<1>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto drop0 = fxt::mdrop<0>(t);
        REQUIRE(std::get<0>(drop0) == 10);
        REQUIRE(std::get<1>(drop0) == 20);

        auto drop1 = fxt::mdrop<1>(t);
        REQUIRE(std::get<0>(drop1) == 20);

        auto drop2 = fxt::mdrop<2>(t);
        REQUIRE(std::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::mdrop<0>(t);
        REQUIRE(std::get<0>(drop0) == 1);
        REQUIRE(std::get<1>(drop0) == 2.5);
        REQUIRE(std::get<2>(drop0) == "three");

        auto drop1 = fxt::mdrop<1>(t);
        REQUIRE(std::get<0>(drop1) == 2.5);
        REQUIRE(std::get<1>(drop1) == "three");

        auto drop2 = fxt::mdrop<2>(t);
        REQUIRE(std::get<0>(drop2) == "three");

        auto drop3 = fxt::mdrop<3>(t);
        REQUIRE(std::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::mdrop<2>(t);
        REQUIRE(std::get<0>(drop2) == 3);
        REQUIRE(std::get<1>(drop2) == 4);
        REQUIRE(std::get<2>(drop2) == 5);

        auto drop4 = fxt::mdrop<4>(t);
        REQUIRE(std::get<0>(drop4) == 5);
    }

    SECTION("drop from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::mdrop<5>(t);
        REQUIRE(std::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(std::get<0>(drop5) == 6);
        REQUIRE(std::get<4>(drop5) == 10);

        auto drop9 = fxt::mdrop<9>(t);
        REQUIRE(std::get<0>(drop9) == 10);
    }
}

TEST_CASE("drop with different types", "[drop]") {
    SECTION("drop with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::mdrop<2>(t);
        REQUIRE(std::get<0>(result) == "hello");
        REQUIRE(std::get<1>(result) == 'c');
        REQUIRE(std::get<2>(result) == true);
    }

    SECTION("drop with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::mdrop<2>(t);

        REQUIRE(std::get<0>(result) == 30);
        REQUIRE(std::get<1>(result) == 40);
        REQUIRE(std::get<2>(result) == 50);
    }

    SECTION("drop with string types") {
        auto t = std::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::mdrop<1>(t);

        REQUIRE(std::get<0>(result) == "second");
        REQUIRE(std::get<1>(result) == "third");
        REQUIRE(std::get<2>(result) == "fourth");
    }
}

TEST_CASE("drop with pipe operator on fxt::tuple", "[drop][tuple][pipe]") {
    SECTION("basic pipe with drop") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = t | fxt::drop<2>();

        REQUIRE(std::get<0>(result) == 3);
        REQUIRE(std::get<1>(result) == 4);
        REQUIRE(std::get<2>(result) == 5);
    }

    SECTION("pipe with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("test"), 'x', true);
        auto result = t | fxt::drop<2>();

        REQUIRE(std::get<0>(result) == "test");
        REQUIRE(std::get<1>(result) == 'x');
        REQUIRE(std::get<2>(result) == true);
    }

    SECTION("pipe with rvalue tuple") {
        auto result = fxt::make_tuple(100, 200, 300, 400) | fxt::drop<2>();

        REQUIRE(std::get<0>(result) == 300);
        REQUIRE(std::get<1>(result) == 400);
    }

    SECTION("chained pipes") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto result = t | fxt::drop<2>() | fxt::drop<2>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 6);
        REQUIRE(std::get<0>(result) == 5);
        REQUIRE(std::get<5>(result) == 10);
    }
}

TEST_CASE("drop_last with various tuple sizes", "[drop_last]") {
    SECTION("drop_last from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::mdrop_last<0>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::mdrop_last<1>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop_last from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto drop0 = fxt::mdrop_last<0>(t);
        REQUIRE(std::get<0>(drop0) == 10);
        REQUIRE(std::get<1>(drop0) == 20);

        auto drop1 = fxt::mdrop_last<1>(t);
        REQUIRE(std::get<0>(drop1) == 10);

        auto drop2 = fxt::mdrop_last<2>(t);
        REQUIRE(std::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop_last from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::mdrop_last<0>(t);
        REQUIRE(std::get<0>(drop0) == 1);
        REQUIRE(std::get<1>(drop0) == 2.5);
        REQUIRE(std::get<2>(drop0) == "three");

        auto drop1 = fxt::mdrop_last<1>(t);
        REQUIRE(std::get<0>(drop1) == 1);
        REQUIRE(std::get<1>(drop1) == 2.5);

        auto drop2 = fxt::mdrop_last<2>(t);
        REQUIRE(std::get<0>(drop2) == 1);

        auto drop3 = fxt::mdrop_last<3>(t);
        REQUIRE(std::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop_last from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::mdrop_last<2>(t);
        REQUIRE(std::get<0>(drop2) == 1);
        REQUIRE(std::get<1>(drop2) == 2);
        REQUIRE(std::get<2>(drop2) == 3);

        auto drop4 = fxt::mdrop_last<4>(t);
        REQUIRE(std::get<0>(drop4) == 1);
    }

    SECTION("drop_last from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::mdrop_last<5>(t);
        REQUIRE(std::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(std::get<0>(drop5) == 1);
        REQUIRE(std::get<4>(drop5) == 5);

        auto drop9 = fxt::mdrop_last<9>(t);
        REQUIRE(std::get<0>(drop9) == 1);
    }
}

TEST_CASE("drop_last with different types", "[drop_last]") {
    SECTION("drop_last with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::mdrop_last<2>(t);
        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "hello");
    }

    SECTION("drop_last with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::mdrop_last<2>(t);

        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 20);
        REQUIRE(std::get<2>(result) == 30);
    }

    SECTION("drop_last with string types") {
        auto t = std::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::mdrop_last<1>(t);

        REQUIRE(std::get<0>(result) == "first");
        REQUIRE(std::get<1>(result) == "second");
        REQUIRE(std::get<2>(result) == "third");
    }
}

TEST_CASE("drop_last with pipe operator on fxt::tuple", "[drop_last][tuple][pipe]") {
    SECTION("basic pipe with drop_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = t | fxt::drop_last<2>();

        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<1>(result) == 2);
        REQUIRE(std::get<2>(result) == 3);
    }

    SECTION("pipe with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("test"), 'x', true);
        auto result = t | fxt::drop_last<2>();

        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "test");
    }

    SECTION("pipe with rvalue tuple") {
        auto result = fxt::make_tuple(100, 200, 300, 400) | fxt::drop_last<2>();

        REQUIRE(std::get<0>(result) == 100);
        REQUIRE(std::get<1>(result) == 200);
    }

    SECTION("chained pipes") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto result = t | fxt::drop_last<2>() | fxt::drop_last<2>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 6);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<5>(result) == 6);
    }
}

TEST_CASE("drop and drop_last edge cases", "[drop][drop_last]") {
    SECTION("drop 0 elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::mdrop<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop_last 0 elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::mdrop_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop all elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::mdrop<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("drop_last all elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::mdrop_last<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("drop and drop_last with move-only types", "[drop][drop_last]") {
    SECTION("drop with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::mdrop<1>(std::move(t));
        REQUIRE(*std::get<0>(result) == 100);
        REQUIRE(*std::get<1>(result) == 200);
    }

    SECTION("drop_last with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::mdrop_last<1>(std::move(t));
        REQUIRE(*std::get<0>(result) == 42);
        REQUIRE(*std::get<1>(result) == 100);
    }
}

TEST_CASE("drop and drop_last combined", "[drop][drop_last]") {
    SECTION("drop then drop_last") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_first = fxt::mdrop<2>(t);  // {3, 4, 5, 6, 7}
        auto dropped_both = fxt::mdrop_last<2>(dropped_first);  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(std::get<0>(dropped_both) == 3);
        REQUIRE(std::get<1>(dropped_both) == 4);
        REQUIRE(std::get<2>(dropped_both) == 5);
    }

    SECTION("drop_last then drop") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_last = fxt::mdrop_last<2>(t);  // {1, 2, 3, 4, 5}
        auto dropped_both = fxt::mdrop<2>(dropped_last);  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(std::get<0>(dropped_both) == 3);
        REQUIRE(std::get<1>(dropped_both) == 4);
        REQUIRE(std::get<2>(dropped_both) == 5);
    }

    SECTION("symmetry: drop then drop_last vs drop_last then drop") {
        auto t1 = std::make_tuple(10, 20, 30, 40, 50);
        auto t2 = std::make_tuple(10, 20, 30, 40, 50);

        auto result1 = fxt::mdrop_last<1>(fxt::mdrop<1>(t1));
        auto result2 = fxt::mdrop<1>(fxt::mdrop_last<1>(t2));

        REQUIRE(std::get<0>(result1) == std::get<0>(result2));
        REQUIRE(std::get<1>(result1) == std::get<1>(result2));
        REQUIRE(std::get<2>(result1) == std::get<2>(result2));
    }
}

TEST_CASE("drop and drop_last with large tuples", "[drop][drop_last]") {
    SECTION("drop from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::mdrop<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 11);
        REQUIRE(std::get<4>(result) == 15);
    }

    SECTION("drop_last from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::mdrop_last<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<4>(result) == 5);
    }
}

// ===== NEW TESTS: fxt::flat_tuple with mdrop and mdrop_last =====

TEST_CASE("mdrop with fxt::flat_tuple", "[drop][flat_tuple]") {
    SECTION("mdrop from 1-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(42);
        auto result = fxt::mdrop<0>(ft);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::mdrop<1>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("mdrop from 3-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);

        auto drop0 = fxt::mdrop<0>(ft);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);
        REQUIRE(fxt::get<2>(drop0) == 30);

        auto drop1 = fxt::mdrop<1>(ft);
        REQUIRE(fxt::get<0>(drop1) == 20);
        REQUIRE(fxt::get<1>(drop1) == 30);

        auto drop2 = fxt::mdrop<2>(ft);
        REQUIRE(fxt::get<0>(drop2) == 30);

        auto drop3 = fxt::mdrop<3>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("mdrop from 5-element flat_tuple with mixed types") {
        auto ft = fxt::make_flat_tuple(1, 2.5, std::string("test"), 'x', true);

        auto result = fxt::mdrop<2>(ft);
        REQUIRE(fxt::get<0>(result) == "test");
        REQUIRE(fxt::get<1>(result) == 'x');
        REQUIRE(fxt::get<2>(result) == true);
    }

    SECTION("mdrop from rvalue flat_tuple") {
        auto result = fxt::mdrop<2>(fxt::make_flat_tuple(100, 200, 300, 400));
        REQUIRE(fxt::get<0>(result) == 300);
        REQUIRE(fxt::get<1>(result) == 400);
    }
}

TEST_CASE("mdrop_last with fxt::flat_tuple", "[drop_last][flat_tuple]") {
    SECTION("mdrop_last from 1-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(42);
        auto result = fxt::mdrop_last<0>(ft);
        REQUIRE(fxt::get<0>(result) == 42);

        auto empty = fxt::mdrop_last<1>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("mdrop_last from 3-element flat_tuple") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);

        auto drop0 = fxt::mdrop_last<0>(ft);
        REQUIRE(fxt::get<0>(drop0) == 10);
        REQUIRE(fxt::get<1>(drop0) == 20);
        REQUIRE(fxt::get<2>(drop0) == 30);

        auto drop1 = fxt::mdrop_last<1>(ft);
        REQUIRE(fxt::get<0>(drop1) == 10);
        REQUIRE(fxt::get<1>(drop1) == 20);

        auto drop2 = fxt::mdrop_last<2>(ft);
        REQUIRE(fxt::get<0>(drop2) == 10);

        auto drop3 = fxt::mdrop_last<3>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("mdrop_last from 5-element flat_tuple with mixed types") {
        auto ft = fxt::make_flat_tuple(1, 2.5, std::string("test"), 'x', true);

        auto result = fxt::mdrop_last<2>(ft);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == "test");
    }

    SECTION("mdrop_last from rvalue flat_tuple") {
        auto result = fxt::mdrop_last<2>(fxt::make_flat_tuple(100, 200, 300, 400));
        REQUIRE(fxt::get<0>(result) == 100);
        REQUIRE(fxt::get<1>(result) == 200);
    }
}

// ===== NEW TESTS: fxt::drop (direct call) with fxt::tuple and fxt::flat_tuple =====

TEST_CASE("drop (direct call) with fxt::tuple", "[drop][tuple][direct]") {
    SECTION("drop with fxt::make_tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = fxt::drop<2>(t);

        REQUIRE(std::get<0>(result) == 3);
        REQUIRE(std::get<1>(result) == 4);
        REQUIRE(std::get<2>(result) == 5);
    }

    SECTION("drop with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), 'c');
        auto result = fxt::drop<1>(t);

        REQUIRE(std::get<0>(result) == 3.14);
        REQUIRE(std::get<1>(result) == "hello");
        REQUIRE(std::get<2>(result) == 'c');
    }

    SECTION("drop zero elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto result = fxt::drop<0>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
        REQUIRE(std::get<0>(result) == 10);
    }

    SECTION("drop all elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::drop<3>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("drop (direct call) with fxt::flat_tuple", "[drop][flat_tuple][direct]") {
    SECTION("drop with flat_tuple") {
        auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5);
        auto result = fxt::drop<2>(ft);

        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 5);
    }

    SECTION("drop with mixed types") {
        auto ft = fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'c');
        auto result = fxt::drop<1>(ft);

        REQUIRE(fxt::get<0>(result) == 3.14);
        REQUIRE(fxt::get<1>(result) == "hello");
        REQUIRE(fxt::get<2>(result) == 'c');
    }

    SECTION("drop zero elements") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);
        auto result = fxt::drop<0>(ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 10);
    }

    SECTION("drop all elements") {
        auto ft = fxt::make_flat_tuple(1, 2, 3);
        auto result = fxt::drop<3>(ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("drop_last (direct call) with fxt::tuple", "[drop_last][tuple][direct]") {
    SECTION("drop_last with fxt::make_tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);
        auto result = fxt::drop_last<2>(t);

        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<1>(result) == 2);
        REQUIRE(std::get<2>(result) == 3);
    }

    SECTION("drop_last with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), 'c');
        auto result = fxt::drop_last<1>(t);

        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "hello");
    }

    SECTION("drop_last zero elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto result = fxt::drop_last<0>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
        REQUIRE(std::get<0>(result) == 10);
    }

    SECTION("drop_last all elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::drop_last<3>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }
}

TEST_CASE("drop_last (direct call) with fxt::flat_tuple", "[drop_last][flat_tuple][direct]") {
    SECTION("drop_last with flat_tuple") {
        auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5);
        auto result = fxt::drop_last<2>(ft);

        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("drop_last with mixed types") {
        auto ft = fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'c');
        auto result = fxt::drop_last<1>(ft);

        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 3.14);
        REQUIRE(fxt::get<2>(result) == "hello");
    }

    SECTION("drop_last zero elements") {
        auto ft = fxt::make_flat_tuple(10, 20, 30);
        auto result = fxt::drop_last<0>(ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 10);
    }

    SECTION("drop_last all elements") {
        auto ft = fxt::make_flat_tuple(1, 2, 3);
        auto result = fxt::drop_last<3>(ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }
}

// ===== NEW TESTS: combining drop and drop_last with pipes =====

TEST_CASE("combining drop and drop_last with pipe on fxt::tuple", "[drop][drop_last][tuple][pipe]") {
    SECTION("drop then drop_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9);
        auto result = t | fxt::drop<2>() | fxt::drop_last<2>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 3);
        REQUIRE(std::get<4>(result) == 7);
    }

    SECTION("drop_last then drop") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9);
        auto result = t | fxt::drop_last<2>() | fxt::drop<2>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 3);
        REQUIRE(std::get<4>(result) == 7);
    }

    SECTION("extracting middle section") {
        auto t = fxt::make_tuple(10, 20, 30, 40, 50, 60, 70, 80, 90);
        auto result = t | fxt::drop<3>() | fxt::drop_last<3>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
        REQUIRE(std::get<0>(result) == 40);
        REQUIRE(std::get<1>(result) == 50);
        REQUIRE(std::get<2>(result) == 60);
    }

    SECTION("complex chaining") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
        auto result = t | fxt::drop<2>() | fxt::drop_last<3>() | fxt::drop<1>() | fxt::drop_last<1>();

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 4);
        REQUIRE(std::get<4>(result) == 8);
    }
}

TEST_CASE("combining drop and drop_last with pipe on fxt::flat_tuple", "[drop][drop_last][flat_tuple][pipe]") {
    SECTION("drop then drop_last") {
        auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9);
        auto result = ft | fxt::drop<2>() | fxt::drop_last<2>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<4>(result) == 7);
    }

    SECTION("drop_last then drop") {
        auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9);
        auto result = ft | fxt::drop_last<2>() | fxt::drop<2>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<4>(result) == 7);
    }

    SECTION("extracting middle section") {
        auto ft = fxt::make_flat_tuple(10, 20, 30, 40, 50, 60, 70, 80, 90);
        auto result = ft | fxt::drop<3>() | fxt::drop_last<3>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 40);
        REQUIRE(fxt::get<1>(result) == 50);
        REQUIRE(fxt::get<2>(result) == 60);
    }

    SECTION("complex chaining with mixed types") {
        auto ft = fxt::make_flat_tuple(1, 2.0, 3, 4.0, 5, 6.0, 7, 8.0, 9, 10.0);
        auto result = ft | fxt::drop<2>() | fxt::drop_last<2>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 6);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<5>(result) == 8.0);
    }
}

// ===== NEW TESTS: comparing mdrop vs drop behavior =====

TEST_CASE("comparing mdrop and drop behavior with fxt::tuple", "[drop][tuple][comparison]") {
    SECTION("mdrop and drop produce same result") {
        auto t = fxt::make_tuple(5, 10, 15, 20, 25);

        auto mdrop_result = fxt::mdrop<2>(t);
        auto drop_result = fxt::drop<2>(t);

        REQUIRE(std::get<0>(mdrop_result) == std::get<0>(drop_result));
        REQUIRE(std::get<1>(mdrop_result) == std::get<1>(drop_result));
        REQUIRE(std::get<2>(mdrop_result) == std::get<2>(drop_result));
    }

    SECTION("mdrop_last and drop_last produce same result") {
        auto t = fxt::make_tuple(5, 10, 15, 20, 25);

        auto mdrop_last_result = fxt::mdrop_last<2>(t);
        auto drop_last_result = fxt::drop_last<2>(t);

        REQUIRE(std::get<0>(mdrop_last_result) == std::get<0>(drop_last_result));
        REQUIRE(std::get<1>(mdrop_last_result) == std::get<1>(drop_last_result));
        REQUIRE(std::get<2>(mdrop_last_result) == std::get<2>(drop_last_result));
    }
}

TEST_CASE("comparing mdrop and drop behavior with fxt::flat_tuple", "[drop][flat_tuple][comparison]") {
    SECTION("mdrop and drop produce same result") {
        auto ft = fxt::make_flat_tuple(5.0, 10.0, 15.0, 20.0, 25.0);

        auto mdrop_result = fxt::mdrop<2>(ft);
        auto drop_result = fxt::drop<2>(ft);

        REQUIRE(fxt::get<0>(mdrop_result) == fxt::get<0>(drop_result));
        REQUIRE(fxt::get<1>(mdrop_result) == fxt::get<1>(drop_result));
        REQUIRE(fxt::get<2>(mdrop_result) == fxt::get<2>(drop_result));
    }

    SECTION("mdrop_last and drop_last produce same result") {
        auto ft = fxt::make_flat_tuple(5.0, 10.0, 15.0, 20.0, 25.0);

        auto mdrop_last_result = fxt::mdrop_last<2>(ft);
        auto drop_last_result = fxt::drop_last<2>(ft);

        REQUIRE(fxt::get<0>(mdrop_last_result) == fxt::get<0>(drop_last_result));
        REQUIRE(fxt::get<1>(mdrop_last_result) == fxt::get<1>(drop_last_result));
        REQUIRE(fxt::get<2>(mdrop_last_result) == fxt::get<2>(drop_last_result));
    }
}
