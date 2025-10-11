//
// Test suite for fxt::flat_tuple and fxt::mget
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt/tuples/FlatTuple.hpp>
#include <memory>
#include <string>
#include <vector>

TEST_CASE("flat_tuple - construction", "[flat_tuple]")
{
    SECTION("construct empty flat_tuple")
    {
        fxt::flat_tuple<> empty;
        REQUIRE(empty.size() == 0);
    }

    SECTION("construct single element flat_tuple")
    {
        fxt::flat_tuple<int> single(42);
        REQUIRE(single.size() == 1);
        REQUIRE(fxt::get<0>(single) == 42);
    }

    SECTION("construct two element flat_tuple")
    {
        fxt::flat_tuple<int, std::string> tuple(42, "hello");
        REQUIRE(tuple.size() == 2);
        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(fxt::get<1>(tuple) == "hello");
    }

    SECTION("construct three element flat_tuple")
    {
        fxt::flat_tuple<int, std::string, double> tuple(42, "world", 3.14);
        REQUIRE(tuple.size() == 3);
        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(fxt::get<1>(tuple) == "world");
        REQUIRE(fxt::get<2>(tuple) == 3.14);
    }

    SECTION("construct with multiple types")
    {
        fxt::flat_tuple<int, double, bool, std::string> tuple(1, 2.5, true, "test");
        REQUIRE(tuple.size() == 4);
        REQUIRE(fxt::get<0>(tuple) == 1);
        REQUIRE(fxt::get<1>(tuple) == 2.5);
        REQUIRE(fxt::get<2>(tuple) == true);
        REQUIRE(fxt::get<3>(tuple) == "test");
    }
}

TEST_CASE("flat_tuple - default construction", "[flat_tuple]")
{
    SECTION("default construct empty tuple")
    {
        fxt::flat_tuple<> empty;
        REQUIRE(empty.size() == 0);
    }
}

TEST_CASE("flat_tuple - get access", "[flat_tuple][get]")
{
    SECTION("get from single element tuple")
    {
        fxt::flat_tuple<int> tuple(100);
        REQUIRE(fxt::get<0>(tuple) == 100);
    }

    SECTION("get from multi-element tuple")
    {
        fxt::flat_tuple<int, std::string, double> tuple(42, "test", 2.71);
        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(fxt::get<1>(tuple) == "test");
        REQUIRE(fxt::get<2>(tuple) == 2.71);
    }

    SECTION("get with different types")
    {
        fxt::flat_tuple<char, int, long, float, double> tuple('A', 1, 2L, 3.0f, 4.0);
        REQUIRE(fxt::get<0>(tuple) == 'A');
        REQUIRE(fxt::get<1>(tuple) == 1);
        REQUIRE(fxt::get<2>(tuple) == 2L);
        REQUIRE(fxt::get<3>(tuple) == 3.0f);
        REQUIRE(fxt::get<4>(tuple) == 4.0);
    }
}

TEST_CASE("flat_tuple - get modification", "[flat_tuple][get]")
{
    SECTION("modify single element")
    {
        fxt::flat_tuple<int> tuple(10);
        fxt::get<0>(tuple) = 20;
        REQUIRE(fxt::get<0>(tuple) == 20);
    }

    SECTION("modify multiple elements")
    {
        fxt::flat_tuple<int, std::string, double> tuple(1, "old", 1.0);

        fxt::get<0>(tuple) = 99;
        fxt::get<1>(tuple) = "new";
        fxt::get<2>(tuple) = 9.99;

        REQUIRE(fxt::get<0>(tuple) == 99);
        REQUIRE(fxt::get<1>(tuple) == "new");
        REQUIRE(fxt::get<2>(tuple) == 9.99);
    }

    SECTION("modify and verify independence")
    {
        fxt::flat_tuple<int, int, int> tuple(1, 2, 3);

        fxt::get<0>(tuple) = 10;
        REQUIRE(fxt::get<0>(tuple) == 10);
        REQUIRE(fxt::get<1>(tuple) == 2);
        REQUIRE(fxt::get<2>(tuple) == 3);

        fxt::get<1>(tuple) = 20;
        REQUIRE(fxt::get<0>(tuple) == 10);
        REQUIRE(fxt::get<1>(tuple) == 20);
        REQUIRE(fxt::get<2>(tuple) == 3);
    }
}

