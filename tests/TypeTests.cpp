//
// Created by AI Assistant on 07/10/2025.
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Test Primary Template (Type Tag)
// ============================================================================

TEST_CASE("Type - primary template default construction", "[type]")
{
    SECTION("default construct Type<int>")
    {
        fxt::type_value<int> type_tag;
        using ExtractedType = typename decltype(type_tag)::type_t;
        REQUIRE(std::is_same_v<ExtractedType, int>);
    }

    SECTION("default construct Type<std::string>")
    {
        fxt::type_value<std::string> type_tag;
        using ExtractedType = typename decltype(type_tag)::type_t;
        REQUIRE(std::is_same_v<ExtractedType, std::string>);
    }

    SECTION("default construct Type<double>")
    {
        fxt::type_value<double> type_tag;
        using ExtractedType = typename decltype(type_tag)::type_t;
        REQUIRE(std::is_same_v<ExtractedType, double>);
    }
}

TEST_CASE("Type - primary template implicit conversion", "[type]")
{
    SECTION("implicit conversion from int")
    {
        auto accept_int_tag = [](fxt::type_value<int>) { return true; };
        REQUIRE(accept_int_tag(42));
    }

    SECTION("implicit conversion from string")
    {
        auto accept_string_tag = [](fxt::type_value<std::string>) { return true; };
        REQUIRE(accept_string_tag(std::string("hello")));
    }

    SECTION("implicit conversion from lvalue")
    {
        int value = 100;
        auto accept_int_tag = [](fxt::type_value<int>) { return true; };
        REQUIRE(accept_int_tag(value));
    }

    SECTION("implicit conversion from rvalue")
    {
        auto accept_int_tag = [](fxt::type_value<int>) { return true; };
        REQUIRE(accept_int_tag(std::move(42)));
    }
}

TEST_CASE("Type - primary template copy and move semantics", "[type]")
{
    SECTION("copy construction")
    {
        fxt::type_value<int> tag1;
        fxt::type_value<int> tag2 = tag1;
        using T1 = typename decltype(tag1)::type_t;
        using T2 = typename decltype(tag2)::type_t;
        REQUIRE(std::is_same_v<T1, T2>);
    }

    SECTION("move construction")
    {
        fxt::type_value<int> tag1;
        fxt::type_value<int> tag2 = std::move(tag1);
        using T2 = typename decltype(tag2)::type_t;
        REQUIRE(std::is_same_v<T2, int>);
    }

    SECTION("copy assignment")
    {
        fxt::type_value<int> tag1;
        fxt::type_value<int> tag2;
        tag2 = tag1;
        using T2 = typename decltype(tag2)::type_t;
        REQUIRE(std::is_same_v<T2, int>);
    }

    SECTION("move assignment")
    {
        fxt::type_value<int> tag1;
        fxt::type_value<int> tag2;
        tag2 = std::move(tag1);
        using T2 = typename decltype(tag2)::type_t;
        REQUIRE(std::is_same_v<T2, int>);
    }
}

TEST_CASE("Type - primary template constexpr support", "[type]")
{
    SECTION("constexpr construction")
    {
        constexpr fxt::type_value<int> tag;
        static_assert(std::is_same_v<typename decltype(tag)::type_t, int>);
    }

    SECTION("constexpr with implicit conversion")
    {
        constexpr auto test = []() {
            fxt::type_value<int> tag(42);
            return true;
        };
        static_assert(test());
    }
}

// ============================================================================
// Test Specialization (Type with Value)
// ============================================================================

TEST_CASE("Type - specialization construction with value", "[type]")
{
    SECTION("construct with int value")
    {
        fxt::type_value<std::string, int> tagged(42);
        REQUIRE(tagged.value() == 42);
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, std::string>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, int>);
    }

    SECTION("construct with string value")
    {
        fxt::type_value<int, std::string> tagged(std::string("hello"));
        REQUIRE(tagged.value() == "hello");
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, int>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, std::string>);
    }

    SECTION("construct with double value")
    {
        fxt::type_value<char, double> tagged(3.14159);
        REQUIRE(tagged.value() == Catch::Approx(3.14159));
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, char>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, double>);
    }

    SECTION("construct with rvalue")
    {
        fxt::type_value<int, std::string> tagged(std::string("temp"));
        REQUIRE(tagged.value() == "temp");
    }
}

