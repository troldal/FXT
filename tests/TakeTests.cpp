//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

// ============================================================================
// Tests for fxt::take with std::tuple (direct call)
// ============================================================================

TEST_CASE("take with various std::tuple sizes", "[take]") {
    SECTION("take from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::tuple_take<1>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::tuple_take<0>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("take from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto take0 = fxt::tuple_take<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::tuple_take<1>(t);
        REQUIRE(std::get<0>(take1) == 10);

        auto take2 = fxt::tuple_take<2>(t);
        REQUIRE(std::get<0>(take2) == 10);
        REQUIRE(std::get<1>(take2) == 20);
    }

    SECTION("take from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto take0 = fxt::tuple_take<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::tuple_take<1>(t);
        REQUIRE(std::get<0>(take1) == 1);

        auto take2 = fxt::tuple_take<2>(t);
        REQUIRE(std::get<0>(take2) == 1);
        REQUIRE(std::get<1>(take2) == 2.5);

        auto take3 = fxt::tuple_take<3>(t);
        REQUIRE(std::get<0>(take3) == 1);
        REQUIRE(std::get<1>(take3) == 2.5);
        REQUIRE(std::get<2>(take3) == "three");
    }

    SECTION("take from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::tuple_take<2>(t);
        REQUIRE(std::get<0>(take2) == 1);
        REQUIRE(std::get<1>(take2) == 2);

        auto take4 = fxt::tuple_take<4>(t);
        REQUIRE(std::get<0>(take4) == 1);
        REQUIRE(std::get<3>(take4) == 4);
    }

    SECTION("take from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto take5 = fxt::tuple_take<5>(t);
        REQUIRE(std::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(std::get<0>(take5) == 1);
        REQUIRE(std::get<4>(take5) == 5);

        auto take9 = fxt::tuple_take<9>(t);
        REQUIRE(std::get<0>(take9) == 1);
        REQUIRE(std::get<8>(take9) == 9);
    }
}

TEST_CASE("take with different types", "[take]") {
    SECTION("take with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::tuple_take<3>(t);
        REQUIRE(std::get<0>(result) == 42);
        REQUIRE(std::get<1>(result) == 3.14);
        REQUIRE(std::get<2>(result) == "hello");
    }

    SECTION("take with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::tuple_take<3>(t);

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
        auto result = fxt::tuple_take<2>(t);

        REQUIRE(std::get<0>(result) == "first");
        REQUIRE(std::get<1>(result) == "second");
    }
}

TEST_CASE("take with rvalue tuples", "[take]") {
    SECTION("take from rvalue 3-element tuple") {
        auto result = fxt::tuple_take<2>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 100);
        REQUIRE(std::get<1>(result) == 200);
    }

    SECTION("take from rvalue mixed-type tuple") {
        auto result = fxt::tuple_take<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<1>(result) == 2.5);
    }
}

// ============================================================================
// Tests for fxt::take_last with std::tuple (direct call)
// ============================================================================

TEST_CASE("take_last with various std::tuple sizes", "[take_last]") {
    SECTION("take_last from 1-element tuple") {
        auto t = std::make_tuple(42);
        auto result = fxt::tuple_take_last<1>(t);
        REQUIRE(std::get<0>(result) == 42);

        auto empty = fxt::tuple_take_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(empty)> == 0);
    }

    SECTION("take_last from 2-element tuple") {
        auto t = std::make_tuple(10, 20);

        auto take0 = fxt::tuple_take_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::tuple_take_last<1>(t);
        REQUIRE(std::get<0>(take1) == 20);

        auto take2 = fxt::tuple_take_last<2>(t);
        REQUIRE(std::get<0>(take2) == 10);
        REQUIRE(std::get<1>(take2) == 20);
    }

    SECTION("take_last from 3-element tuple") {
        auto t = std::make_tuple(1, 2.5, std::string("three"));

        auto take0 = fxt::tuple_take_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(take0)> == 0);

        auto take1 = fxt::tuple_take_last<1>(t);
        REQUIRE(std::get<0>(take1) == "three");

        auto take2 = fxt::tuple_take_last<2>(t);
        REQUIRE(std::get<0>(take2) == 2.5);
        REQUIRE(std::get<1>(take2) == "three");

        auto take3 = fxt::tuple_take_last<3>(t);
        REQUIRE(std::get<0>(take3) == 1);
        REQUIRE(std::get<1>(take3) == 2.5);
        REQUIRE(std::get<2>(take3) == "three");
    }

    SECTION("take_last from 5-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::tuple_take_last<2>(t);
        REQUIRE(std::get<0>(take2) == 4);
        REQUIRE(std::get<1>(take2) == 5);

        auto take4 = fxt::tuple_take_last<4>(t);
        REQUIRE(std::get<0>(take4) == 2);
        REQUIRE(std::get<3>(take4) == 5);
    }

    SECTION("take_last from 10-element tuple") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        auto take5 = fxt::tuple_take_last<5>(t);
        REQUIRE(std::tuple_size_v<decltype(take5)> == 5);
        REQUIRE(std::get<0>(take5) == 6);
        REQUIRE(std::get<4>(take5) == 10);

        auto take9 = fxt::tuple_take_last<9>(t);
        REQUIRE(std::get<0>(take9) == 2);
        REQUIRE(std::get<8>(take9) == 10);
    }
}

