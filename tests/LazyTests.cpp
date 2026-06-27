//
// Created by kenne on 18/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>
#include <stdexcept>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

using namespace std::string_literals;

// Helper functions and counters for testing
static std::atomic<int> call_counter{0};

int expensive_computation() {
    call_counter++;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 42;
}

int multiply_by_two(int x) {
    call_counter++;
    return x * 2;
}

std::string create_string() {
    call_counter++;
    return "Hello, Lazy!"s;
}

void throwing_function() {
    call_counter++;
    throw std::runtime_error("Intentional error");
}

int throwing_int_function() {
    call_counter++;
    throw std::invalid_argument("Cannot compute");
    return 0; // Never reached
}

TEST_CASE("fxt::lazy basic construction and evaluation", "[lazy][construction]")
{
    SECTION("Construct from lambda")
    {
        call_counter = 0;
        fxt::lazy val = []{ return 42; };

        REQUIRE(call_counter == 0); // Not evaluated yet
        int result = val;
        REQUIRE(result == 42);
        REQUIRE(call_counter == 0); // Lambda doesn't increment counter
    }

    SECTION("Construct from function pointer")
    {
        call_counter = 0;
        fxt::lazy val = expensive_computation;

        REQUIRE(call_counter == 0); // Not evaluated yet
        int result = val;
        REQUIRE(result == 42);
        REQUIRE(call_counter == 1); // Evaluated once
    }

    SECTION("Construct from function object")
    {
        struct Functor {
            int operator()() const { return 123; }
        };

        fxt::lazy val = Functor{};
        int result = val;
        REQUIRE(result == 123);
    }

    SECTION("Construct with captured state")
    {
        int multiplier = 10;
        fxt::lazy val = [multiplier]{ return 42 * multiplier; };

        int result = val;
        REQUIRE(result == 420);
    }

    SECTION("CTAD works correctly")
    {
        fxt::lazy val = []{ return 3.14; };
        static_assert(std::same_as<decltype(val)::value_type, double>);
        double result = val;
        REQUIRE(result == 3.14);
    }
}

TEST_CASE("fxt::lazy memoization", "[lazy][memoization]")
{
    SECTION("Value is computed only once")
    {
        call_counter = 0;
        fxt::lazy val = expensive_computation;

        REQUIRE(call_counter == 0);
        int result1 = val;
        REQUIRE(call_counter == 1);
        int result2 = val;
        REQUIRE(call_counter == 1); // Still 1, not recomputed
        int result3 = val;
        REQUIRE(call_counter == 1); // Still 1

        REQUIRE(result1 == 42);
        REQUIRE(result2 == 42);
        REQUIRE(result3 == 42);
    }

    SECTION("Multiple access methods return same cached value")
    {
        call_counter = 0;
        fxt::lazy val = expensive_computation;

        int result1 = val; // Implicit conversion
        int result2 = val.value(); // Explicit value()
        int result3 = val(); // Function call operator

        REQUIRE(call_counter == 1); // Computed only once
        REQUIRE(result1 == 42);
        REQUIRE(result2 == 42);
        REQUIRE(result3 == 42);
    }
}

TEST_CASE("fxt::lazy copy semantics", "[lazy][copy]")
{
    SECTION("Explicit copy shares computation state")
    {
        call_counter = 0;
        fxt::lazy val1 = expensive_computation;

        auto val2 = fxt::lazy(val1); // Explicit copy

        REQUIRE(call_counter == 0);
        int result1 = val1;
        REQUIRE(call_counter == 1);
        int result2 = val2;
        REQUIRE(call_counter == 1); // Still 1, shared computation

        REQUIRE(result1 == 42);
        REQUIRE(result2 == 42);
    }

    SECTION("Copy assignment shares computation state")
    {
        call_counter = 0;
        fxt::lazy val1 = expensive_computation;
        fxt::lazy val2 = expensive_computation; // Same type

        val2 = val1; // Copy assignment

        REQUIRE(call_counter == 0);
        int result1 = val1;
        REQUIRE(call_counter == 1);
        int result2 = val2;
        REQUIRE(call_counter == 1); // Shared

        REQUIRE(result1 == 42);
        REQUIRE(result2 == 42);
    }

    SECTION("Copied lazy objects share cached result")
    {
        call_counter = 0;
        fxt::lazy val1 = expensive_computation;

        // Evaluate first
        int result1 = val1;
        REQUIRE(call_counter == 1);

        // Copy after evaluation
        auto val2 = fxt::lazy(val1);

        // Second should use cached result
        int result2 = val2;
        REQUIRE(call_counter == 1); // No additional computation
        REQUIRE(result2 == 42);
    }
}

