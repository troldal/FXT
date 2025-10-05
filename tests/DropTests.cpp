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
        auto result = fxt::drop<0>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::drop<1>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto drop0 = fxt::drop<0>(t);
        REQUIRE(std::get<0>(drop0) == 10);
        REQUIRE(std::get<1>(drop0) == 20);

        auto drop1 = fxt::drop<1>(t);
        REQUIRE(std::get<0>(drop1) == 20);

        auto drop2 = fxt::drop<2>(t);
        REQUIRE(std::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::drop<0>(t);
        REQUIRE(std::get<0>(drop0) == 1);
        REQUIRE(std::get<1>(drop0) == 2.5);
        REQUIRE(std::get<2>(drop0) == "three");

        auto drop1 = fxt::drop<1>(t);
        REQUIRE(std::get<0>(drop1) == 2.5);
        REQUIRE(std::get<1>(drop1) == "three");

        auto drop2 = fxt::drop<2>(t);
        REQUIRE(std::get<0>(drop2) == "three");

        auto drop3 = fxt::drop<3>(t);
        REQUIRE(std::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::drop<2>(t);
        REQUIRE(std::get<0>(drop2) == 3);
        REQUIRE(std::get<1>(drop2) == 4);
        REQUIRE(std::get<2>(drop2) == 5);

        auto drop4 = fxt::drop<4>(t);
        REQUIRE(std::get<0>(drop4) == 5);
    }

    SECTION("drop from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::drop<5>(t);
        REQUIRE(std::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(std::get<0>(drop5) == 6);
        REQUIRE(std::get<4>(drop5) == 10);

        auto drop9 = fxt::drop<9>(t);
        REQUIRE(std::get<0>(drop9) == 10);
    }
}

TEST_CASE("drop with different types", "[drop]") {
    SECTION("drop with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::drop<2>(t);
        REQUIRE(std::get<0>(result) == "hello");
        REQUIRE(std::get<1>(result) == 'c');
        REQUIRE(std::get<2>(result) == true);
    }

    SECTION("drop with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::drop<2>(t);

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
        auto result = fxt::drop<1>(t);

        REQUIRE(std::get<0>(result) == "second");
        REQUIRE(std::get<1>(result) == "third");
        REQUIRE(std::get<2>(result) == "fourth");
    }
}

TEST_CASE("drop with rvalue tuples", "[drop]") {
    SECTION("drop from rvalue 3-element tuple") {
        auto result = fxt::drop<1>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 200);
        REQUIRE(std::get<1>(result) == 300);
    }

    SECTION("drop from rvalue mixed-type tuple") {
        auto result = fxt::drop<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == "test");
        REQUIRE(std::get<1>(result) == 'x');
    }
}

TEST_CASE("drop_last with various tuple sizes", "[drop_last]") {
    SECTION("drop_last from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::drop_last<0>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::drop_last<1>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("drop_last from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto drop0 = fxt::drop_last<0>(t);
        REQUIRE(std::get<0>(drop0) == 10);
        REQUIRE(std::get<1>(drop0) == 20);

        auto drop1 = fxt::drop_last<1>(t);
        REQUIRE(std::get<0>(drop1) == 10);

        auto drop2 = fxt::drop_last<2>(t);
        REQUIRE(std::tuple_size_v<decltype(drop2)> == 0);
    }

    SECTION("drop_last from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto drop0 = fxt::drop_last<0>(t);
        REQUIRE(std::get<0>(drop0) == 1);
        REQUIRE(std::get<1>(drop0) == 2.5);
        REQUIRE(std::get<2>(drop0) == "three");

        auto drop1 = fxt::drop_last<1>(t);
        REQUIRE(std::get<0>(drop1) == 1);
        REQUIRE(std::get<1>(drop1) == 2.5);

        auto drop2 = fxt::drop_last<2>(t);
        REQUIRE(std::get<0>(drop2) == 1);

        auto drop3 = fxt::drop_last<3>(t);
        REQUIRE(std::tuple_size_v<decltype(drop3)> == 0);
    }

    SECTION("drop_last from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto drop2 = fxt::drop_last<2>(t);
        REQUIRE(std::get<0>(drop2) == 1);
        REQUIRE(std::get<1>(drop2) == 2);
        REQUIRE(std::get<2>(drop2) == 3);

        auto drop4 = fxt::drop_last<4>(t);
        REQUIRE(std::get<0>(drop4) == 1);
    }

    SECTION("drop_last from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto drop5 = fxt::drop_last<5>(t);
        REQUIRE(std::tuple_size_v<decltype(drop5)> == 5);
        REQUIRE(std::get<0>(drop5) == 1);
        REQUIRE(std::get<4>(drop5) == 5);

        auto drop9 = fxt::drop_last<9>(t);
        REQUIRE(std::get<0>(drop9) == 1);
    }
}