TEST_CASE("Type - specialization value access", "[type]")
{
    SECTION("value() on const lvalue returns copy")
    {
        const fxt::type_value<std::string, int> tagged(100);
        int val = tagged.value();
        REQUIRE(val == 100);
    }

    SECTION("value() on lvalue returns reference")
    {
        fxt::type_value<std::string, int> tagged(200);
        int& val = tagged.value();
        val = 300;
        REQUIRE(tagged.value() == 300);
    }

    SECTION("value() on rvalue moves value")
    {
        fxt::type_value<std::string, std::string> tagged(std::string("movable"));
        std::string moved = std::move(tagged).value();
        REQUIRE(moved == "movable");
    }

    SECTION("get() returns reference")
    {
        fxt::type_value<std::string, int> tagged(42);
        const int& ref = tagged.get();
        REQUIRE(ref == 42);
        REQUIRE(&ref == &tagged.get()); // Same address
    }

    SECTION("get() allows modification")
    {
        fxt::type_value<std::string, int> tagged(10);
        tagged.get() = 20;
        REQUIRE(tagged.value() == 20);
    }
}

TEST_CASE("Type - specialization copy and move semantics", "[type]")
{
    SECTION("copy construction")
    {
        fxt::type_value<std::string, int> original(42);
        fxt::type_value<std::string, int> copy = original;
        REQUIRE(copy.value() == 42);
        REQUIRE(original.value() == 42);
    }

    SECTION("move construction")
    {
        fxt::type_value<std::string, std::string> original(std::string("hello"));
        fxt::type_value<std::string, std::string> moved = std::move(original);
        REQUIRE(moved.value() == "hello");
    }

    SECTION("copy assignment")
    {
        fxt::type_value<std::string, int> original(42);
        fxt::type_value<std::string, int> copy(0);
        copy = original;
        REQUIRE(copy.value() == 42);
        REQUIRE(original.value() == 42);
    }

    SECTION("move assignment")
    {
        fxt::type_value<std::string, std::string> original(std::string("world"));
        fxt::type_value<std::string, std::string> moved(std::string(""));
        moved = std::move(original);
        REQUIRE(moved.value() == "world");
    }
}

TEST_CASE("Type - specialization implicit conversion", "[type]")
{
    SECTION("implicit conversion to const reference")
    {
        fxt::type_value<std::string, int> tagged(42);
        const int& val = tagged;
        REQUIRE(val == 42);
    }

    SECTION("implicit conversion to mutable reference")
    {
        fxt::type_value<std::string, int> tagged(100);
        int& val = tagged;
        val = 200;
        REQUIRE(tagged.value() == 200);
    }

    SECTION("implicit conversion in function call")
    {
        auto accept_int = [](int x) { return x * 2; };
        fxt::type_value<std::string, int> tagged(21);
        REQUIRE(accept_int(tagged) == 42);
    }
}

TEST_CASE("Type - specialization comparison operators", "[type]")
{
    SECTION("equality comparison - equal values")
    {
        fxt::type_value<std::string, int> a(42);
        fxt::type_value<std::string, int> b(42);
        REQUIRE(a == b);
    }

    SECTION("equality comparison - different values")
    {
        fxt::type_value<std::string, int> a(42);
        fxt::type_value<std::string, int> b(100);
        REQUIRE(a != b);
    }

    SECTION("three-way comparison - less than")
    {
        fxt::type_value<std::string, int> a(10);
        fxt::type_value<std::string, int> b(20);
        REQUIRE(a < b);
        REQUIRE(a <= b);
        REQUIRE(!(a > b));
        REQUIRE(!(a >= b));
    }

    SECTION("three-way comparison - greater than")
    {
        fxt::type_value<std::string, int> a(50);
        fxt::type_value<std::string, int> b(30);
        REQUIRE(a > b);
        REQUIRE(a >= b);
        REQUIRE(!(a < b));
        REQUIRE(!(a <= b));
    }

    SECTION("three-way comparison - equal")
    {
        fxt::type_value<std::string, int> a(42);
        fxt::type_value<std::string, int> b(42);
        REQUIRE(a <= b);
        REQUIRE(a >= b);
    }

    SECTION("string comparison")
    {
        fxt::type_value<int, std::string> a(std::string("apple"));
        fxt::type_value<int, std::string> b(std::string("banana"));
        REQUIRE(a < b);
        REQUIRE(a != b);
    }
}

TEST_CASE("Type - specialization constexpr support", "[type]")
{
    SECTION("constexpr construction")
    {
        constexpr fxt::type_value<std::string, int> tagged(42);
        static_assert(tagged.value() == 42);
    }

    SECTION("constexpr value access")
    {
        constexpr fxt::type_value<std::string, int> tagged(100);
        constexpr int val = tagged.value();
        static_assert(val == 100);
    }

    SECTION("constexpr get access")
    {
        constexpr fxt::type_value<std::string, int> tagged(200);
        static_assert(tagged.get() == 200);
    }
}

// ============================================================================
// Test Helper Aliases
// ============================================================================

