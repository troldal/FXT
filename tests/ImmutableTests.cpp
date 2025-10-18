//
// Created by kenne on 18/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>
#include <vector>
#include <memory>
#include <type_traits>

using namespace std::string_literals;

// Helper struct for testing
struct TestStruct {
    int value;
    std::string name;

    bool operator==(const TestStruct& other) const {
        return value == other.value && name == other.name;
    }

    auto operator<=>(const TestStruct& other) const = default;
};

TEST_CASE("fxt::immutable basic construction", "[immutable][construction]")
{
    SECTION("Construct from lvalue")
    {
        int x = 42;
        fxt::immutable<int> immut(x);
        REQUIRE(*immut == 42);
    }

    SECTION("Construct from rvalue")
    {
        fxt::immutable<int> immut(42);
        REQUIRE(*immut == 42);
    }

    SECTION("Construct with string")
    {
        fxt::immutable<std::string> immut("hello"s);
        REQUIRE(*immut == "hello");
    }

    SECTION("Construct with vector")
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        fxt::immutable<std::vector<int>> immut(std::move(vec));
        REQUIRE(immut->size() == 5);
        REQUIRE((*immut)[0] == 1);
    }

    SECTION("Construct with custom struct")
    {
        TestStruct ts{42, "test"};
        fxt::immutable<TestStruct> immut(std::move(ts));
        REQUIRE(immut->value == 42);
        REQUIRE(immut->name == "test");
    }
}

TEST_CASE("fxt::immutable copy semantics", "[immutable][copy]")
{
    SECTION("Copy constructor")
    {
        fxt::immutable<int> immut1(42);
        fxt::immutable<int> immut2(immut1);

        REQUIRE(*immut1 == 42);
        REQUIRE(*immut2 == 42);
    }

    SECTION("Copy assignment")
    {
        fxt::immutable<int> immut1(42);
        fxt::immutable<int> immut2(100);

        immut2 = immut1;

        REQUIRE(*immut1 == 42);
        REQUIRE(*immut2 == 42);
    }

    SECTION("Copy with string")
    {
        fxt::immutable<std::string> immut1("hello"s);
        fxt::immutable<std::string> immut2 = immut1;

        REQUIRE(*immut1 == "hello");
        REQUIRE(*immut2 == "hello");
    }
}

TEST_CASE("fxt::immutable move semantics", "[immutable][move]")
{
    SECTION("Move constructor")
    {
        fxt::immutable<std::vector<int>> immut1({1, 2, 3, 4, 5});
        fxt::immutable<std::vector<int>> immut2(std::move(immut1));

        REQUIRE(immut2->size() == 5);
        REQUIRE((*immut2)[0] == 1);
    }

    SECTION("Move assignment")
    {
        fxt::immutable<std::vector<int>> immut1({1, 2, 3, 4, 5});
        fxt::immutable<std::vector<int>> immut2({10, 20});

        immut2 = std::move(immut1);

        REQUIRE(immut2->size() == 5);
        REQUIRE((*immut2)[0] == 1);
    }

    SECTION("Move with unique_ptr")
    {
        auto ptr = std::make_unique<int>(42);
        fxt::immutable<std::unique_ptr<int>> immut1(std::move(ptr));

        fxt::immutable<std::unique_ptr<int>> immut2(std::move(immut1));

        REQUIRE(**immut2 == 42);
    }
}

TEST_CASE("fxt::immutable access operators", "[immutable][access]")
{
    SECTION("Dereference operator with int")
    {
        fxt::immutable<int> immut(42);
        REQUIRE(*immut == 42);
    }

    SECTION("Implicit conversion to const reference")
    {
        fxt::immutable<int> immut(42);
        const int& ref = immut;
        REQUIRE(ref == 42);
    }

    SECTION("Arrow operator with struct")
    {
        fxt::immutable<TestStruct> immut(TestStruct{42, "test"});
        REQUIRE(immut->value == 42);
        REQUIRE(immut->name == "test");
    }

    SECTION("Arrow operator with vector")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3});
        REQUIRE(immut->size() == 3);
        REQUIRE(immut->empty() == false);
    }

    SECTION("Const reference prevents modification")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3});
        const auto& vec = *immut;

        // This would be a compile error:
        // vec.push_back(4);

        REQUIRE(vec.size() == 3);
    }
}

TEST_CASE("fxt::immutable extract method", "[immutable][extract]")
{
    SECTION("Extract from rvalue")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3, 4, 5});
        std::vector<int> extracted = std::move(immut).extract();

        REQUIRE(extracted.size() == 5);
        REQUIRE(extracted[0] == 1);
        REQUIRE(extracted[4] == 5);
    }

    SECTION("Extract with string")
    {
        fxt::immutable<std::string> immut("hello world"s);
        std::string extracted = std::move(immut).extract();

        REQUIRE(extracted == "hello world");
    }

    SECTION("Extract via implicit conversion")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3});
        std::vector<int> extracted = std::move(immut);

        REQUIRE(extracted.size() == 3);
    }

    SECTION("Extract via dereference operator")
    {
        fxt::immutable<std::string> immut("test"s);
        std::string extracted = *std::move(immut);

        REQUIRE(extracted == "test");
    }
}

