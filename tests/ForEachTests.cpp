#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <deque>
#include <list>
#include <string>
#include <vector>

// ============================================================================
// Helpers
// ============================================================================

struct Counter
{
    int value = 0;
    void increment() { ++value; }
    void add(int n)  { value += n; }
};

// ============================================================================
// Pipe-operator usage
// ============================================================================

TEST_CASE("fxt::for_each pipe — visits every element", "[for_each]")
{
    SECTION("vector<int>")
    {
        std::vector<int> visited;
        std::vector<int> input = {1, 2, 3, 4, 5};

        input | fxt::for_each([&visited](int x) { visited.push_back(x); });

        REQUIRE(visited == std::vector<int>{1, 2, 3, 4, 5});
    }

    SECTION("list<int>")
    {
        std::list<int> visited;
        std::list<int> input = {10, 20, 30};

        input | fxt::for_each([&visited](int x) { visited.push_back(x); });

        REQUIRE(visited == std::list<int>{10, 20, 30});
    }

    SECTION("deque<string>")
    {
        std::vector<std::string> visited;
        std::deque<std::string> input = {"a", "b", "c"};

        input | fxt::for_each([&visited](const std::string& s) { visited.push_back(s); });

        REQUIRE(visited == std::vector<std::string>{"a", "b", "c"});
    }
}

TEST_CASE("fxt::for_each pipe — empty range visits nothing", "[for_each]")
{
    int call_count = 0;
    std::vector<int> empty;

    empty | fxt::for_each([&call_count](int) { ++call_count; });

    REQUIRE(call_count == 0);
}

TEST_CASE("fxt::for_each pipe — can mutate elements in-place", "[for_each]")
{
    std::vector<int> v = {1, 2, 3};

    v | fxt::for_each([](int& x) { x *= 10; });

    REQUIRE(v == std::vector<int>{10, 20, 30});
}

TEST_CASE("fxt::for_each pipe — returns the range by value (copy)", "[for_each]")
{
    std::vector<int> input = {1, 2, 3};
    std::vector<int> visited;

    auto result = input | fxt::for_each([&visited](int x) { visited.push_back(x); });

    STATIC_REQUIRE(std::is_same_v<decltype(result), std::vector<int>>);
    REQUIRE(result == std::vector<int>{1, 2, 3});
    REQUIRE(visited == std::vector<int>{1, 2, 3});
}

TEST_CASE("fxt::for_each pipe — can chain multiple adaptors", "[for_each]")
{
    std::vector<int> log1, log2;
    std::vector<int> input = {5, 10, 15};

    auto result = input
        | fxt::for_each([&log1](int x) { log1.push_back(x); })
        | fxt::for_each([&log2](int x) { log2.push_back(x * 2); });

    REQUIRE(log1 == std::vector<int>{5, 10, 15});
    REQUIRE(log2 == std::vector<int>{10, 20, 30});
    REQUIRE(result == std::vector<int>{5, 10, 15});
}

// ============================================================================
// Direct-call usage — value category
// ============================================================================

TEST_CASE("fxt::for_each direct call — lvalue returns lvalue reference", "[for_each]")
{
    std::vector<int> v = {1, 2, 3};
    auto adaptor = fxt::for_each([](int) {});

    decltype(auto) ref = adaptor(v);

    STATIC_REQUIRE(std::is_lvalue_reference_v<decltype(ref)>);
    REQUIRE(&ref == &v);
}

TEST_CASE("fxt::for_each direct call — rvalue returns a value (no dangling)", "[for_each]")
{
    std::vector<int> visited;
    auto adaptor = fxt::for_each([&visited](int x) { visited.push_back(x); });

    auto result = adaptor(std::vector<int>{7, 8, 9});

    STATIC_REQUIRE(std::is_same_v<decltype(result), std::vector<int>>);
    REQUIRE(result == std::vector<int>{7, 8, 9});
    REQUIRE(visited == std::vector<int>{7, 8, 9});
}

TEST_CASE("fxt::for_each direct call — lvalue reference chains without copying", "[for_each]")
{
    std::vector<int> v = {2, 4, 6};

    decltype(auto) ref = fxt::for_each([](int& x) { x += 1; })(v);

    REQUIRE(ref == std::vector<int>{3, 5, 7});
    REQUIRE(v    == std::vector<int>{3, 5, 7});
    REQUIRE(&ref == &v);
}

// ============================================================================
// std::invoke support — member function pointers
// ============================================================================

TEST_CASE("fxt::for_each — member function pointer via std::invoke", "[for_each]")
{
    SECTION("nullary member function — increment")
    {
        std::vector<Counter> counters(3);

        counters | fxt::for_each(&Counter::increment);

        for (auto& c : counters) {
            REQUIRE(c.value == 1);
        }
    }

    SECTION("member function pointer applied twice — check result of the chain")
    {
        std::vector<Counter> counters(2);

        // operator| returns a copy each time, so the second increment runs on
        // the copy produced by the first step. Check the final result, not counters.
        auto result = counters
            | fxt::for_each(&Counter::increment)
            | fxt::for_each(&Counter::increment);

        for (auto& c : result) {
            REQUIRE(c.value == 2);
        }
        // counters itself only had increment called once (in the first step)
        for (auto& c : counters) {
            REQUIRE(c.value == 1);
        }
    }
}

// ============================================================================
// Stored adaptor is reusable
// ============================================================================

TEST_CASE("fxt::for_each — stored adaptor is reusable", "[for_each]")
{
    int total = 0;
    auto accumulate = fxt::for_each([&total](int x) { total += x; });

    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10, 20};

    v1 | accumulate;
    v2 | accumulate;

    REQUIRE(total == 36);  // 1+2+3 + 10+20
}

// ============================================================================
// Type-deduction sanity checks
// ============================================================================

TEST_CASE("fxt::for_each — output container type matches input", "[for_each]")
{
    SECTION("vector in -> vector out")
    {
        std::vector<int> input = {1, 2, 3};
        auto result = input | fxt::for_each([](int) {});
        STATIC_REQUIRE(std::is_same_v<decltype(result), std::vector<int>>);
    }

    SECTION("list in -> list out")
    {
        std::list<int> input = {1, 2};
        auto result = input | fxt::for_each([](int) {});
        STATIC_REQUIRE(std::is_same_v<decltype(result), std::list<int>>);
    }

    SECTION("deque in -> deque out")
    {
        std::deque<double> input = {1.0, 2.0};
        auto result = input | fxt::for_each([](double) {});
        STATIC_REQUIRE(std::is_same_v<decltype(result), std::deque<double>>);
    }
}
