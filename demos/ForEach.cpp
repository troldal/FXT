//
// Demo: fxt::for_each
// Shows how to use fxt::for_each to apply side effects to range elements
// while passing the range through for further processing.
//

#include <fxt.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>

// ============================================================================
// Example domain types
// ============================================================================

struct Sensor
{
    std::string name;
    double      reading = 0.0;

    void calibrate() { reading *= 1.02; }
};

// ============================================================================
// Main
// ============================================================================

int main()
{
    std::cout << "=== fxt::for_each Demo ===\n\n";

    // -------------------------------------------------------------------------
    // 1. Basic pipe usage — log each element without changing the range
    // -------------------------------------------------------------------------
    std::cout << "1. Logging each element without modifying the range:\n";
    {
        std::vector<int> v = {3, 1, 4, 1, 5, 9};

        auto result = v | fxt::for_each([](int x) {
            std::cout << "   visiting: " << x << '\n';
        });

        std::cout << "   range unchanged: ";
        for (int x : result) std::cout << x << ' ';
        std::cout << '\n';
    }

    // -------------------------------------------------------------------------
    // 2. Mutation — double every element in-place
    // -------------------------------------------------------------------------
    std::cout << "\n2. Mutating elements in-place:\n";
    {
        std::vector<int> v = {1, 2, 3, 4, 5};

        v | fxt::for_each([](int& x) { x *= 2; });

        std::cout << "   after doubling: ";
        for (int x : v) std::cout << x << ' ';
        std::cout << '\n';
    }

    // -------------------------------------------------------------------------
    // 3. Chaining multiple for_each adaptors
    // -------------------------------------------------------------------------
    std::cout << "\n3. Chaining two for_each steps:\n";
    {
        std::vector<int> v = {10, 20, 30};

        auto result = v
            | fxt::for_each([](int x) { std::cout << "   step1: " << x << '\n'; })
            | fxt::for_each([](int x) { std::cout << "   step2: " << x * 2 << '\n'; });

        (void)result;
    }

    // -------------------------------------------------------------------------
    // 4. Accumulation via captured reference
    // -------------------------------------------------------------------------
    std::cout << "\n4. Accumulating a sum via capture:\n";
    {
        std::vector<double> prices = {9.99, 4.49, 14.95, 1.25};
        double total = 0.0;

        prices | fxt::for_each([&total](double p) { total += p; });

        std::cout << "   total: " << total << '\n';
    }

    // -------------------------------------------------------------------------
    // 5. Member-function pointer — std::invoke support
    // -------------------------------------------------------------------------
    std::cout << "\n5. Calling a member function on each element:\n";
    {
        std::vector<Sensor> sensors = {
            {"pressure", 100.0},
            {"temperature", 25.0},
            {"flow", 3.7}
        };

        sensors | fxt::for_each(&Sensor::calibrate);

        std::cout << "   calibrated readings:\n";
        for (auto& s : sensors) {
            std::cout << "   " << s.name << ": " << s.reading << '\n';
        }
    }

    // -------------------------------------------------------------------------
    // 6. Direct-call syntax — lvalue returns reference (no copy)
    // -------------------------------------------------------------------------
    std::cout << "\n6. Direct call with lvalue — returns reference, no copy:\n";
    {
        std::vector<int> v = {7, 8, 9};

        decltype(auto) ref = fxt::for_each([](int& x) { x += 100; })(v);

        std::cout << "   same object? " << (&ref == &v ? "yes" : "no") << '\n';
        std::cout << "   values: ";
        for (int x : v) std::cout << x << ' ';
        std::cout << '\n';
    }

    // -------------------------------------------------------------------------
    // 7. Direct-call syntax — rvalue returns a safe value (no dangling)
    // -------------------------------------------------------------------------
    std::cout << "\n7. Direct call with rvalue — returns a safe moved value:\n";
    {
        std::vector<std::string> collected;

        auto result = fxt::for_each([&collected](const std::string& s) {
            collected.push_back(s);
        })(std::vector<std::string>{"hello", "world"});

        std::cout << "   result: ";
        for (auto& s : result) std::cout << s << ' ';
        std::cout << '\n';
        std::cout << "   collected: ";
        for (auto& s : collected) std::cout << s << ' ';
        std::cout << '\n';
    }

    // -------------------------------------------------------------------------
    // 8. Works with other container types
    // -------------------------------------------------------------------------
    std::cout << "\n8. Works with std::list and other containers:\n";
    {
        std::list<int> lst = {2, 4, 6, 8};

        lst | fxt::for_each([](int& x) { x /= 2; });

        std::cout << "   list after halving: ";
        for (int x : lst) std::cout << x << ' ';
        std::cout << '\n';
    }

    // -------------------------------------------------------------------------
    // 9. Stored adaptor is reusable across multiple ranges
    // -------------------------------------------------------------------------
    std::cout << "\n9. Stored adaptor applied to multiple ranges:\n";
    {
        int count = 0;
        auto counter = fxt::for_each([&count](int) { ++count; });

        std::vector<int> a = {1, 2, 3};
        std::vector<int> b = {4, 5};

        a | counter;
        b | counter;

        std::cout << "   total elements visited: " << count << '\n';
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