TEST_CASE("take_last with different types", "[take_last]") {
    SECTION("take_last with mixed types") {
        auto t = std::make_tuple(42, 3.14, std::string("hello"), 'c', true);

        auto result = fxt::tuple_take_last<3>(t);
        REQUIRE(std::get<0>(result) == "hello");
        REQUIRE(std::get<1>(result) == 'c');
        REQUIRE(std::get<2>(result) == true);
    }

    SECTION("take_last with all same type") {
        auto t = std::make_tuple(10, 20, 30, 40, 50);
        auto result = fxt::tuple_take_last<3>(t);

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
        auto result = fxt::tuple_take_last<2>(t);

        REQUIRE(std::get<0>(result) == "third");
        REQUIRE(std::get<1>(result) == "fourth");
    }
}

TEST_CASE("take_last with rvalue tuples", "[take_last]") {
    SECTION("take_last from rvalue 3-element tuple") {
        auto result = fxt::tuple_take_last<2>(std::make_tuple(100, 200, 300));
        REQUIRE(std::get<0>(result) == 200);
        REQUIRE(std::get<1>(result) == 300);
    }

    SECTION("take_last from rvalue mixed-type tuple") {
        auto result = fxt::tuple_take_last<2>(std::make_tuple(1, 2.5, std::string("test"), 'x'));
        REQUIRE(std::get<0>(result) == "test");
        REQUIRE(std::get<1>(result) == 'x');
    }
}

// ============================================================================
// Tests for take and take_last with std::tuple - edge cases and combinations
// ============================================================================

TEST_CASE("take and take_last edge cases", "[take][take_last]") {
    SECTION("take 0 elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::tuple_take<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("take_last 0 elements returns empty tuple") {
        auto t = std::make_tuple(1, 2, 3);
        auto result = fxt::tuple_take_last<0>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("take all elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::tuple_take<4>(t);
        REQUIRE(std::tuple_size_v<decltype(result)> == 4);
        REQUIRE(std::get<0>(result) == 1);
        REQUIRE(std::get<3>(result) == 4);
    }

    SECTION("take_last all elements returns same tuple") {
        auto t = std::make_tuple(1, 2, 3, 4);
        auto result = fxt::tuple_take_last<4>(t);
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
        auto result = fxt::tuple_take<2>(std::move(t));
        REQUIRE(*std::get<0>(result) == 42);
        REQUIRE(*std::get<1>(result) == 100);
    }

    SECTION("take_last with unique_ptr") {
        auto t = std::make_tuple(
            std::make_unique<int>(42),
            std::make_unique<int>(100),
            std::make_unique<int>(200)
        );
        auto result = fxt::tuple_take_last<2>(std::move(t));
        REQUIRE(*std::get<0>(result) == 100);
        REQUIRE(*std::get<1>(result) == 200);
    }
}

TEST_CASE("take and take_last combined", "[take][take_last]") {
    SECTION("take then take_last") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto first_take = fxt::tuple_take<7>(t);  // {1, 2, 3, 4, 5, 6, 7}
        auto both_taken = fxt::tuple_take_last<5>(first_take);  // {3, 4, 5, 6, 7}

        REQUIRE(std::tuple_size_v<decltype(both_taken)> == 5);
        REQUIRE(std::get<0>(both_taken) == 3);
        REQUIRE(std::get<1>(both_taken) == 4);
        REQUIRE(std::get<2>(both_taken) == 5);
        REQUIRE(std::get<3>(both_taken) == 6);
        REQUIRE(std::get<4>(both_taken) == 7);
    }

    SECTION("take_last then take") {
        auto t = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
        auto last_take = fxt::tuple_take_last<7>(t);  // {4, 5, 6, 7, 8, 9, 10}
        auto both_taken = fxt::tuple_take<5>(last_take);  // {4, 5, 6, 7, 8}

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
        auto middle = fxt::tuple_take_last<3>(fxt::tuple_take<5>(t));  // {3, 4, 5}

        REQUIRE(std::tuple_size_v<decltype(middle)> == 3);
        REQUIRE(std::get<0>(middle) == 3);
        REQUIRE(std::get<1>(middle) == 4);
        REQUIRE(std::get<2>(middle) == 5);
    }
}