TEST_CASE("flat_tuple - const get access", "[flat_tuple][get]")
{
    SECTION("const get from single element")
    {
        const fxt::flat_tuple<int> tuple(42);
        REQUIRE(fxt::get<0>(tuple) == 42);
    }

    SECTION("const get from multi-element")
    {
        const fxt::flat_tuple<int, std::string, double> tuple(100, "const", 3.14159);
        REQUIRE(fxt::get<0>(tuple) == 100);
        REQUIRE(fxt::get<1>(tuple) == "const");
        REQUIRE(fxt::get<2>(tuple) == 3.14159);
    }

    SECTION("const reference parameter")
    {
        auto check_tuple = [](const fxt::flat_tuple<int, std::string>& t) {
            return fxt::get<0>(t) == 42 && fxt::get<1>(t) == "test";
        };

        fxt::flat_tuple<int, std::string> tuple(42, "test");
        REQUIRE(check_tuple(tuple));
    }
}

TEST_CASE("flat_tuple - rvalue get access", "[flat_tuple][get]")
{
    SECTION("get from rvalue single element")
    {
        auto value = fxt::get<0>(fxt::flat_tuple<int>(42));
        REQUIRE(value == 42);
    }

    SECTION("get from rvalue multi-element")
    {
        auto value = fxt::get<1>(fxt::flat_tuple<int, std::string, double>(1, "rvalue", 2.5));
        REQUIRE(value == "rvalue");
    }

    SECTION("move from rvalue")
    {
        auto ptr = fxt::get<0>(fxt::flat_tuple<std::unique_ptr<int>>(std::make_unique<int>(100)));
        REQUIRE(*ptr == 100);
    }
}

TEST_CASE("flat_tuple - complex types", "[flat_tuple]")
{
    SECTION("vector in tuple")
    {
        std::vector<int> vec{1, 2, 3, 4, 5};
        fxt::flat_tuple<int, std::vector<int>> tuple(42, vec);

        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(fxt::get<1>(tuple).size() == 5);
        REQUIRE(fxt::get<1>(tuple)[0] == 1);
        REQUIRE(fxt::get<1>(tuple)[4] == 5);
    }

    SECTION("modify vector in tuple")
    {
        fxt::flat_tuple<std::vector<int>> tuple(std::vector<int>{1, 2, 3});

        fxt::get<0>(tuple).push_back(4);
        fxt::get<0>(tuple).push_back(5);

        REQUIRE(fxt::get<0>(tuple).size() == 5);
        REQUIRE(fxt::get<0>(tuple)[3] == 4);
        REQUIRE(fxt::get<0>(tuple)[4] == 5);
    }

    SECTION("string manipulation")
    {
        fxt::flat_tuple<std::string, std::string> tuple("hello", "world");

        fxt::get<0>(tuple) += " ";
        fxt::get<0>(tuple) += fxt::get<1>(tuple);

        REQUIRE(fxt::get<0>(tuple) == "hello world");
        REQUIRE(fxt::get<1>(tuple) == "world");
    }

    SECTION("nested containers")
    {
        using VecVec = std::vector<std::vector<int>>;
        VecVec nested{{1, 2}, {3, 4}, {5, 6}};

        fxt::flat_tuple<int, VecVec> tuple(42, nested);

        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(fxt::get<1>(tuple).size() == 3);
        REQUIRE(fxt::get<1>(tuple)[0][0] == 1);
        REQUIRE(fxt::get<1>(tuple)[2][1] == 6);
    }
}

TEST_CASE("flat_tuple - move-only types", "[flat_tuple]")
{
    SECTION("single unique_ptr")
    {
        fxt::flat_tuple<std::unique_ptr<int>> tuple(std::make_unique<int>(42));
        REQUIRE(*fxt::get<0>(tuple) == 42);

        *fxt::get<0>(tuple) = 100;
        REQUIRE(*fxt::get<0>(tuple) == 100);
    }

    SECTION("multiple unique_ptrs")
    {
        fxt::flat_tuple<std::unique_ptr<int>, std::unique_ptr<std::string>> tuple(
            std::make_unique<int>(42),
            std::make_unique<std::string>("test")
        );

        REQUIRE(*fxt::get<0>(tuple) == 42);
        REQUIRE(*fxt::get<1>(tuple) == "test");
    }

    SECTION("move construction")
    {
        auto ptr = std::make_unique<int>(100);
        fxt::flat_tuple<std::unique_ptr<int>> tuple(std::move(ptr));

        REQUIRE(ptr == nullptr);
        REQUIRE(*fxt::get<0>(tuple) == 100);
    }

    SECTION("mixed move-only and copyable")
    {
        fxt::flat_tuple<int, std::unique_ptr<int>, std::string> tuple(
            42,
            std::make_unique<int>(100),
            "test"
        );

        REQUIRE(fxt::get<0>(tuple) == 42);
        REQUIRE(*fxt::get<1>(tuple) == 100);
        REQUIRE(fxt::get<2>(tuple) == "test");
    }
}

