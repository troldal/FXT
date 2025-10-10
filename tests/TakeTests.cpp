//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

TEST_CASE("take with various tuple sizes", "[take]") {
    SECTION("take from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::mtake<1>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::mtake<0>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("take from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto take0 = fxt::mtake<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::mtake<1>(t);
        REQUIRE(std::get<0>(take1) == 10);

        auto take2 = fxt::mtake<2>(t);
        REQUIRE(std::get<0>(take2) == 10);
        REQUIRE(std::get<1>(take2) == 20);
    }

    SECTION("take from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto take0 = fxt::mtake<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::mtake<1>(t);
        REQUIRE(std::get<0>(take1) == 1);

        auto take2 = fxt::mtake<2>(t);
        REQUIRE(std::get<0>(take2) == 1);
        REQUIRE(std::get<1>(take2) == 2.5);

        auto take3 = fxt::mtake<3>(t);
        REQUIRE(std::get<0>(take3) == 1);
        REQUIRE(std::get<1>(take3) == 2.5);
        REQUIRE(std::get<2>(take3) == "three");
    }

    SECTION("take from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::mtake<2>(t);
        REQUIRE(std::get<0>(take2) == 1);
        REQUIRE(std::get<1>(take2) == 2);

        auto take4 = fxt::mtake<4>(t);
        REQUIRE(std::get<0>(take4) == 1);
        REQUIRE(std::get<3>(take4) == 4);
    }

    SECTION("take from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto take5 = fxt::mtake<5>(t);
        REQUIRE(std::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(std::get<0>(take5) == 1);
        REQUIRE(std::get<4>(take5) == 5);

        auto take9 = fxt::mtake<9>(t);
        REQUIRE(std::get<0>(take9) == 1);
        REQUIRE(std::get<8>(take9) == 9);
    }
}

TEST_CASE("take with different types", "[take]") {
    SECTION("take with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::mtake<3>(t);
        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "hello");
    }

    SECTION("take with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::mtake<3>(t);

        REQUIRE(std::get<0>(result) == 10);
        REQUIRE(std::get<1>(result) == 20);
        REQUIRE(std::get<2>(result) == 30);
    }

    SECTION("take with string types") {
        auto t = std::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::mtake<2>(t);

        REQUIRE(std::get<0>(result) == "first");
        REQUIRE(std::get<1>(result) == "second");
    }
}

TEST_CASE("take with rvalue tuples", "[take]") {
    SECTION("take from rvalue 3-element tuple") {
        auto result = fxt::mtake<2>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 100);
        REQUIRE(std::get<1>(result) == 200);
    }

    SECTION("take from rvalue mixed-type tuple") {
        auto result = fxt::mtake<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<1>(result) == 2.5);
    }
}