// ============================================================================
// Tests for fxt::take with fxt::tuple (direct call)
// ============================================================================

TEST_CASE("fxt::take with fxt::tuple direct call", "[take][tuple][direct]") {
    SECTION("take from tuple - basic usage") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5);

        auto take2 = fxt::tuple_take<2>(t);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take2)>> == 2);
        REQUIRE(fxt::get<0>(take2) == 1);
        REQUIRE(fxt::get<1>(take2) == 2);
    }

    SECTION("take zero elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto take0 = fxt::tuple_take<0>(t);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take0)>> == 0);
    }

    SECTION("take all elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto take3 = fxt::tuple_take<3>(t);
        REQUIRE(fxt::get<0>(take3) == 10);
        REQUIRE(fxt::get<2>(take3) == 30);
    }

    SECTION("take with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), true);
        auto take3 = fxt::tuple_take<3>(t);
        REQUIRE(fxt::get<0>(take3) == 42);
        REQUIRE(fxt::get<1>(take3) == 3.14);
        REQUIRE(fxt::get<2>(take3) == "hello");
    }

    SECTION("take from rvalue tuple") {
        auto result = fxt::tuple_take<2>(fxt::make_tuple(100, 200, 300));
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

        auto result = t | fxt::tuple_take<2>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 2);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
    }

    SECTION("take zero using pipe") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = t | fxt::tuple_take<0>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 0);
    }

    SECTION("chaining take operations") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
        auto result = t | fxt::tuple_take<6>() | fxt::tuple_take<3>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("pipe with rvalue") {
        auto result = fxt::make_tuple(10, 20, 30, 40) | fxt::tuple_take<2>();
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

        auto take2 = fxt::tuple_take_last<2>(t);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take2)>> == 2);
        REQUIRE(fxt::get<0>(take2) == 4);
        REQUIRE(fxt::get<1>(take2) == 5);
    }

    SECTION("take_last zero elements") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto take0 = fxt::tuple_take_last<0>(t);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take0)>> == 0);
    }

    SECTION("take_last all elements") {
        auto t = fxt::make_tuple(10, 20, 30);
        auto take3 = fxt::tuple_take_last<3>(t);
        REQUIRE(fxt::get<0>(take3) == 10);
        REQUIRE(fxt::get<2>(take3) == 30);
    }

    SECTION("take_last with mixed types") {
        auto t = fxt::make_tuple(42, 3.14, std::string("hello"), true);
        auto take2 = fxt::tuple_take_last<2>(t);
        REQUIRE(fxt::get<0>(take2) == "hello");
        REQUIRE(fxt::get<1>(take2) == true);
    }

    SECTION("take_last from rvalue tuple") {
        auto result = fxt::tuple_take_last<2>(fxt::make_tuple(100, 200, 300));
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

        auto result = t | fxt::tuple_take_last<2>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 2);
        REQUIRE(fxt::get<0>(result) == 4);
        REQUIRE(fxt::get<1>(result) == 5);
    }

    SECTION("take_last zero using pipe") {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = t | fxt::tuple_take_last<0>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 0);
    }

    SECTION("chaining take and take_last") {
        auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
        auto result = t | fxt::tuple_take<6>() | fxt::tuple_take_last<3>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 3);
        REQUIRE(fxt::get<0>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 6);
    }

    SECTION("pipe with rvalue") {
        auto result = fxt::make_tuple(10, 20, 30, 40) | fxt::tuple_take_last<2>();
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

        auto take2 = fxt::tuple_take<2>(ft);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take2)>> == 2);
        REQUIRE(fxt::get<0>(take2) == 1.0);
        REQUIRE(fxt::get<1>(take2) == 2.0);
    }

    SECTION("take zero elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto take0 = fxt::tuple_take<0>(ft);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take0)>> == 0);
    }

    SECTION("take all elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(10.0, 20.0, 30.0);
        auto take3 = fxt::tuple_take<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 10.0);
        REQUIRE(fxt::get<2>(take3) == 30.0);
    }

    SECTION("take from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(100, 200, 300, 400);
        auto take3 = fxt::tuple_take<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 100);
        REQUIRE(fxt::get<1>(take3) == 200);
        REQUIRE(fxt::get<2>(take3) == 300);
    }

    SECTION("take from rvalue flat_tuple") {
        auto result = fxt::tuple_take<2>(fxt::make_flat_tuple(1.0, 2.0, 3.0));
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

        auto result = ft | fxt::tuple_take<2>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 2);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
    }

    SECTION("take zero using pipe on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto result = ft | fxt::tuple_take<0>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 0);
    }

    SECTION("chaining take operations on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
        auto result = ft | fxt::tuple_take<6>() | fxt::tuple_take<3>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("pipe with rvalue flat_tuple") {
        auto result = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0) | fxt::tuple_take<2>();
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

        auto take2 = fxt::tuple_take_last<2>(ft);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take2)>> == 2);
        REQUIRE(fxt::get<0>(take2) == 4.0);
        REQUIRE(fxt::get<1>(take2) == 5.0);
    }

    SECTION("take_last zero elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto take0 = fxt::tuple_take_last<0>(ft);
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(take0)>> == 0);
    }

    SECTION("take_last all elements from flat_tuple") {
        auto ft = fxt::make_flat_tuple(10.0, 20.0, 30.0);
        auto take3 = fxt::tuple_take_last<3>(ft);
        REQUIRE(fxt::get<0>(take3) == 10.0);
        REQUIRE(fxt::get<2>(take3) == 30.0);
    }

    SECTION("take_last from flat_tuple with integers") {
        auto ft = fxt::make_flat_tuple(100, 200, 300, 400);
        auto take2 = fxt::tuple_take_last<2>(ft);
        REQUIRE(fxt::get<0>(take2) == 300);
        REQUIRE(fxt::get<1>(take2) == 400);
    }

    SECTION("take_last from rvalue flat_tuple") {
        auto result = fxt::tuple_take_last<2>(fxt::make_flat_tuple(1.0, 2.0, 3.0));
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

        auto result = ft | fxt::tuple_take_last<2>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 2);
        REQUIRE(fxt::get<0>(result) == 4.0);
        REQUIRE(fxt::get<1>(result) == 5.0);
    }

    SECTION("take_last zero using pipe on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto result = ft | fxt::tuple_take_last<0>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 0);
    }

    SECTION("chaining take and take_last on flat_tuple") {
        auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
        auto result = ft | fxt::tuple_take<6>() | fxt::tuple_take_last<3>();
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(result)>> == 3);
        REQUIRE(fxt::get<0>(result) == 4.0);
        REQUIRE(fxt::get<2>(result) == 6.0);
    }

    SECTION("pipe with rvalue flat_tuple") {
        auto result = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0) | fxt::tuple_take_last<2>();
        REQUIRE(fxt::get<0>(result) == 30.0);
        REQUIRE(fxt::get<1>(result) == 40.0);
    }
}

