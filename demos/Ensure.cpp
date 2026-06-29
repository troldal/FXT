//
// Created by kenne on 28/06/2026.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Domain types used throughout the demo
struct Temperature {
    double celsius;
};

struct Measurement {
    double value;
    std::string unit;
};

// Simulate reading a sensor value that may fail
fxt::expected<double, std::string> read_sensor(int id) {
    if (id == 1) return 98.6;
    if (id == 2) return -5.0;
    if (id == 3) return 150.0;
    return fxt::unexpected<std::string>{"sensor not found"};
}

int main() {
    std::cout << "=== fxt::ensure Demonstration ===\n\n";

    // -------------------------------------------------------------------------
    std::cout << "--- Basic usage ---\n";

    // Example 1: Value satisfies the condition — passed through unchanged
    fxt::expected<int, std::string> v1 = 42;
    auto r1 = v1 | fxt::ensure([](int x) { return x > 0; }, std::string{"must be positive"});
    std::cout << "ensure(42 > 0):   " << (r1.has_value() ? std::to_string(*r1) : r1.error()) << "\n";

    // Example 2: Value violates the condition — replaced with the error
    fxt::expected<int, std::string> v2 = -3;
    auto r2 = v2 | fxt::ensure([](int x) { return x > 0; }, std::string{"must be positive"});
    std::cout << "ensure(-3 > 0):   " << (r2.has_value() ? std::to_string(*r2) : r2.error()) << "\n";

    // Example 3: Pre-existing error — propagated unchanged, predicate never called
    bool called = false;
    fxt::expected<int, std::string> v3 = fxt::unexpected<std::string>{"upstream error"};
    auto r3 = v3 | fxt::ensure([&called](int) { called = true; return true; },
                                std::string{"new error"});
    std::cout << "ensure(error):    " << r3.error()
              << "  (predicate called: " << (called ? "yes" : "no") << ")\n";

    // -------------------------------------------------------------------------
    std::cout << "\n--- Reusable ensure adaptors ---\n";

    auto positive   = fxt::ensure([](double x) { return x > 0.0;   }, std::string{"must be positive"});
    auto below_100  = fxt::ensure([](double x) { return x < 100.0; }, std::string{"must be below 100"});
    auto finite     = fxt::ensure([](double x) { return x == x;    }, std::string{"must be finite (not NaN)"});

    for (int id : {1, 2, 3, 4}) {
        auto r = read_sensor(id) | positive;
        std::cout << "  sensor " << id << " | positive:  ";
        if (r.has_value()) std::cout << *r << "\n";
        else               std::cout << "error: " << r.error() << "\n";
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- Validation pipeline (flagship use case) ---\n";
    std::cout << "    Rules: value must be positive, below 100, and finite\n\n";

    auto validate_reading = [&](int sensor_id) {
        return read_sensor(sensor_id)
             | positive
             | below_100
             | finite;
    };

    for (int id : {1, 2, 3, 4}) {
        auto r = validate_reading(id);
        std::cout << "  sensor " << id << ": ";
        if (r.has_value()) std::cout << *r << "  (ok)\n";
        else               std::cout << "rejected — " << r.error() << "\n";
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- ensure in a larger pipeline ---\n";

    // Parse → validate → transform → format
    auto process = [](fxt::expected<double, std::string> raw) {
        return raw
             | fxt::ensure([](double x) { return x >= 0.0;   }, std::string{"negative value"})
             | fxt::ensure([](double x) { return x <= 200.0; }, std::string{"value out of range"})
             | fxt::transform([](double x) { return x * 9.0 / 5.0 + 32.0; })   // °C → °F
             | fxt::transform([](double f) {
                   return "Temperature: " + std::to_string(f) + " °F";
               });
    };

    for (int id : {1, 2, 3, 4}) {
        auto r = process(read_sensor(id));
        std::cout << "  sensor " << id << ": ";
        if (r.has_value()) std::cout << *r << "\n";
        else               std::cout << "error: " << r.error() << "\n";
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- ensure vs filter: same mechanics, different intent ---\n";

    fxt::expected<int, std::string> value = 7;

    // filter: "keep only even numbers" — data selection
    auto filtered = value | fxt::filter([](int x) { return x % 2 == 0; },
                                        std::string{"not even"});

    // ensure: "this value must be even" — precondition / assertion
    auto ensured  = value | fxt::ensure([](int x) { return x % 2 == 0; },
                                        std::string{"value must be even"});

    std::cout << "  filter(7, even): "
              << (filtered.has_value() ? std::to_string(*filtered) : filtered.error()) << "\n";
    std::cout << "  ensure(7, even): "
              << (ensured.has_value()  ? std::to_string(*ensured)  : ensured.error())  << "\n";
    std::cout << "  (same result, different reading of the code)\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