TEST_CASE("fxt::lazy move semantics", "[lazy][move]")
{
    SECTION("Move transfers ownership")
    {
        call_counter = 0;
        fxt::lazy val1 = expensive_computation;

        auto val2 = fxt::lazy(std::move(val1)); // Explicit move

        int result = val2;
        REQUIRE(result == 42);
        REQUIRE(call_counter == 1);
    }

    SECTION("Moved-from lazy throws on access")
    {
        fxt::lazy val1 = expensive_computation;
        auto val2 = fxt::lazy(std::move(val1));

        // Accessing moved-from object should throw
        REQUIRE_THROWS_AS(val1.value(), std::runtime_error);
        REQUIRE_THROWS_WITH(val1.value(), "lazy object has been moved from");
    }

    SECTION("Move assignment works correctly")
    {
        call_counter = 0;
        fxt::lazy val1 = expensive_computation;
        fxt::lazy val2 = expensive_computation; // Same type

        val2 = fxt::lazy(std::move(val1)); // Move assignment

        int result = val2;
        REQUIRE(result == 42);
        REQUIRE(call_counter == 1);
    }
}

TEST_CASE("fxt::lazy exception handling", "[lazy][exceptions]")
{
    SECTION("Exception is captured and rethrown")
    {
        call_counter = 0;
        fxt::lazy val = throwing_int_function;

        REQUIRE_THROWS_AS(val.value(), std::invalid_argument);
        REQUIRE(call_counter == 1);
    }

    SECTION("Exception is cached and rethrown on subsequent accesses")
    {
        call_counter = 0;
        fxt::lazy val = throwing_int_function;

        REQUIRE_THROWS_AS(val.value(), std::invalid_argument);
        REQUIRE(call_counter == 1);

        // Second access should rethrow cached exception without recomputing
        REQUIRE_THROWS_AS(val.value(), std::invalid_argument);
        REQUIRE(call_counter == 1); // Still 1

        // Third access
        REQUIRE_THROWS_AS(val(), std::invalid_argument);
        REQUIRE(call_counter == 1); // Still 1
    }

    SECTION("Exception message is preserved")
    {
        fxt::lazy val = throwing_int_function;

        REQUIRE_THROWS_WITH(val.value(), "Cannot compute");
    }

    SECTION("Different exception types are handled")
    {
        fxt::lazy val1 = []() -> int { throw std::runtime_error("Runtime"); return 0; };
        fxt::lazy val2 = []() -> int { throw std::logic_error("Logic"); return 0; };
        fxt::lazy val3 = []() -> int { throw std::invalid_argument("Invalid"); return 0; };

        REQUIRE_THROWS_AS(val1.value(), std::runtime_error);
        REQUIRE_THROWS_AS(val2.value(), std::logic_error);
        REQUIRE_THROWS_AS(val3.value(), std::invalid_argument);
    }

    SECTION("Copied lazy shares cached exception")
    {
        call_counter = 0;
        fxt::lazy val1 = throwing_int_function;

        // Trigger exception
        REQUIRE_THROWS(val1.value());
        REQUIRE(call_counter == 1);

        // Copy
        auto val2 = fxt::lazy(val1);

        // Exception should be shared
        REQUIRE_THROWS(val2.value());
        REQUIRE(call_counter == 1); // No recomputation
    }
}