// ============================================================================
// Tests for fxt::mtake with fxt::expected<fxt::tuple> (direct call)
// ============================================================================

TEST_CASE("fxt::mtake with fxt::expected<fxt::tuple> direct call", "[mtake][expected][tuple][direct]") {
    SECTION("mtake from expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::make_tuple(10, 20, 30, 40, 50)
        };

        auto result = fxt::mtuple_take<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10);
        REQUIRE(fxt::get<1>(*result) == 20);
        REQUIRE(fxt::get<2>(*result) == 30);
    }

    SECTION("mtake from expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Error occurred")}
        };

        auto result = fxt::mtuple_take<2>(exp);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error occurred");
    }

    SECTION("mtake zero elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3)
        };

        auto result = fxt::mtuple_take<0>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 0);
    }

    SECTION("mtake all elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{
            fxt::make_tuple(100, 200, 300)
        };

        auto result = fxt::mtuple_take<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 100);
        REQUIRE(fxt::get<1>(*result) == 200);
        REQUIRE(fxt::get<2>(*result) == 300);
    }

    SECTION("mtake with mixed types in expected") {
        auto exp = fxt::expected<fxt::tuple<int, double, std::string, bool>, std::string>{
            fxt::make_tuple(42, 3.14, std::string("hello"), true)
        };

        auto result = fxt::mtuple_take<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 42);
        REQUIRE(fxt::get<1>(*result) == 3.14);
        REQUIRE(fxt::get<2>(*result) == "hello");
    }
}

