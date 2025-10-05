//
// Created by kenne on 05/10/2025.
//

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <functional>

// Test helper functions
int add(int a, int b) {
    return a + b;
}

int add_three(int a, int b, int c) {
    return a + b + c;
}

int add_four(int a, int b, int c, int d) {
    return a + b + c + d;
}

std::string concat(const std::string& a, const std::string& b, const std::string& c) {
    return a + b + c;
}

double multiply(double a, double b) {
    return a * b;
}

TEST_CASE("curry with two-argument function", "[curry]") {
    SECTION("curry and call with all arguments at once") {
        auto curried_add = fxt::curry(add);
        auto result = curried_add(5, 10);
        REQUIRE(result == 15);
    }

    SECTION("curry and call with one argument at a time") {
        auto curried_add = fxt::curry(add);
        auto add5 = curried_add(5);
        auto result = add5(10);
        REQUIRE(result == 15);
    }

    SECTION("curry lambda with two arguments") {
        auto lambda = [](int x, int y) { return x * y; };
        auto curried = fxt::curry(lambda);
        auto times3 = curried(3);
        REQUIRE(times3(7) == 21);
    }
}

TEST_CASE("curry with three-argument function", "[curry]") {
    SECTION("curry and call with all arguments at once") {
        auto curried = fxt::curry(add_three);
        REQUIRE(curried(1, 2, 3) == 6);
    }

    SECTION("curry and call with one argument at a time") {
        auto curried = fxt::curry(add_three);
        auto step1 = curried(10);
        auto step2 = step1(20);
        auto result = step2(30);
        REQUIRE(result == 60);
    }

    SECTION("curry and call with two arguments, then one") {
        auto curried = fxt::curry(add_three);
        auto partial = curried(5, 10);
        auto result = partial(15);
        REQUIRE(result == 30);
    }

    SECTION("curry string concatenation") {
        auto curried = fxt::curry(concat);
        auto result = curried("Hello")(" ")("World");
        REQUIRE(result == "Hello World");
    }
}

TEST_CASE("curry with four-argument function", "[curry]") {
    SECTION("curry and apply arguments in different combinations") {
        auto curried = fxt::curry(add_four);

        // All at once
        REQUIRE(curried(1, 2, 3, 4) == 10);

        // One at a time
        REQUIRE(curried(1)(2)(3)(4) == 10);

        // Two and two
        REQUIRE(curried(1, 2)(3, 4) == 10);

        // Three and one
        REQUIRE(curried(1, 2, 3)(4) == 10);
    }
}

TEST_CASE("curry with different types", "[curry]") {
    SECTION("curry double multiplication") {
        auto curried = fxt::curry(multiply);
        auto double_it = curried(2.0);
        REQUIRE(double_it(3.5) == 7.0);
    }

    SECTION("curry mixed types lambda") {
        auto lambda = [](int x, double y, std::string z) {
            return std::to_string(x) + " " + std::to_string(y) + " " + z;
        };
        auto curried = fxt::curry(lambda);
        auto result = curried(42)(3.14)("test");
        REQUIRE(result == "42 3.140000 test");
    }
}

TEST_CASE("curry reusability", "[curry]") {
    SECTION("reuse partially applied function") {
        auto curried = fxt::curry(add_three);
        auto add10 = curried(10);

        REQUIRE(add10(5, 3) == 18);
        REQUIRE(add10(2, 8) == 20);
        REQUIRE(add10(1)(1) == 12);
    }

    SECTION("create multiple partial applications") {
        auto curried = fxt::curry(add);
        auto add5 = curried(5);
        auto add10 = curried(10);

        REQUIRE(add5(3) == 8);
        REQUIRE(add10(3) == 13);
    }
}

TEST_CASE("curry with lambdas", "[curry]") {
    SECTION("curry generic lambda") {
        auto lambda = [](auto x, auto y) { return x + y; };
        auto curried = fxt::curry(lambda);

        REQUIRE(curried(5)(10) == 15);
        REQUIRE(curried(std::string("Hello"))(std::string(" World")) == "Hello World");
    }

    SECTION("curry lambda with captures") {
        int multiplier = 3;
        auto lambda = [multiplier](int x, int y) { return (x + y) * multiplier; };
        auto curried = fxt::curry(lambda);

        REQUIRE(curried(2)(3) == 15);
        REQUIRE(curried(5, 5) == 30);
    }
}

TEST_CASE("curry with std::function", "[curry]") {
    SECTION("curry std::function with two arguments") {
        std::function<int(int, int)> func = add;
        auto curried = fxt::curry(func);
        auto add7 = curried(7);
        REQUIRE(add7(3) == 10);
    }
}

TEST_CASE("curry edge cases", "[curry]") {
    SECTION("curry function returning void") {
        int result = 0;
        auto lambda = [&result](int x, int y) { result = x + y; };
        auto curried = fxt::curry(lambda);
        curried(5)(10);
        REQUIRE(result == 15);
    }

    SECTION("curry with single argument function") {
        auto lambda = [](int x) { return x * 2; };
        auto curried = fxt::curry(lambda);
        REQUIRE(curried(5) == 10);
    }

    SECTION("curry with reference arguments - no partial application") {
        // Note: std::bind_front copies arguments, so references don't work through partial application
        // This test only works when all arguments are provided at once
        auto lambda = [](int& x, int y) { x += y; return x; };
        auto curried = fxt::curry(lambda);
        int value = 10;
        auto result = curried(value, 5);
        REQUIRE(value == 15);
        REQUIRE(result == 15);
    }
}

TEST_CASE("curry composition", "[curry]") {
    SECTION("compose curried functions") {
        auto add_curried = fxt::curry([](int x, int y) { return x + y; });
        auto mul_curried = fxt::curry([](int x, int y) { return x * y; });

        auto add5 = add_curried(5);
        auto mul3 = mul_curried(3);

        // (10 + 5) * 3 = 45
        REQUIRE(mul3(add5(10)) == 45);
    }
}

// Note: Move-only types (like unique_ptr) cannot be used with partial application
// because std::bind_front requires the callable and bound arguments to be copyable.
// Move-only types work only when all arguments are provided at once:
//
// TEST_CASE("curry with move-only types", "[curry]") {
//     SECTION("curry with unique_ptr - all args at once") {
//         auto lambda = [](std::unique_ptr<int> p, int x) {
//             return *p + x;
//         };
//         auto curried = fxt::curry(lambda);
//         // This works: all arguments provided at once
//         REQUIRE(curried(std::make_unique<int>(100), 23) == 123);
//
//         // This does NOT work: partial application with move-only type
//         // auto partial = curried(std::make_unique<int>(100));
//         // REQUIRE(partial(23) == 123);  // Won't compile
//     }
// }