TEST_CASE("flat_tuple - size", "[flat_tuple]")
{
    SECTION("empty tuple size")
    {
        fxt::flat_tuple<> empty;
        REQUIRE(empty.size() == 0);
        static_assert(fxt::flat_tuple<>::size() == 0);
    }

    SECTION("single element size")
    {
        fxt::flat_tuple<int> single(1);
        REQUIRE(single.size() == 1);
        static_assert(fxt::flat_tuple<int>::size() == 1);
    }

    SECTION("multiple elements size")
    {
        fxt::flat_tuple<int, double, std::string> tuple(1, 2.0, "three");
        REQUIRE(tuple.size() == 3);
        static_assert(fxt::flat_tuple<int, double, std::string>::size() == 3);
    }

    SECTION("large tuple size")
    {
        fxt::flat_tuple<int, int, int, int, int, int, int, int, int, int> large(
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10
        );
        REQUIRE(large.size() == 10);
        static_assert(decltype(large)::size() == 10);
    }
}

TEST_CASE("flat_tuple - type correctness", "[flat_tuple]")
{
    SECTION("correct types")
    {
        fxt::flat_tuple<int, std::string, double> tuple(42, "test", 3.14);

        static_assert(std::is_same_v<decltype(fxt::get<0>(tuple)), int&>);
        static_assert(std::is_same_v<decltype(fxt::get<1>(tuple)), std::string&>);
        static_assert(std::is_same_v<decltype(fxt::get<2>(tuple)), double&>);
    }

    SECTION("const correctness")
    {
        const fxt::flat_tuple<int, std::string> tuple(42, "test");

        static_assert(std::is_same_v<decltype(fxt::get<0>(tuple)), const int&>);
        static_assert(std::is_same_v<decltype(fxt::get<1>(tuple)), const std::string&>);
    }

    SECTION("rvalue types")
    {
        using Tuple = fxt::flat_tuple<int, std::string>;

        static_assert(std::is_same_v<decltype(fxt::get<0>(Tuple(1, "test"))), int&&>);
        static_assert(std::is_same_v<decltype(fxt::get<1>(Tuple(1, "test"))), std::string&&>);
    }
}

TEST_CASE("flat_tuple - copy semantics", "[flat_tuple]")
{
    SECTION("copy construct")
    {
        fxt::flat_tuple<int, std::string> original(42, "hello");
        fxt::flat_tuple<int, std::string> copy = original;

        REQUIRE(fxt::get<0>(copy) == 42);
        REQUIRE(fxt::get<1>(copy) == "hello");

        // Verify independence
        fxt::get<0>(copy) = 100;
        REQUIRE(fxt::get<0>(original) == 42);
        REQUIRE(fxt::get<0>(copy) == 100);
    }

    SECTION("copy assign")
    {
        fxt::flat_tuple<int, std::string> original(42, "hello");
        fxt::flat_tuple<int, std::string> other(0, "");

        other = original;

        REQUIRE(fxt::get<0>(other) == 42);
        REQUIRE(fxt::get<1>(other) == "hello");
    }
}

TEST_CASE("flat_tuple - move semantics", "[flat_tuple]")
{
    SECTION("move construct")
    {
        fxt::flat_tuple<std::unique_ptr<int>> original(std::make_unique<int>(42));
        fxt::flat_tuple<std::unique_ptr<int>> moved(std::move(original));

        REQUIRE(*fxt::get<0>(moved) == 42);
    }

    SECTION("move assign")
    {
        fxt::flat_tuple<std::unique_ptr<int>> original(std::make_unique<int>(42));
        fxt::flat_tuple<std::unique_ptr<int>> other(std::make_unique<int>(0));

        other = std::move(original);

        REQUIRE(*fxt::get<0>(other) == 42);
    }

    SECTION("move string")
    {
        fxt::flat_tuple<std::string> original("hello world");
        fxt::flat_tuple<std::string> moved(std::move(original));

        REQUIRE(fxt::get<0>(moved) == "hello world");
    }
}