// ============================================================================
// Tests for fxt::mtake with fxt::expected<fxt::tuple> (pipe operator)
// ============================================================================

TEST_CASE("fxt::mtake with fxt::expected<fxt::tuple> pipe operator", "[mtake][expected][tuple][pipe]") {
    SECTION("mtake using pipe operator with success") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5)
        };

        auto result = exp | fxt::mtuple_take<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 1);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 3);
    }

    SECTION("mtake using pipe operator with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Pipeline error")}
        };

        auto result = exp | fxt::mtuple_take<2>();
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Pipeline error");
    }

    SECTION("chaining mtake operations") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };

        auto result = exp | fxt::mtuple_take<5>() | fxt::mtuple_take<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 3);
        REQUIRE(fxt::get<0>(*result) == 1);
        REQUIRE(fxt::get<2>(*result) == 3);
    }

    SECTION("error propagates through chain") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Initial error")}
        };

        auto result = exp | fxt::mtuple_take<3>() | fxt::mtuple_take<2>();
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Initial error");
    }
}

// ============================================================================
// Tests for fxt::mtake_last with fxt::expected<fxt::tuple> (direct call)
// ============================================================================

TEST_CASE("fxt::mtake_last with fxt::expected<fxt::tuple> direct call", "[mtake_last][expected][tuple][direct]") {
    SECTION("mtake_last from expected with value") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::make_tuple(10, 20, 30, 40, 50)
        };

        auto result = fxt::mtuple_take_last<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 30);
        REQUIRE(fxt::get<1>(*result) == 40);
        REQUIRE(fxt::get<2>(*result) == 50);
    }

    SECTION("mtake_last from expected with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Error occurred")}
        };

        auto result = fxt::mtuple_take_last<2>(exp);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Error occurred");
    }

    SECTION("mtake_last zero elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3)
        };

        auto result = fxt::mtuple_take_last<0>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 0);
    }

    SECTION("mtake_last all elements from expected") {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{
            fxt::make_tuple(100, 200, 300)
        };

        auto result = fxt::mtuple_take_last<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 100);
        REQUIRE(fxt::get<1>(*result) == 200);
        REQUIRE(fxt::get<2>(*result) == 300);
    }
}

// ============================================================================
// Tests for fxt::mtake_last with fxt::expected<fxt::tuple> (pipe operator)
// ============================================================================

TEST_CASE("fxt::mtake_last with fxt::expected<fxt::tuple> pipe operator", "[mtake_last][expected][tuple][pipe]") {
    SECTION("mtake_last using pipe operator with success") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5)
        };

        auto result = exp | fxt::mtuple_take_last<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 4);
        REQUIRE(fxt::get<2>(*result) == 5);
    }

    SECTION("mtake_last using pipe operator with error") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Pipeline error")}
        };

        auto result = exp | fxt::mtuple_take_last<2>();
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Pipeline error");
    }

    SECTION("chaining mtake and mtake_last") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8)
        };

        auto result = exp | fxt::mtuple_take<6>() | fxt::mtuple_take_last<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 3);
        REQUIRE(fxt::get<0>(*result) == 4);
        REQUIRE(fxt::get<1>(*result) == 5);
        REQUIRE(fxt::get<2>(*result) == 6);
    }
}

// ============================================================================
// Tests for fxt::mtake with fxt::expected<fxt::flat_tuple>
// ============================================================================