TEST_CASE("fxt::lazy access methods", "[lazy][access]")
{
    SECTION("Implicit conversion operator works")
    {
        fxt::lazy val = []{ return 100; };
        int result = val; // Implicit conversion
        REQUIRE(result == 100);
    }

    SECTION("value() method works")
    {
        fxt::lazy val = []{ return 200; };
        int result = val.value();
        REQUIRE(result == 200);
    }

    SECTION("Function call operator works")
    {
        fxt::lazy val = []{ return 300; };
        int result = val();
        REQUIRE(result == 300);
    }

    SECTION("All access methods return the same value")
    {
        int count = 0;
        fxt::lazy val = [&count]{ ++count; return std::string("test"); };

        std::string v1 = val;
        std::string v2 = val.value();
        std::string v3 = val();

        REQUIRE(v1 == "test");
        REQUIRE(v2 == "test");
        REQUIRE(v3 == "test");
        // Callable must have been invoked exactly once despite three accesses
        REQUIRE(count == 1);
    }

    SECTION("Function call operator returns value")
    {
        fxt::lazy val = []{ return std::string("test"); };
        std::string copy = val();
        REQUIRE(copy == "test");
    }
}

TEST_CASE("fxt::lazy with different return types", "[lazy][types]")
{
    SECTION("int type")
    {
        fxt::lazy val = []{ return 42; };
        REQUIRE(val.value() == 42);
    }

    SECTION("double type")
    {
        fxt::lazy val = []{ return 3.14159; };
        REQUIRE(val.value() == 3.14159);
    }

    SECTION("std::string type")
    {
        fxt::lazy val = []{ return "Hello"s; };
        REQUIRE(val.value() == "Hello");
    }

    SECTION("std::vector type")
    {
        fxt::lazy val = []{ return std::vector{1, 2, 3, 4, 5}; };
        auto result = val.value();
        REQUIRE(result.size() == 5);
        REQUIRE(result[0] == 1);
        REQUIRE(result[4] == 5);
    }

    SECTION("Custom struct type")
    {
        struct Point { int x, y; };
        fxt::lazy val = []{ return Point{10, 20}; };
        auto result = val.value();
        REQUIRE(result.x == 10);
        REQUIRE(result.y == 20);
    }

    SECTION("Pointer type")
    {
        fxt::lazy val = []{ return new int(42); };
        int* ptr = val.value();
        REQUIRE(*ptr == 42);
        delete ptr;
    }

}