TEST_CASE("Type - type_t helper alias", "[type]")
{
    SECTION("extract type from primary template")
    {
        using TagType = fxt::type_value<int>::type_t;
        using ExtractedType = fxt::type_value<TagType>::type_t;
        REQUIRE(std::is_same_v<ExtractedType, int>);
    }

    SECTION("extract type from specialization")
    {
        using TagType = fxt::type_value<std::string, int>::type_t;
        using ExtractedType = fxt::type_value<TagType>::type_t;
        REQUIRE(std::is_same_v<ExtractedType, std::string>);
    }

    SECTION("extract type with multiple types")
    {
        using TagType = fxt::type_value<double>::type_t;
        using ExtractedType = fxt::type_value<TagType>::type_t;
        REQUIRE(std::is_same_v<ExtractedType, double>);
    }
}

TEST_CASE("Type - value_type_t helper alias", "[type]")
{
    SECTION("extract value_type from specialization with int")
    {
        using TagType = fxt::type_value<std::string, int>;
        using ValueType = fxt::value_type_t<TagType>;
        REQUIRE(std::is_same_v<ValueType, int>);
    }

    SECTION("extract value_type from specialization with string")
    {
        using TagType = fxt::type_value<int, std::string>;
        using ValueType = fxt::value_type_t<TagType>;
        REQUIRE(std::is_same_v<ValueType, std::string>);
    }

    SECTION("extract value_type from specialization with double")
    {
        using TagType = fxt::type_value<char, double>;
        using ValueType = fxt::value_type_t<TagType>;
        REQUIRE(std::is_same_v<ValueType, double>);
    }
}

// ============================================================================
// Test Helper Functions
// ============================================================================

TEST_CASE("Type - make_type() helper for pure type tag", "[type]")
{
    SECTION("create int type tag")
    {
        auto tag = fxt::make_type<int>();
        REQUIRE(std::is_same_v<typename decltype(tag)::type_t, int>);
        REQUIRE(fxt::is_type_v<decltype(tag)>);
    }

    SECTION("create string type tag")
    {
        auto tag = fxt::make_type<std::string>();
        REQUIRE(std::is_same_v<typename decltype(tag)::type_t, std::string>);
        REQUIRE(fxt::is_type_v<decltype(tag)>);
    }

    SECTION("create double type tag")
    {
        auto tag = fxt::make_type<double>();
        REQUIRE(std::is_same_v<typename decltype(tag)::type_t, double>);
        REQUIRE(fxt::is_type_v<decltype(tag)>);
    }

    SECTION("constexpr make_type")
    {
        constexpr auto tag = fxt::make_type<int>();
        static_assert(std::is_same_v<typename decltype(tag)::type_t, int>);
    }
}

TEST_CASE("Type - make_type(value) helper with value", "[type]")
{
    SECTION("create Type with int value")
    {
        auto tagged = fxt::make_type<std::string>(42);
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, std::string>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, int>);
        REQUIRE(tagged.value() == 42);
    }

    SECTION("create Type with string value")
    {
        auto tagged = fxt::make_type<int>(std::string("hello"));
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, int>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, std::string>);
        REQUIRE(tagged.value() == "hello");
    }

    SECTION("create Type with double value")
    {
        auto tagged = fxt::make_type<char>(3.14);
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, char>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, double>);
        REQUIRE(tagged.value() == Catch::Approx(3.14));
    }

    SECTION("make_type with rvalue")
    {
        auto tagged = fxt::make_type<int>(std::string("temp"));
        REQUIRE(tagged.value() == "temp");
    }

    SECTION("make_type with lvalue")
    {
        int val = 100;
        auto tagged = fxt::make_type<std::string>(val);
        REQUIRE(tagged.value() == 100);
    }

    SECTION("constexpr make_type with value")
    {
        constexpr auto tagged = fxt::make_type<std::string>(42);
        static_assert(tagged.value() == 42);
    }
}

// ============================================================================
// Test Type Trait
// ============================================================================

TEST_CASE("Type - is_type trait", "[type]")
{
    SECTION("is_type with primary template")
    {
        using T = fxt::type_value<int>;
        REQUIRE(fxt::is_type<T>::value);
        REQUIRE(fxt::is_type_v<T>);
    }

    SECTION("is_type with specialization")
    {
        using T = fxt::type_value<std::string, int>;
        REQUIRE(fxt::is_type<T>::value);
        REQUIRE(fxt::is_type_v<T>);
    }

    SECTION("is_type with non-Type")
    {
        REQUIRE_FALSE(fxt::is_type<int>::value);
        REQUIRE_FALSE(fxt::is_type_v<int>);
        REQUIRE_FALSE(fxt::is_type_v<std::string>);
        REQUIRE_FALSE(fxt::is_type_v<double>);
    }

    SECTION("is_type with various Type instantiations")
    {
        REQUIRE(fxt::is_type_v<fxt::type_value<int>>);
        REQUIRE(fxt::is_type_v<fxt::type_value<std::string>>);
        REQUIRE(fxt::is_type_v<fxt::type_value<double, int>>);
        REQUIRE(fxt::is_type_v<fxt::type_value<char, std::string>>);
    }
}