TEST_CASE("fxt::mtake with fxt::expected<fxt::flat_tuple>", "[mtake][expected][flat_tuple]") {
    SECTION("mtake from expected flat_tuple with value") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double, double, double>, std::string>{
            fxt::make_flat_tuple(1.1, 2.2, 3.3, 4.4, 5.5)
        };

        auto result = fxt::mtuple_take<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 1.1);
        REQUIRE(fxt::get<1>(*result) == 2.2);
        REQUIRE(fxt::get<2>(*result) == 3.3);
    }

    SECTION("mtake from expected flat_tuple with error") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double>, std::string>{
            fxt::unexpected{std::string("Calculation error")}
        };

        auto result = fxt::mtuple_take<2>(exp);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Calculation error");
    }

    SECTION("mtake using pipe on expected flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int, int, int>, std::string>{
            fxt::make_flat_tuple(10, 20, 30, 40, 50)
        };

        auto result = exp | fxt::mtuple_take<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10);
        REQUIRE(fxt::get<1>(*result) == 20);
        REQUIRE(fxt::get<2>(*result) == 30);
    }
}

// ============================================================================
// Tests for fxt::mtake_last with fxt::expected<fxt::flat_tuple>
// ============================================================================

TEST_CASE("fxt::mtake_last with fxt::expected<fxt::flat_tuple>", "[mtake_last][expected][flat_tuple]") {
    SECTION("mtake_last from expected flat_tuple with value") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double, double, double>, std::string>{
            fxt::make_flat_tuple(1.1, 2.2, 3.3, 4.4, 5.5)
        };

        auto result = fxt::mtuple_take_last<3>(exp);
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3.3);
        REQUIRE(fxt::get<1>(*result) == 4.4);
        REQUIRE(fxt::get<2>(*result) == 5.5);
    }

    SECTION("mtake_last from expected flat_tuple with error") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double>, std::string>{
            fxt::unexpected{std::string("Calculation error")}
        };

        auto result = fxt::mtuple_take_last<2>(exp);
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Calculation error");
    }

    SECTION("mtake_last using pipe on expected flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int, int, int>, std::string>{
            fxt::make_flat_tuple(10, 20, 30, 40, 50)
        };

        auto result = exp | fxt::mtuple_take_last<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 30);
        REQUIRE(fxt::get<1>(*result) == 40);
        REQUIRE(fxt::get<2>(*result) == 50);
    }

    SECTION("chaining mtake and mtake_last on expected flat_tuple") {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double, double, double, double, double>, std::string>{
            fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0)
        };

        auto result = exp | fxt::mtuple_take<5>() | fxt::mtuple_take_last<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 3);
        REQUIRE(fxt::get<0>(*result) == 3.0);
        REQUIRE(fxt::get<1>(*result) == 4.0);
        REQUIRE(fxt::get<2>(*result) == 5.0);
    }
}

// ============================================================================
// Combined tests: mtake and mtake_last complex scenarios
// ============================================================================

TEST_CASE("Complex monadic scenarios with mtake and mtake_last", "[mtake][mtake_last][expected][complex]") {
    SECTION("extracting middle elements from expected tuple") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7)
        };

        // Take first 5, then take last 3 from that
        auto result = exp | fxt::mtuple_take<5>() | fxt::mtuple_take_last<3>();
        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 4);
        REQUIRE(fxt::get<2>(*result) == 5);
    }

    SECTION("error in middle of complex chain") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
            fxt::unexpected{std::string("Early error")}
        };

        auto result = exp | fxt::mtuple_take<4>() | fxt::mtuple_take_last<2>() | fxt::mtuple_take<1>();
        REQUIRE(!result.has_value());
        REQUIRE(result.error() == "Early error");
    }

    SECTION("multiple transformations preserving type") {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int, int, int, int, int, int, int>, std::string>{
            fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10)
        };

        auto result = exp
            | fxt::mtuple_take<8>()           // {1,2,3,4,5,6,7,8}
            | fxt::mtuple_take_last<6>()      // {3,4,5,6,7,8}
            | fxt::mtuple_take<4>()           // {3,4,5,6}
            | fxt::mtuple_take_last<2>();     // {5,6}

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_reference_t<decltype(*result)>> == 2);
        REQUIRE(fxt::get<0>(*result) == 5);
        REQUIRE(fxt::get<1>(*result) == 6);
    }
}