TEST_CASE("drop_last with different types", "[drop_last]") {
    SECTION("drop_last with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::drop_last<2>(t);
        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "hello");
    }

    SECTION("drop_last with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::drop_last<2>(t);

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
        auto result = fxt::drop_last<1>(t);

        REQUIRE(std::get<0>(result) == "first");
        REQUIRE(std::get<1>(result) == "second");
        REQUIRE(std::get<2>(result) == "third");
    }
}

TEST_CASE("drop_last with rvalue tuples", "[drop_last]") {
    SECTION("drop_last from rvalue 3-element tuple") {
        auto result = fxt::drop_last<1>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 100);
        REQUIRE(std::get<1>(result) == 200);
    }

    SECTION("drop_last from rvalue mixed-type tuple") {
        auto result = fxt::drop_last<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<1>(result) == 2.5);
    }
}

TEST_CASE("drop and drop_last edge cases", "[drop][drop_last]") {
    SECTION("drop 0 elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::drop<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop_last 0 elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::drop_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 3);
    }

    SECTION("drop all elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::drop<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("drop_last all elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::drop_last<4>(t);
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
        auto result = fxt::drop<1>(std::move(t));
        REQUIRE(*std::get<0>(result) == 100);
        REQUIRE(*std::get<1>(result) == 200);
    }

    SECTION("drop_last with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::drop_last<1>(std::move(t));
        REQUIRE(*std::get<0>(result) == 42);
        REQUIRE(*std::get<1>(result) == 100);
    }
}

TEST_CASE("drop and drop_last combined", "[drop][drop_last]") {
    SECTION("drop then drop_last") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_first = fxt::drop<2>(t);  // {3, 4, 5, 6, 7}
        auto dropped_both = fxt::drop_last<2>(dropped_first);  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(std::get<0>(dropped_both) == 3);
        REQUIRE(std::get<1>(dropped_both) == 4);
        REQUIRE(std::get<2>(dropped_both) == 5);
    }

    SECTION("drop_last then drop") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        auto dropped_last = fxt::drop_last<2>(t);  // {1, 2, 3, 4, 5}
        auto dropped_both = fxt::drop<2>(dropped_last);  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(dropped_both)> == 3);
        REQUIRE(std::get<0>(dropped_both) == 3);
        REQUIRE(std::get<1>(dropped_both) == 4);
        REQUIRE(std::get<2>(dropped_both) == 5);
    }

    SECTION("symmetry: drop then drop_last vs drop_last then drop") {
        auto t1 = std::make_tuple(10, 20, 30, 40, 50);
        auto t2 = std::make_tuple(10, 20, 30, 40, 50);

        auto result1 = fxt::drop_last<1>(fxt::drop<1>(t1));
        auto result2 = fxt::drop<1>(fxt::drop_last<1>(t2));

        REQUIRE(std::get<0>(result1) == std::get<0>(result2));
        REQUIRE(std::get<1>(result1) == std::get<1>(result2));
        REQUIRE(std::get<2>(result1) == std::get<2>(result2));
    }
}

TEST_CASE("drop and drop_last with large tuples", "[drop][drop_last]") {
    SECTION("drop from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::drop<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 11);
        REQUIRE(std::get<4>(result) == 15);
    }

    SECTION("drop_last from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::drop_last<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 5);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<4>(result) == 5);
    }
}