TEST_CASE("fxt::immutable comparison operators", "[immutable][comparison]")
{
    SECTION("Equality comparison")
    {
        fxt::immutable<int> a(42);
        fxt::immutable<int> b(42);
        fxt::immutable<int> c(100);

        REQUIRE(a == b);
        REQUIRE_FALSE(a == c);
    }

    SECTION("Inequality comparison")
    {
        fxt::immutable<int> a(42);
        fxt::immutable<int> b(100);

        REQUIRE(a != b);
    }

    SECTION("Less than comparison")
    {
        fxt::immutable<int> a(42);
        fxt::immutable<int> b(100);

        REQUIRE(a < b);
        REQUIRE_FALSE(b < a);
    }

    SECTION("Greater than comparison")
    {
        fxt::immutable<int> a(100);
        fxt::immutable<int> b(42);

        REQUIRE(a > b);
        REQUIRE_FALSE(b > a);
    }

    SECTION("Less than or equal")
    {
        fxt::immutable<int> a(42);
        fxt::immutable<int> b(42);
        fxt::immutable<int> c(100);

        REQUIRE(a <= b);
        REQUIRE(a <= c);
        REQUIRE_FALSE(c <= a);
    }

    SECTION("Greater than or equal")
    {
        fxt::immutable<int> a(42);
        fxt::immutable<int> b(42);
        fxt::immutable<int> c(10);

        REQUIRE(a >= b);
        REQUIRE(a >= c);
        REQUIRE_FALSE(c >= a);
    }

    SECTION("String comparison")
    {
        fxt::immutable<std::string> a("apple"s);
        fxt::immutable<std::string> b("banana"s);

        REQUIRE(a < b);
        REQUIRE(b > a);
        REQUIRE(a != b);
    }

    SECTION("Custom struct comparison")
    {
        fxt::immutable<TestStruct> a(TestStruct{42, "test"});
        fxt::immutable<TestStruct> b(TestStruct{42, "test"});
        fxt::immutable<TestStruct> c(TestStruct{100, "other"});

        REQUIRE(a == b);
        REQUIRE(a != c);
        REQUIRE(a < c);
    }
}

TEST_CASE("fxt::immutable reassignment", "[immutable][reassignment]")
{
    SECTION("Reassign with copy")
    {
        fxt::immutable<int> immut(42);
        REQUIRE(*immut == 42);

        immut = fxt::immutable<int>(100);
        REQUIRE(*immut == 100);
    }

    SECTION("Reassign with move")
    {
        fxt::immutable<std::string> immut("hello"s);
        REQUIRE(*immut == "hello");

        immut = fxt::immutable<std::string>("world"s);
        REQUIRE(*immut == "world");
    }

    SECTION("Multiple reassignments")
    {
        fxt::immutable<int> immut(1);

        for (int i = 1; i <= 10; ++i) {
            REQUIRE(*immut == i);
            if (i < 10) {
                immut = fxt::immutable<int>(i + 1);
            }
        }
    }

    SECTION("Reassign from another immutable")
    {
        fxt::immutable<std::vector<int>> immut1({1, 2, 3});
        fxt::immutable<std::vector<int>> immut2({4, 5, 6});

        immut1 = immut2;

        REQUIRE(immut1->size() == 3);
        REQUIRE((*immut1)[0] == 4);
    }
}

TEST_CASE("fxt::immutable with containers", "[immutable][containers]")
{
    SECTION("Store in vector")
    {
        std::vector<fxt::immutable<int>> vec;
        vec.push_back(fxt::immutable<int>(1));
        vec.push_back(fxt::immutable<int>(2));
        vec.push_back(fxt::immutable<int>(3));

        REQUIRE(vec.size() == 3);
        REQUIRE(*vec[0] == 1);
        REQUIRE(*vec[1] == 2);
        REQUIRE(*vec[2] == 3);
    }

    SECTION("Store in map using emplace")
    {
        std::map<std::string, fxt::immutable<int>> map;
        map.emplace("one", fxt::immutable<int>(1));
        map.emplace("two", fxt::immutable<int>(2));
        map.emplace("three", fxt::immutable<int>(3));

        REQUIRE(map.size() == 3);
        REQUIRE(*map.at("one") == 1);
        REQUIRE(*map.at("two") == 2);
        REQUIRE(*map.at("three") == 3);
    }

    SECTION("Sort vector of immutable")
    {
        std::vector<fxt::immutable<int>> vec;
        vec.push_back(fxt::immutable<int>(3));
        vec.push_back(fxt::immutable<int>(1));
        vec.push_back(fxt::immutable<int>(2));

        std::sort(vec.begin(), vec.end());

        REQUIRE(*vec[0] == 1);
        REQUIRE(*vec[1] == 2);
        REQUIRE(*vec[2] == 3);
    }
}