TEST_CASE("flat_tuple - references and pointers", "[flat_tuple]")
{
    SECTION("get returns reference")
    {
        fxt::flat_tuple<int> tuple(42);
        int& ref = fxt::get<0>(tuple);

        ref = 100;
        REQUIRE(fxt::get<0>(tuple) == 100);
    }

    SECTION("get const reference")
    {
        const fxt::flat_tuple<int> tuple(42);
        const int& ref = fxt::get<0>(tuple);

        REQUIRE(ref == 42);
    }

    SECTION("take address of element")
    {
        fxt::flat_tuple<int, std::string> tuple(42, "test");

        int* ptr_int = &fxt::get<0>(tuple);
        std::string* ptr_str = &fxt::get<1>(tuple);

        *ptr_int = 100;
        *ptr_str = "modified";

        REQUIRE(fxt::get<0>(tuple) == 100);
        REQUIRE(fxt::get<1>(tuple) == "modified");
    }
}

TEST_CASE("flat_tuple - with algorithms", "[flat_tuple]")
{
    SECTION("modify vector element with algorithm")
    {
        fxt::flat_tuple<std::vector<int>> tuple(std::vector<int>{5, 2, 8, 1, 9});

        std::sort(fxt::get<0>(tuple).begin(), fxt::get<0>(tuple).end());

        REQUIRE(fxt::get<0>(tuple)[0] == 1);
        REQUIRE(fxt::get<0>(tuple)[1] == 2);
        REQUIRE(fxt::get<0>(tuple)[4] == 9);
    }

    SECTION("use with transform")
    {
        fxt::flat_tuple<std::vector<int>, std::vector<int>> tuple(
            std::vector<int>{1, 2, 3},
            std::vector<int>(3)
        );

        std::transform(
            fxt::get<0>(tuple).begin(),
            fxt::get<0>(tuple).end(),
            fxt::get<1>(tuple).begin(),
            [](int x) { return x * 2; }
        );

        REQUIRE(fxt::get<1>(tuple)[0] == 2);
        REQUIRE(fxt::get<1>(tuple)[1] == 4);
        REQUIRE(fxt::get<1>(tuple)[2] == 6);
    }
}

TEST_CASE("flat_tuple - edge cases", "[flat_tuple]")
{
    SECTION("zero values")
    {
        fxt::flat_tuple<int, double, long> tuple(0, 0.0, 0L);

        REQUIRE(fxt::get<0>(tuple) == 0);
        REQUIRE(fxt::get<1>(tuple) == 0.0);
        REQUIRE(fxt::get<2>(tuple) == 0L);
    }

    SECTION("empty strings")
    {
        fxt::flat_tuple<std::string, std::string> tuple("", "");

        REQUIRE(fxt::get<0>(tuple).empty());
        REQUIRE(fxt::get<1>(tuple).empty());
    }

    SECTION("false boolean")
    {
        fxt::flat_tuple<bool, bool> tuple(false, false);

        REQUIRE(fxt::get<0>(tuple) == false);
        REQUIRE(fxt::get<1>(tuple) == false);
    }

    SECTION("null pointer")
    {
        fxt::flat_tuple<int*, std::unique_ptr<int>> tuple(nullptr, nullptr);

        REQUIRE(fxt::get<0>(tuple) == nullptr);
        REQUIRE(fxt::get<1>(tuple) == nullptr);
    }
}

TEST_CASE("flat_tuple - same types", "[flat_tuple]")
{
    SECTION("multiple ints")
    {
        fxt::flat_tuple<int, int, int, int> tuple(1, 2, 3, 4);

        REQUIRE(fxt::get<0>(tuple) == 1);
        REQUIRE(fxt::get<1>(tuple) == 2);
        REQUIRE(fxt::get<2>(tuple) == 3);
        REQUIRE(fxt::get<3>(tuple) == 4);
    }

    SECTION("modify multiple same types independently")
    {
        fxt::flat_tuple<std::string, std::string, std::string> tuple("a", "b", "c");

        fxt::get<0>(tuple) = "x";
        fxt::get<2>(tuple) = "z";

        REQUIRE(fxt::get<0>(tuple) == "x");
        REQUIRE(fxt::get<1>(tuple) == "b");
        REQUIRE(fxt::get<2>(tuple) == "z");
    }
}