// ============================================================================
// Test Tag Dispatch Pattern
// ============================================================================

namespace tag_dispatch_tests
{
    int process(fxt::type_value<int>, int value) { return value * 2; }
    std::string process(fxt::type_value<std::string>, int value) { return std::to_string(value); }
    double process(fxt::type_value<double>, int value) { return value * 1.5; }
}

TEST_CASE("Type - tag dispatch pattern", "[type]")
{
    using namespace tag_dispatch_tests;

    SECTION("dispatch to int overload")
    {
        int result = process(fxt::type_value<int>{}, 21);
        REQUIRE(result == 42);
    }

    SECTION("dispatch to string overload")
    {
        std::string result = process(fxt::type_value<std::string>{}, 123);
        REQUIRE(result == "123");
    }

    SECTION("dispatch to double overload")
    {
        double result = process(fxt::type_value<double>{}, 10);
        REQUIRE(result == Catch::Approx(15.0));
    }

    SECTION("dispatch with make_type helper")
    {
        int result = process(fxt::make_type<int>(), 21);
        REQUIRE(result == 42);
    }
}

// ============================================================================
// Test Complex Types
// ============================================================================

TEST_CASE("Type - with complex types", "[type]")
{
    SECTION("Type with vector")
    {
        std::vector<int> vec = {1, 2, 3, 4, 5};
        fxt::type_value<std::string, std::vector<int>> tagged(vec);
        REQUIRE(tagged.value().size() == 5);
        REQUIRE(tagged.value()[0] == 1);
    }

    SECTION("Type with unique_ptr")
    {
        auto ptr = std::make_unique<int>(42);
        fxt::type_value<std::string, std::unique_ptr<int>> tagged(std::move(ptr));
        REQUIRE(*tagged.value() == 42);
    }

    SECTION("Type with nested Type")
    {
        fxt::type_value<int, double> inner(3.14);
        fxt::type_value<std::string, fxt::type_value<int, double>> outer(inner);
        REQUIRE(outer.value().value() == Catch::Approx(3.14));
    }

    SECTION("Type with custom struct")
    {
        struct Point { int x, y; };
        fxt::type_value<std::string, Point> tagged(Point{10, 20});
        REQUIRE(tagged.value().x == 10);
        REQUIRE(tagged.value().y == 20);
    }
}

// ============================================================================
// Test Edge Cases
// ============================================================================

TEST_CASE("Type - edge cases", "[type]")
{
    SECTION("Type with same tag and value type")
    {
        fxt::type_value<int, int> tagged(42);
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, int>);
        REQUIRE(std::is_same_v<typename decltype(tagged)::value_type, int>);
        REQUIRE(tagged.value() == 42);
    }

    SECTION("Type with void tag")
    {
        fxt::type_value<void, int> tagged(100);
        REQUIRE(std::is_same_v<typename decltype(tagged)::type_t, void>);
        REQUIRE(tagged.value() == 100);
    }

    SECTION("Type with const qualified tag")
    {
        fxt::type_value<const int> tag;
        REQUIRE(std::is_same_v<typename decltype(tag)::type_t, const int>);
    }

    SECTION("Type with pointer type")
    {
        int value = 42;
        fxt::type_value<std::string, int*> tagged(&value);
        REQUIRE(*tagged.value() == 42);
    }

    SECTION("Type with reference wrapper")
    {
        int value = 100;
        auto ref = std::ref(value);
        fxt::type_value<std::string, std::reference_wrapper<int>> tagged(ref);
        REQUIRE(tagged.value().get() == 100);
        value = 200;
        REQUIRE(tagged.value().get() == 200);
    }
}

// ============================================================================
// Test Noexcept Specifications
// ============================================================================

TEST_CASE("Type - noexcept specifications", "[type]")
{
    SECTION("primary template constructors are noexcept")
    {
        REQUIRE(noexcept(fxt::type_value<int>{}));
        REQUIRE(noexcept(fxt::type_value<int>(42)));
    }

    SECTION("get() is noexcept")
    {
        fxt::type_value<std::string, int> tagged(42);
        REQUIRE(noexcept(tagged.get()));
    }

    SECTION("make_type() without value is noexcept")
    {
        REQUIRE(noexcept(fxt::make_type<int>()));
    }

    SECTION("implicit conversions are noexcept")
    {
        fxt::type_value<std::string, int> tagged(42);
        REQUIRE(noexcept(static_cast<const int&>(tagged)));
    }
}