TEST_CASE("take_last with various tuple sizes", "[take_last]") {
    SECTION("take_last from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::mtake_last<1>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::mtake_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("take_last from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto take0 = fxt::mtake_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::mtake_last<1>(t);
        REQUIRE(std::get<0>(take1) == 20);

        auto take2 = fxt::mtake_last<2>(t);
        REQUIRE(std::get<0>(take2) == 10);
        REQUIRE(std::get<1>(take2) == 20);
    }

    SECTION("take_last from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto take0 = fxt::mtake_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::mtake_last<1>(t);
        REQUIRE(std::get<0>(take1) == "three");

        auto take2 = fxt::mtake_last<2>(t);
        REQUIRE(std::get<0>(take2) == 2.5);
        REQUIRE(std::get<1>(take2) == "three");

        auto take3 = fxt::mtake_last<3>(t);
        REQUIRE(std::get<0>(take3) == 1);
        REQUIRE(std::get<1>(take3) == 2.5);
        REQUIRE(std::get<2>(take3) == "three");
    }

    SECTION("take_last from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::mtake_last<2>(t);
        REQUIRE(std::get<0>(take2) == 4);
        REQUIRE(std::get<1>(take2) == 5);

        auto take4 = fxt::mtake_last<4>(t);
        REQUIRE(std::get<0>(take4) == 2);
        REQUIRE(std::get<3>(take4) == 5);
    }

    SECTION("take_last from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto take5 = fxt::mtake_last<5>(t);
        REQUIRE(std::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(std::get<0>(take5) == 6);
        REQUIRE(std::get<4>(take5) == 10);

        auto take9 = fxt::mtake_last<9>(t);
        REQUIRE(std::get<0>(take9) == 2);
        REQUIRE(std::get<8>(take9) == 10);
    }
}

TEST_CASE("take_last with different types", "[take_last]") {
    SECTION("take_last with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::mtake_last<3>(t);
        REQUIRE(std::get<0>(result) == "hello");
        REQUIRE(std::get<1>(result) == 'c');
        REQUIRE(std::get<2>(result) == true);
    }

    SECTION("take_last with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::mtake_last<3>(t);

        REQUIRE(std::get<0>(result) == 30);
        REQUIRE(std::get<1>(result) == 40);
        REQUIRE(std::get<2>(result) == 50);
    }

    SECTION("take_last with string types") {
        auto t = std::make_tuple(
            std::string("first"),
            std::string("second"),
            std::string("third"),
            std::string("fourth")
        );
        auto result = fxt::mtake_last<2>(t);

        REQUIRE(std::get<0>(result) == "third");
        REQUIRE(std::get<1>(result) == "fourth");
    }
}

TEST_CASE("take_last with rvalue tuples", "[take_last]") {
    SECTION("take_last from rvalue 3-element tuple") {
        auto result = fxt::mtake_last<2>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 200);
        REQUIRE(std::get<1>(result) == 300);
    }

    SECTION("take_last from rvalue mixed-type tuple") {
        auto result = fxt::mtake_last<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == "test");
        REQUIRE(std::get<1>(result) == 'x');
    }
}

TEST_CASE("take and take_last edge cases", "[take][take_last]") {
    SECTION("take 0 elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::mtake<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("take_last 0 elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::mtake_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("take all elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::mtake<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 4);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<3>(result) == 4);
    }

    SECTION("take_last all elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::mtake_last<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 4);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<3>(result) == 4);
    }
}

TEST_CASE("take and take_last with move-only types", "[take][take_last]") {
    SECTION("take with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::mtake<2>(std::move(t));
        REQUIRE(*std::get<0>(result) == 42);
        REQUIRE(*std::get<1>(result) == 100);
    }

    SECTION("take_last with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::mtake_last<2>(std::move(t));
        REQUIRE(*std::get<0>(result) == 100);
        REQUIRE(*std::get<1>(result) == 200);
    }
}

TEST_CASE("take and take_last combined", "[take][take_last]") {
    SECTION("take then take_last") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto first_take = fxt::mtake<7>(t);  // {1, 2, 3, 4, 5, 6, 7}
        auto both_taken = fxt::mtake_last<5>(first_take);  // {3, 4, 5, 6, 7}

        REQUIRE(std::tuple_size_v<decltype(both_taken)> == 5);
        REQUIRE(std::get<0>(both_taken) == 3);
        REQUIRE(std::get<1>(both_taken) == 4);
        REQUIRE(std::get<2>(both_taken) == 5);
        REQUIRE(std::get<3>(both_taken) == 6);
        REQUIRE(std::get<4>(both_taken) == 7);
    }

    SECTION("take_last then take") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto last_take = fxt::mtake_last<7>(t);  // {4, 5, 6, 7, 8, 9, 10}
        auto both_taken = fxt::mtake<5>(last_take);  // {4, 5, 6, 7, 8}

        REQUIRE(std::tuple_size_v<decltype(both_taken)> == 5);
        REQUIRE(std::get<0>(both_taken) == 4);
        REQUIRE(std::get<1>(both_taken) == 5);
        REQUIRE(std::get<2>(both_taken) == 6);
        REQUIRE(std::get<3>(both_taken) == 7);
        REQUIRE(std::get<4>(both_taken) == 8);
    }

    SECTION("extract middle elements using take and take_last") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        // Take first 5 elements, then take last 3 from that
        auto middle = fxt::mtake_last<3>(fxt::mtake<5>(t));  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(middle)> == 3);
        REQUIRE(std::get<0>(middle) == 3);
        REQUIRE(std::get<1>(middle) == 4);
        REQUIRE(std::get<2>(middle) == 5);
    }

    SECTION("alternative middle extraction") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
        // Take last 5 elements, then take first 3 from that
        auto middle = fxt::mtake<3>(fxt::mtake_last<5>(t));  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(middle)> == 3);
        REQUIRE(std::get<0>(middle) == 3);
        REQUIRE(std::get<1>(middle) == 4);
        REQUIRE(std::get<2>(middle) == 5);
    }
}