TEST_CASE("fxt::lazy thread safety", "[lazy][threading]")
{
    SECTION("Multiple threads access same lazy value safely")
    {
        call_counter = 0;
        fxt::lazy val = expensive_computation;

        std::vector<std::thread> threads;
        std::vector<int> results(10);

        // Launch 10 threads simultaneously
        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&val, &results, i]() {
                results[i] = val.value();
            });
        }

        // Wait for all threads
        for (auto& t : threads) {
            t.join();
        }

        // Computation should happen only once
        REQUIRE(call_counter == 1);

        // All threads should get the same result
        for (int result : results) {
            REQUIRE(result == 42);
        }
    }

    SECTION("Race condition does not cause multiple evaluations")
    {
        call_counter = 0;
        fxt::lazy val = expensive_computation;

        // Launch many threads with no delay
        std::vector<std::thread> threads;
        for (int i = 0; i < 100; ++i) {
            threads.emplace_back([&val]() {
                (void)val.value();
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        // Should still only compute once
        REQUIRE(call_counter == 1);
    }

    SECTION("Exception in one thread is seen by all threads")
    {
        call_counter = 0;
        fxt::lazy val = throwing_int_function;

        std::atomic<int> exception_count{0};
        std::vector<std::thread> threads;

        for (int i = 0; i < 10; ++i) {
            threads.emplace_back([&val, &exception_count]() {
                try {
                    (void)val.value();
                } catch (const std::invalid_argument&) {
                    exception_count++;
                }
            });
        }

        for (auto& t : threads) {
            t.join();
        }

        // Function called only once
        REQUIRE(call_counter == 1);
        // All threads should have caught the exception
        REQUIRE(exception_count == 10);
    }
}

TEST_CASE("fxt::lazy with stateful lambdas", "[lazy][lambda]")
{
    SECTION("Captured values are preserved")
    {
        int x = 10;
        int y = 20;
        fxt::lazy val = [x, y]{ return x + y; };

        // Change the captured variables
        x = 100;
        y = 200;

        // Should use captured values, not current values
        REQUIRE(val.value() == 30);
    }

    SECTION("Mutable lambda state is preserved")
    {
        int counter = 0;
        fxt::lazy val = [counter]() mutable { return ++counter; };

        int result1 = val.value();
        int result2 = val.value();

        // Should return the same cached value
        REQUIRE(result1 == 1);
        REQUIRE(result2 == 1); // Not 2, because it's cached
    }

    SECTION("Reference capture works")
    {
        int x = 42;
        fxt::lazy val = [&x]{ return x; };

        // Value is captured by reference at evaluation time
        REQUIRE(val.value() == 42);

        x = 100;
        // But once evaluated, the result is cached
        REQUIRE(val.value() == 42); // Still 42, not 100
    }
}

TEST_CASE("fxt::lazy edge cases", "[lazy][edge]")
{
    SECTION("Lazy with zero-size lambda")
    {
        fxt::lazy val = []{ return 42; };
        REQUIRE(val.value() == 42);
    }

    SECTION("Lazy with large captured state")
    {
        std::vector<int> large_vec(1000, 42);
        fxt::lazy val = [large_vec]{ return large_vec.size(); };
        REQUIRE(val.value() == 1000);
    }

    SECTION("Lazy returning large object is cached")
    {
        // Test that lazy works with large objects
        fxt::lazy val = []{
            return std::vector<int>(1000, 42);
        };

        // Access returns const reference to cached object
        const auto& vec = val.value();
        REQUIRE(vec.size() == 1000);
        REQUIRE(vec[0] == 42);
    }

    SECTION("Nested lazy evaluations")
    {
        fxt::lazy outer = []{
            fxt::lazy inner = []{ return 10; };
            return inner.value() * 2;
        };

        REQUIRE(outer.value() == 20);
    }

    SECTION("Lazy in container")
    {
        std::vector<fxt::lazy<decltype(+[]{ return 1; })>> vec;
        vec.push_back(fxt::lazy(+[]{ return 1; }));
        vec.push_back(fxt::lazy(+[]{ return 2; }));
        vec.push_back(fxt::lazy(+[]{ return 3; }));

        REQUIRE(vec[0].value() == 1);
        REQUIRE(vec[1].value() == 2);
        REQUIRE(vec[2].value() == 3);
    }
}

TEST_CASE("fxt::lazy performance characteristics", "[lazy][performance]")
{
    SECTION("Lazy evaluation defers computation")
    {
        call_counter = 0;

        // Create many lazy values
        std::vector<fxt::lazy<decltype(expensive_computation)*>> lazy_vals;
        for (int i = 0; i < 100; ++i) {
            lazy_vals.push_back(expensive_computation);
        }

        // No computations yet
        REQUIRE(call_counter == 0);

        // Access only a few
        (void)lazy_vals[0].value();
        (void)lazy_vals[50].value();

        // Only those accessed should be computed
        REQUIRE(call_counter == 2);
    }

    SECTION("Cached access is fast")
    {
        fxt::lazy val = expensive_computation;

        // First access is slow
        auto start1 = std::chrono::steady_clock::now();
        (void)val.value();
        auto end1 = std::chrono::steady_clock::now();
        auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);

        // Second access is fast (cached)
        auto start2 = std::chrono::steady_clock::now();
        (void)val.value();
        auto end2 = std::chrono::steady_clock::now();
        auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(end2 - start2);

        // Cached access should be much faster (microseconds vs milliseconds)
        REQUIRE(duration1.count() >= 10); // At least 10ms
        REQUIRE(duration2.count() < 1000); // Less than 1ms (1000 microseconds)
    }
}
