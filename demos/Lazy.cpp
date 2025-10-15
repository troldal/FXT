/*
    Demo: fxt::lazy - Lazy Evaluation with Memoization

    This demo showcases the fxt::lazy class template, which provides
    lazy evaluation of callables with thread-safe memoization.
*/

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <fxt.hpp>

// Expensive computation to demonstrate lazy evaluation
int expensive_computation(int n) {
    std::cout << "  [Computing fibonacci(" << n << ")...]\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    if (n <= 1) return n;
    return expensive_computation(n - 1) + expensive_computation(n - 2);
}

// Demo 1: Basic lazy evaluation
void demo_basic_lazy() {
    std::cout << "\n=== Demo 1: Basic Lazy Evaluation ===\n";

    std::cout << "Creating lazy value (not computed yet)...\n";
    fxt::lazy expensive = []{ return expensive_computation(5); };

    std::cout << "Lazy value created. No computation happened yet.\n";
    std::cout << "Accessing value for the first time...\n";
    int result = expensive;  // Computation happens here
    std::cout << "Result: " << result << "\n";

    std::cout << "Accessing value again...\n";
    int result2 = expensive;  // Returns cached value immediately
    std::cout << "Result: " << result2 << " (retrieved from cache)\n";
}

// Demo 2: Shared computation via copying
void demo_shared_computation() {
    std::cout << "\n=== Demo 2: Shared Computation ===\n";

    std::cout << "Creating first lazy value...\n";
    fxt::lazy expensive = []{
        std::cout << "  [Computing expensive result...]\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return 42;
    };

    std::cout << "Copying to second lazy value...\n";
    auto copy = fxt::lazy(expensive);  // Explicit copy

    std::cout << "Accessing first value (triggers computation)...\n";
    int val1 = expensive;
    std::cout << "Result from first: " << val1 << "\n";

    std::cout << "Accessing second value (uses cached result)...\n";
    int val2 = copy;
    std::cout << "Result from second: " << val2 << " (shared computation)\n";
}

// Demo 3: Exception handling
void demo_exception_handling() {
    std::cout << "\n=== Demo 3: Exception Handling ===\n";

    fxt::lazy will_throw = []() -> int {
        std::cout << "  [Computation that throws...]\n";
        throw std::runtime_error("Computation failed!");
    };

    std::cout << "Attempting to access value that throws...\n";
    try {
        int result = will_throw;
        std::cout << "Result: " << result << "\n";
    } catch (const std::exception& e) {
        std::cout << "Caught exception: " << e.what() << "\n";
    }

    std::cout << "Accessing again (exception is cached and rethrown)...\n";
    try {
        int result = will_throw;
        std::cout << "Result: " << result << "\n";
    } catch (const std::exception& e) {
        std::cout << "Caught exception again: " << e.what() << "\n";
    }
}

// Demo 4: Capturing state in lambdas
void demo_stateful_lambdas() {
    std::cout << "\n=== Demo 4: Stateful Lambdas ===\n";

    int multiplier = 10;
    fxt::lazy computed = [multiplier]{
        std::cout << "  [Computing with multiplier=" << multiplier << "...]\n";
        return 42 * multiplier;
    };

    std::cout << "Accessing lazy value with captured state...\n";
    int result = computed;
    std::cout << "Result: " << result << "\n";

    // Note: The multiplier value is captured at creation time
    multiplier = 20;
    std::cout << "Changed multiplier to 20, but lazy still uses captured value...\n";
    int result2 = computed;
    std::cout << "Result: " << result2 << " (still uses multiplier=10)\n";
}

// Demo 5: Using value() and operator!
void demo_access_methods() {
    std::cout << "\n=== Demo 5: Different Access Methods ===\n";

    fxt::lazy compute = []{
        std::cout << "  [Computing...]\n";
        return 3.14159;
    };

    std::cout << "Using implicit conversion:\n";
    double pi1 = compute;
    std::cout << "Result: " << pi1 << "\n";

    std::cout << "\nUsing explicit value() method:\n";
    double pi2 = compute.value();
    std::cout << "Result: " << pi2 << "\n";
}

// Demo 6: Thread safety
void demo_thread_safety() {
    std::cout << "\n=== Demo 6: Thread Safety ===\n";

    std::cout << "Creating lazy value to be accessed from multiple threads...\n";
    fxt::lazy expensive = []{
        std::cout << "  [Computing (this should only appear once)...]\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        return 999;
    };

    std::cout << "Launching 5 threads to access the same lazy value...\n";
    std::vector<std::thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&expensive, i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(i * 100));
            int result = expensive;
            std::cout << "Thread " << i << " got result: " << result << "\n";
        });
    }

    for (auto& t : threads) {
        t.join();
    }
    std::cout << "All threads completed. Computation happened only once!\n";
}

// Demo 7: Lazy with different callable types
void demo_callable_types() {
    std::cout << "\n=== Demo 7: Different Callable Types ===\n";

    // Lambda
    std::cout << "Lambda: ";
    fxt::lazy from_lambda = []{ return 10; };
    std::cout << from_lambda.value() << "\n";

    // Function pointer
    std::cout << "Function pointer: ";
    auto func = +[]() -> int { return 20; };
    fxt::lazy from_function = func;
    std::cout << from_function.value() << "\n";

    // Function object
    std::cout << "Function object: ";
    struct Functor {
        int operator()() const { return 30; }
    };
    fxt::lazy from_functor = Functor{};
    std::cout << from_functor.value() << "\n";
}

// Demo 8: Practical use case - expensive resource initialization
void demo_practical_use_case() {
    std::cout << "\n=== Demo 8: Practical Use Case - Expensive Resource ===\n";

    // Simulate expensive database connection
    fxt::lazy db_connection = []{
        std::cout << "  [Establishing database connection...]\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return std::string("Connected to DB");
    };

    std::cout << "Application started. DB connection not yet established.\n";
    std::cout << "Doing some work that doesn't need DB...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    std::cout << "Now we need the DB connection...\n";
    std::string conn = db_connection;
    std::cout << "Status: " << conn << "\n";

    std::cout << "Using DB connection again (instant access)...\n";
    std::string conn2 = db_connection;
    std::cout << "Status: " << conn2 << "\n";
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║         FXT::LAZY - Lazy Evaluation Demo                   ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";

    try {
        demo_basic_lazy();
        demo_shared_computation();
        demo_exception_handling();
        demo_stateful_lambdas();
        demo_access_methods();
        demo_thread_safety();
        demo_callable_types();
        demo_practical_use_case();

        std::cout << "\n╔════════════════════════════════════════════════════════════╗\n";
        std::cout << "║         All demos completed successfully!                  ║\n";
        std::cout << "╚════════════════════════════════════════════════════════════╝\n";

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