TEST_CASE("take and take_last with large tuples", "[take][take_last]") {
    SECTION("take from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::mtake<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 10);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<9>(result) == 10);
    }

    SECTION("take_last from 15-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
        auto result = fxt::mtake_last<10>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 10);
        REQUIRE(std::get<0>(result) == 6);
        REQUIRE(std::get<9>(result) == 15);
    }
}

TEST_CASE("take and take_last complementary operations", "[take][take_last]") {
    SECTION("take and take_last are complementary") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto first_part = fxt::mtake<3>(t);  // {1, 2, 3}
        auto last_part = fxt::mtake_last<2>(t);  // {4, 5}

        // Verify first part
        REQUIRE(std::get<0>(first_part) == 1);
        REQUIRE(std::get<1>(first_part) == 2);
        REQUIRE(std::get<2>(first_part) == 3);

        // Verify last part
        REQUIRE(std::get<0>(last_part) == 4);
        REQUIRE(std::get<1>(last_part) == 5);
    }

    SECTION("take N and take_last (size-N) partition tuple") {
        auto t = std::make_tuple(10, 20, 30, 40, 50, 60, 70);

        auto first = fxt::mtake<4>(t);
        auto last = fxt::mtake_last<3>(t);

        REQUIRE(std::tuple_size_v<decltype(first)> == 4);
        REQUIRE(std::tuple_size_v<decltype(last)> == 3);
        REQUIRE(std::get<3>(first) == 40);
        REQUIRE(std::get<0>(last) == 50);
    }
}

TEST_CASE("take and take_last single element", "[take][take_last]") {
    SECTION("take one element from beginning") {
        auto t = std::make_tuple(100, 200, 300, 400);
        auto result = fxt::mtake<1>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 1);
        REQUIRE(std::get<0>(result) == 100);
    }

    SECTION("take_last one element from end") {
        auto t = std::make_tuple(100, 200, 300, 400);
        auto result = fxt::mtake_last<1>(t);

        REQUIRE(std::tuple_size_v<decltype(result)> == 1);
        REQUIRE(std::get<0>(result) == 400);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple with mtake and mtake_last
// ============================================================================

TEST_CASE("mtake with fxt::flat_tuple", "[mtake][flat_tuple]") {
    SECTION("mtake from flat_tuple with various sizes") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto take2 = fxt::mtake<2>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 1.0);
        REQUIRE(fxt::get<1>(take2) == 2.0);

        auto take0 = fxt::mtake<0>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);

        auto take5 = fxt::mtake<5>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(fxt::get<4>(take5) == 5.0);
    }

    SECTION("mtake from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(10, 20, 30, 40);

        auto take3 = fxt::mtake<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 10);
        REQUIRE(fxt::get<1>(take3) == 20);
        REQUIRE(fxt::get<2>(take3) == 30);
    }

    SECTION("mtake from flat_tuple rvalue") {
        auto result = fxt::mtake<3>(fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0));
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }
}

TEST_CASE("mtake_last with fxt::flat_tuple", "[mtake_last][flat_tuple]") {
    SECTION("mtake_last from flat_tuple with various sizes") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto take2 = fxt::mtake_last<2>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 4.0);
        REQUIRE(fxt::get<1>(take2) == 5.0);

        auto take0 = fxt::mtake_last<0>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);

        auto take5 = fxt::mtake_last<5>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(fxt::get<0>(take5) == 1.0);
    }

    SECTION("mtake_last from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(10, 20, 30, 40);

        auto take3 = fxt::mtake_last<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 20);
        REQUIRE(fxt::get<1>(take3) == 30);
        REQUIRE(fxt::get<2>(take3) == 40);
    }

    SECTION("mtake_last from flat_tuple rvalue") {
        auto result = fxt::mtake_last<3>(fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0));
        REQUIRE(fxt::get<0>(result) == 3.0);
        REQUIRE(fxt::get<2>(result) == 5.0);
    }
}