TEST_CASE("flat_tuple - large tuples", "[flat_tuple]")
{
    SECTION("ten elements")
    {
        fxt::flat_tuple<int, int, int, int, int, int, int, int, int, int> tuple(
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9
        );

        for (int i = 0; i < 10; ++i) {
            if (i == 0) REQUIRE(fxt::get<0>(tuple) == i);
            else if (i == 1) REQUIRE(fxt::get<1>(tuple) == i);
            else if (i == 2) REQUIRE(fxt::get<2>(tuple) == i);
            else if (i == 3) REQUIRE(fxt::get<3>(tuple) == i);
            else if (i == 4) REQUIRE(fxt::get<4>(tuple) == i);
            else if (i == 5) REQUIRE(fxt::get<5>(tuple) == i);
            else if (i == 6) REQUIRE(fxt::get<6>(tuple) == i);
            else if (i == 7) REQUIRE(fxt::get<7>(tuple) == i);
            else if (i == 8) REQUIRE(fxt::get<8>(tuple) == i);
            else if (i == 9) REQUIRE(fxt::get<9>(tuple) == i);
        }
    }

    SECTION("mixed types large tuple")
    {
        fxt::flat_tuple<int, std::string, double, bool, char, float, long, short, unsigned, std::string> tuple(
            1, "two", 3.0, true, 'c', 6.0f, 7L, short(8), 9u, "ten"
        );

        REQUIRE(fxt::get<0>(tuple) == 1);
        REQUIRE(fxt::get<1>(tuple) == "two");
        REQUIRE(fxt::get<2>(tuple) == 3.0);
        REQUIRE(fxt::get<3>(tuple) == true);
        REQUIRE(fxt::get<4>(tuple) == 'c');
        REQUIRE(fxt::get<5>(tuple) == 6.0f);
        REQUIRE(fxt::get<6>(tuple) == 7L);
        REQUIRE(fxt::get<7>(tuple) == short(8));
        REQUIRE(fxt::get<8>(tuple) == 9u);
        REQUIRE(fxt::get<9>(tuple) == "ten");
    }
}

TEST_CASE("flat_tuple - comparison with std::tuple behavior", "[flat_tuple]")
{
    SECTION("similar construction")
    {
        std::tuple<int, std::string> std_tuple(42, "test");
        fxt::flat_tuple<int, std::string> flat_tuple(42, "test");

        REQUIRE(std::get<0>(std_tuple) == fxt::get<0>(flat_tuple));
        REQUIRE(std::get<1>(std_tuple) == fxt::get<1>(flat_tuple));
    }

    SECTION("similar modification")
    {
        std::tuple<int, std::string> std_tuple(1, "old");
        fxt::flat_tuple<int, std::string> flat_tuple(1, "old");

        std::get<0>(std_tuple) = 2;
        fxt::get<0>(flat_tuple) = 2;

        std::get<1>(std_tuple) = "new";
        fxt::get<1>(flat_tuple) = "new";

        REQUIRE(std::get<0>(std_tuple) == fxt::get<0>(flat_tuple));
        REQUIRE(std::get<1>(std_tuple) == fxt::get<1>(flat_tuple));
    }

    SECTION("similar size")
    {
        std::tuple<int, double, std::string> std_tuple(1, 2.0, "three");
        fxt::flat_tuple<int, double, std::string> flat_tuple(1, 2.0, "three");

        REQUIRE(std::tuple_size_v<decltype(std_tuple)> == flat_tuple.size());
    }
}

TEST_CASE("flat_tuple - practical use cases", "[flat_tuple]")
{
    SECTION("coordinate system")
    {
        fxt::flat_tuple<double, double, double> point(1.0, 2.0, 3.0);

        auto x = fxt::get<0>(point);
        auto y = fxt::get<1>(point);
        auto z = fxt::get<2>(point);

        auto distance = std::sqrt(x*x + y*y + z*z);
        REQUIRE(distance > 3.74);
        REQUIRE(distance < 3.75);
    }

    SECTION("configuration storage")
    {
        fxt::flat_tuple<std::string, int, bool, double> config("Server", 8080, true, 30.0);

        REQUIRE(fxt::get<0>(config) == "Server");
        REQUIRE(fxt::get<1>(config) == 8080);
        REQUIRE(fxt::get<2>(config) == true);
        REQUIRE(fxt::get<3>(config) == 30.0);

        // Modify configuration
        fxt::get<1>(config) = 9090;
        fxt::get<3>(config) = 60.0;

        REQUIRE(fxt::get<1>(config) == 9090);
        REQUIRE(fxt::get<3>(config) == 60.0);
    }

    SECTION("return multiple values")
    {
        auto parse_result = []() {
            return fxt::flat_tuple<bool, int, std::string>(true, 42, "Success");
        };

        auto result = parse_result();
        REQUIRE(fxt::get<0>(result) == true);
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == "Success");
    }
}