TEST_CASE("fxt::immutable gradual construction pattern", "[immutable][pattern]")
{
    SECTION("Build vector then wrap")
    {
        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
        vec.push_back(3);

        fxt::immutable<std::vector<int>> immut(std::move(vec));

        REQUIRE(immut->size() == 3);
        // vec.push_back(4); would work if not moved
        // immut->push_back(4); compile error - not const
    }

    SECTION("Build struct then wrap")
    {
        TestStruct ts;
        ts.value = 42;
        ts.name = "constructed";

        fxt::immutable<TestStruct> immut(std::move(ts));

        REQUIRE(immut->value == 42);
        REQUIRE(immut->name == "constructed");
    }

    SECTION("Build complex structure then freeze")
    {
        struct Config {
            std::vector<std::string> features;
            std::map<std::string, int> settings;
        };

        Config cfg;
        cfg.features.push_back("feature1");
        cfg.features.push_back("feature2");
        cfg.settings["timeout"] = 30;
        cfg.settings["retries"] = 3;

        fxt::immutable<Config> immut(std::move(cfg));

        REQUIRE(immut->features.size() == 2);
        REQUIRE(immut->settings.size() == 2);
        REQUIRE(immut->settings.at("timeout") == 30);
    }
}

TEST_CASE("fxt::immutable const correctness", "[immutable][const]")
{
    SECTION("Const reference to immutable")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3});
        const auto& immut_ref = immut;

        REQUIRE(immut_ref->size() == 3);
        // immut_ref->push_back(4); compile error
    }

    SECTION("Passing to const reference parameter")
    {
        auto func = [](const fxt::immutable<int>& val) {
            return *val * 2;
        };

        fxt::immutable<int> immut(21);
        int result = func(immut);

        REQUIRE(result == 42);
    }

    SECTION("Arrow operator returns const pointer")
    {
        fxt::immutable<std::vector<int>> immut({1, 2, 3});

        // Can call const methods
        size_t size = immut->size();
        bool empty = immut->empty();

        REQUIRE(size == 3);
        REQUIRE(empty == false);

        // Would be compile error:
        // immut->push_back(4);
        // immut->clear();
    }
}

TEST_CASE("fxt::immutable type traits", "[immutable][traits]")
{
    SECTION("Is copyable when T is copyable")
    {
        REQUIRE(std::is_copy_constructible_v<fxt::immutable<int>>);
        REQUIRE(std::is_copy_assignable_v<fxt::immutable<int>>);
        REQUIRE(std::is_copy_constructible_v<fxt::immutable<std::string>>);
    }

    SECTION("Is movable when T is movable")
    {
        REQUIRE(std::is_move_constructible_v<fxt::immutable<int>>);
        REQUIRE(std::is_move_assignable_v<fxt::immutable<int>>);
        REQUIRE(std::is_move_constructible_v<fxt::immutable<std::unique_ptr<int>>>);
    }

    SECTION("Not default constructible")
    {
        REQUIRE_FALSE(std::is_default_constructible_v<fxt::immutable<int>>);
        REQUIRE_FALSE(std::is_default_constructible_v<fxt::immutable<std::string>>);
    }
}

TEST_CASE("fxt::immutable with move-only types", "[immutable][move-only]")
{
    SECTION("With unique_ptr")
    {
        auto ptr = std::make_unique<int>(42);
        fxt::immutable<std::unique_ptr<int>> immut(std::move(ptr));

        REQUIRE(**immut == 42);

        // Can move the wrapper
        auto immut2 = std::move(immut);
        REQUIRE(**immut2 == 42);
    }

    SECTION("Extract unique_ptr from rvalue")
    {
        auto ptr = std::make_unique<int>(42);
        fxt::immutable<std::unique_ptr<int>> immut(std::move(ptr));

        std::unique_ptr<int> extracted = std::move(immut).extract();

        REQUIRE(*extracted == 42);
    }
}

TEST_CASE("fxt::immutable perfect forwarding", "[immutable][forwarding]")
{
    SECTION("Construct from lvalue avoids copy when possible")
    {
        struct NonCopyable {
            int value;
            NonCopyable(int v) : value(v) {}
            NonCopyable(const NonCopyable&) = delete;
            NonCopyable(NonCopyable&&) = default;
        };

        NonCopyable nc(42);
        fxt::immutable<NonCopyable> immut(std::move(nc));

        REQUIRE(immut->value == 42);
    }

    SECTION("Construct from temporary")
    {
        fxt::immutable<std::string> immut(std::string("temporary"));
        REQUIRE(*immut == "temporary");
    }
}

TEST_CASE("fxt::immutable noexcept specifications", "[immutable][noexcept]")
{
    SECTION("Move operations are conditionally noexcept")
    {
        // int is nothrow movable
        REQUIRE(std::is_nothrow_move_constructible_v<fxt::immutable<int>>);
        REQUIRE(std::is_nothrow_move_assignable_v<fxt::immutable<int>>);
    }

    SECTION("Access operators are noexcept")
    {
        fxt::immutable<int> immut(42);

        REQUIRE(noexcept(*immut));
        REQUIRE(noexcept(immut.operator->()));
    }

    SECTION("Extract is conditionally noexcept")
    {
        fxt::immutable<int> immut(42);
        REQUIRE(noexcept(std::move(immut).extract()));
    }
}