TEST_CASE("mtake and mtake_last combined with flat_tuple", "[mtake][mtake_last][flat_tuple]") {
    SECTION("extract middle elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);

        // Take first 5, then take last 3 from that
        auto middle = fxt::mtake_last<3>(fxt::mtake<5>(ft));
        REQUIRE(fxt::tuple_size_v<decltype(middle)> == 3);
        REQUIRE(fxt::get<0>(middle) == 3.0);
        REQUIRE(fxt::get<1>(middle) == 4.0);
        REQUIRE(fxt::get<2>(middle) == 5.0);
    }
}

// ============================================================================
// Tests for fxt::take with fxt::tuple (direct call)
// ============================================================================

TEST_CASE("fxt::take with fxt::tuple direct call", "[take][tuple][direct]") {
    SECTION("take from tuple - basic usage") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::take<2>(t);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 1);
        REQUIRE(fxt::get<1>(take2) == 2);
    }

    SECTION("take zero elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto take0 = fxt::take<0>(t);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);
    }

    SECTION("take all elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto take3 = fxt::take<3>(t);
        REQUIRE(fxt::get<0>(take3) == 10);
        REQUIRE(fxt::get<2>(take3) == 30);
    }

    SECTION("take with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), true);
        auto take3 = fxt::take<3>(t);
        REQUIRE(fxt::get<0>(take3) == 42);
        REQUIRE(fxt::get<1>(take3) == 3.14);
        REQUIRE(fxt::get<2>(take3) == "hello");
    }

    SECTION("take from rvalue tuple") {
        auto result = fxt::take<2>(fxt::make_tuple(100, 200, 300));
        REQUIRE(fxt::get<0>(result) == 100);
        REQUIRE(fxt::get<1>(result) == 200);
    }
}

// ============================================================================
// Tests for fxt::take with fxt::tuple (pipe operator)
// ============================================================================

TEST_CASE("fxt::take with fxt::tuple pipe operator", "[take][tuple][pipe]") {
    SECTION("take using pipe operator") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto result = t | fxt::take<2>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
    }

    SECTION("take zero using pipe") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = t | fxt::take<0>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("chaining take operations") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
        auto result = t | fxt::take<6>() | fxt::take<3>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("pipe with rvalue") {
        auto result = fxt::make_tuple(10, 20, 30, 40) | fxt::take<2>();
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
    }
}

// ============================================================================
// Tests for fxt::take_last with fxt::tuple (direct call)
// ============================================================================

TEST_CASE("fxt::take_last with fxt::tuple direct call", "[take_last][tuple][direct]") {
    SECTION("take_last from tuple - basic usage") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::take_last<2>(t);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 4);
        REQUIRE(fxt::get<1>(take2) == 5);
    }

    SECTION("take_last zero elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto take0 = fxt::take_last<0>(t);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);
    }

    SECTION("take_last all elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto take3 = fxt::take_last<3>(t);
        REQUIRE(fxt::get<0>(take3) == 10);
        REQUIRE(fxt::get<2>(take3) == 30);
    }

    SECTION("take_last with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), true);
        auto take2 = fxt::take_last<2>(t);
        REQUIRE(fxt::get<0>(take2) == "hello");
        REQUIRE(fxt::get<1>(take2) == true);
    }

    SECTION("take_last from rvalue tuple") {
        auto result = fxt::take_last<2>(fxt::make_tuple(100, 200, 300));
        REQUIRE(fxt::get<0>(result) == 200);
        REQUIRE(fxt::get<1>(result) == 300);
    }
}

// ============================================================================
// Tests for fxt::take_last with fxt::tuple (pipe operator)
// ============================================================================

TEST_CASE("fxt::take_last with fxt::tuple pipe operator", "[take_last][tuple][pipe]") {
    SECTION("take_last using pipe operator") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto result = t | fxt::take_last<2>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 4);
        REQUIRE(fxt::get<1>(result) == 5);
    }

    SECTION("take_last zero using pipe") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = t | fxt::take_last<0>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("chaining take and take_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
        auto result = t | fxt::take<6>() | fxt::take_last<3>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 6);
    }

    SECTION("pipe with rvalue") {
        auto result = fxt::make_tuple(10, 20, 30, 40) | fxt::take_last<2>();
        REQUIRE(fxt::get<0>(result) == 30);
        REQUIRE(fxt::get<1>(result) == 40);
    }
}

// ============================================================================
// Tests for fxt::take with fxt::flat_tuple (direct call)
// ============================================================================

TEST_CASE("fxt::take with fxt::flat_tuple direct call", "[take][flat_tuple][direct]") {
    SECTION("take from flat_tuple - basic usage") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto take2 = fxt::take<2>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 1.0);
        REQUIRE(fxt::get<1>(take2) == 2.0);
    }

    SECTION("take zero elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto take0 = fxt::take<0>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);
    }

    SECTION("take all elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(10.0, 20.0, 30.0);
        auto take3 = fxt::take<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 10.0);
        REQUIRE(fxt::get<2>(take3) == 30.0);
    }

    SECTION("take from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(100, 200, 300, 400);
        auto take3 = fxt::take<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 100);
        REQUIRE(fxt::get<1>(take3) == 200);
        REQUIRE(fxt::get<2>(take3) == 300);
    }

    SECTION("take from rvalue flat_tuple") {
        auto result = fxt::take<2>(fxt::make_flat_tuple(1.0, 2.0, 3.0));
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
    }
}

// ============================================================================
// Tests for fxt::take with fxt::flat_tuple (pipe operator)
// ============================================================================

TEST_CASE("fxt::take with fxt::flat_tuple pipe operator", "[take][flat_tuple][pipe]") {
    SECTION("take using pipe operator on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto result = ft | fxt::take<2>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
    }

    SECTION("take zero using pipe on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto result = ft | fxt::take<0>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("chaining take operations on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
        auto result = ft | fxt::take<6>() | fxt::take<3>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("pipe with rvalue flat_tuple") {
        auto result = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0) | fxt::take<2>();
        REQUIRE(fxt::get<0>(result) == 10.0);
        REQUIRE(fxt::get<1>(result) == 20.0);
    }
}

// ============================================================================
// Tests for fxt::take_last with fxt::flat_tuple (direct call)
// ============================================================================

TEST_CASE("fxt::take_last with fxt::flat_tuple direct call", "[take_last][flat_tuple][direct]") {
    SECTION("take_last from flat_tuple - basic usage") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto take2 = fxt::take_last<2>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take2)> == 2);
        REQUIRE(fxt::get<0>(take2) == 4.0);
        REQUIRE(fxt::get<1>(take2) == 5.0);
    }

    SECTION("take_last zero elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto take0 = fxt::take_last<0>(ft);
        REQUIRE(fxt::tuple_size_v<decltype(take0)> == 0);
    }

    SECTION("take_last all elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(10.0, 20.0, 30.0);
        auto take3 = fxt::take_last<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 10.0);
        REQUIRE(fxt::get<2>(take3) == 30.0);
    }

    SECTION("take_last from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(100, 200, 300, 400);
        auto take2 = fxt::take_last<2>(ft);
        REQUIRE(fxt::get<0>(take2) == 300);
        REQUIRE(fxt::get<1>(take2) == 400);
    }

    SECTION("take_last from rvalue flat_tuple") {
        auto result = fxt::take_last<2>(fxt::make_flat_tuple(1.0, 2.0, 3.0));
        REQUIRE(fxt::get<0>(result) == 2.0);
        REQUIRE(fxt::get<1>(result) == 3.0);
    }
}

// ============================================================================
// Tests for fxt::take_last with fxt::flat_tuple (pipe operator)
// ============================================================================

TEST_CASE("fxt::take_last with fxt::flat_tuple pipe operator", "[take_last][flat_tuple][pipe]") {
    SECTION("take_last using pipe operator on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto result = ft | fxt::take_last<2>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 4.0);
        REQUIRE(fxt::get<1>(result) == 5.0);
    }

    SECTION("take_last zero using pipe on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto result = ft | fxt::take_last<0>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("chaining take and take_last on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
        auto result = ft | fxt::take<6>() | fxt::take_last<3>();
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 4.0);
        REQUIRE(fxt::get<2>(result) == 6.0);
    }

    SECTION("pipe with rvalue flat_tuple") {
        auto result = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0) | fxt::take_last<2>();
        REQUIRE(fxt::get<0>(result) == 30.0);
        REQUIRE(fxt::get<1>(result) == 40.0);
    }
}

// ============================================================================
// Comparison tests: mtake vs take, mtake_last vs take_last
// ============================================================================

TEST_CASE("Comparison: mtake vs take equivalence", "[take][mtake][comparison]") {
    SECTION("mtake and take produce same results on fxt::tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto mtake_result = fxt::mtake<3>(t);
        auto take_result = fxt::take<3>(t);

        REQUIRE(fxt::get<0>(mtake_result) == fxt::get<0>(take_result));
        REQUIRE(fxt::get<1>(mtake_result) == fxt::get<1>(take_result));
        REQUIRE(fxt::get<2>(mtake_result) == fxt::get<2>(take_result));
    }

    SECTION("mtake and take produce same results on fxt::flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto mtake_result = fxt::mtake<3>(ft);
        auto take_result = fxt::take<3>(ft);

        REQUIRE(fxt::get<0>(mtake_result) == fxt::get<0>(take_result));
        REQUIRE(fxt::get<1>(mtake_result) == fxt::get<1>(take_result));
        REQUIRE(fxt::get<2>(mtake_result) == fxt::get<2>(take_result));
    }
}

TEST_CASE("Comparison: mtake_last vs take_last equivalence", "[take_last][mtake_last][comparison]") {
    SECTION("mtake_last and take_last produce same results on fxt::tuple") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto mtake_last_result = fxt::mtake_last<3>(t);
        auto take_last_result = fxt::take_last<3>(t);

        REQUIRE(fxt::get<0>(mtake_last_result) == fxt::get<0>(take_last_result));
        REQUIRE(fxt::get<1>(mtake_last_result) == fxt::get<1>(take_last_result));
        REQUIRE(fxt::get<2>(mtake_last_result) == fxt::get<2>(take_last_result));
    }

    SECTION("mtake_last and take_last produce same results on fxt::flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);

        auto mtake_last_result = fxt::mtake_last<3>(ft);
        auto take_last_result = fxt::take_last<3>(ft);

        REQUIRE(fxt::get<0>(mtake_last_result) == fxt::get<0>(take_last_result));
        REQUIRE(fxt::get<1>(mtake_last_result) == fxt::get<1>(take_last_result));
        REQUIRE(fxt::get<2>(mtake_last_result) == fxt::get<2>(take_last_result));
    }
}

// ============================================================================
// Complex chaining tests
// ============================================================================

TEST_CASE("Complex chaining with take and take_last", "[take][take_last][chaining]") {
    SECTION("extract middle elements from tuple using pipe") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto middle = t | fxt::take<7>() | fxt::take_last<4>();

        REQUIRE(fxt::tuple_size_v<decltype(middle)> == 4);
        REQUIRE(fxt::get<0>(middle) == 4);
        REQUIRE(fxt::get<1>(middle) == 5);
        REQUIRE(fxt::get<2>(middle) == 6);
        REQUIRE(fxt::get<3>(middle) == 7);
    }

    SECTION("extract middle elements from flat_tuple using pipe") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);
        auto middle = ft | fxt::take<7>() | fxt::take_last<4>();

        REQUIRE(fxt::tuple_size_v<decltype(middle)> == 4);
        REQUIRE(fxt::get<0>(middle) == 4.0);
        REQUIRE(fxt::get<1>(middle) == 5.0);
        REQUIRE(fxt::get<2>(middle) == 6.0);
        REQUIRE(fxt::get<3>(middle) == 7.0);
    }

    SECTION("multiple chaining operations") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto result = t | fxt::take<8>() | fxt::take_last<6>() | fxt::take<4>();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 6);
    }
}
