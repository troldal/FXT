//
// Created by kenne on 18/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std::string_literals;

// Example struct for demonstration
struct Config {
    std::string name;
    int version;
    std::vector<std::string> features;

    void print() const {
        std::cout << "Config: " << name << " v" << version << "\n";
        std::cout << "Features: ";
        for (const auto& f : features) {
            std::cout << f << " ";
        }
        std::cout << "\n";
    }
};

void demonstrate_basic_usage() {
    std::cout << "=== Basic Usage ===\n\n";

    // Build an object mutably
    std::vector<int> data;
    data.push_back(1);
    data.push_back(2);
    data.push_back(3);
    data.push_back(4);
    data.push_back(5);

    std::cout << "Built mutable vector with " << data.size() << " elements\n";

    // Wrap it in immutable to prevent modification
    fxt::immutable<std::vector<int>> immut_data(std::move(data));

    // Can read but not modify
    std::cout << "Immutable vector size: " << immut_data->size() << "\n";
    std::cout << "First element: " << (*immut_data)[0] << "\n";

    // This would be a compile error:
    // immut_data->push_back(6);  // Error: push_back() is not const

    std::cout << "\n";
}

void demonstrate_gradual_construction() {
    std::cout << "=== Gradual Construction Pattern ===\n\n";

    // Build a complex object step by step
    Config config;
    config.name = "MyApp";
    config.version = 1;
    config.features.push_back("feature1");
    config.features.push_back("feature2");
    config.features.push_back("feature3");

    std::cout << "Built config mutably:\n";
    config.print();

    // Now wrap it to make it immutable
    fxt::immutable<Config> immut_config(std::move(config));

    std::cout << "\nWrapped in immutable - can read but not modify:\n";
    immut_config->print();

    // Can access const members
    std::cout << "Config name: " << immut_config->name << "\n";

    // This would be a compile error:
    // immut_config->features.push_back("feature4");  // Error: not const

    std::cout << "\n";
}

void demonstrate_reassignment() {
    std::cout << "=== Reassignment (wrapper is not const) ===\n\n";

    // Create first immutable value
    fxt::immutable<std::string> message("Hello");
    std::cout << "Initial message: " << *message << "\n";

    // The wrapper can be reassigned (unlike const variable)
    message = fxt::immutable<std::string>("World"s);
    std::cout << "After reassignment: " << *message << "\n";

    // Can assign from another immutable
    auto another = fxt::immutable<std::string>("Goodbye"s);
    message = another;
    std::cout << "After copy assignment: " << *message << "\n";

    std::cout << "\n";
}

void demonstrate_move_semantics() {
    std::cout << "=== Move Semantics ===\n\n";

    // Create an immutable with large data
    std::vector<int> large_data(1000, 42);
    fxt::immutable<std::vector<int>> immut_large(std::move(large_data));

    std::cout << "Created immutable with " << immut_large->size() << " elements\n";

    // Move the wrapper (efficient, no copy of data)
    auto moved = std::move(immut_large);
    std::cout << "Moved to new wrapper: " << moved->size() << " elements\n";

    // Extract value from rvalue using explicit method
    std::vector<int> extracted = std::move(moved).extract();
    std::cout << "Extracted value: " << extracted.size() << " elements\n";

    // Alternative: implicit conversion from rvalue
    fxt::immutable<std::vector<int>> another({1, 2, 3});
    std::vector<int> extracted2 = std::move(another);
    std::cout << "Extracted via implicit conversion: " << extracted2.size() << " elements\n";

    std::cout << "\n";
}

void demonstrate_comparison() {
    std::cout << "=== Comparison Operations ===\n\n";

    fxt::immutable<int> a(10);
    fxt::immutable<int> b(20);
    fxt::immutable<int> c(10);

    std::cout << "a = " << *a << ", b = " << *b << ", c = " << *c << "\n";

    std::cout << "a == c: " << (a == c ? "true" : "false") << "\n";
    std::cout << "a == b: " << (a == b ? "true" : "false") << "\n";
    std::cout << "a < b: " << (a < b ? "true" : "false") << "\n";
    std::cout << "b < a: " << (b < a ? "true" : "false") << "\n";

    std::cout << "\n";
}

void demonstrate_container_usage() {
    std::cout << "=== Using in Containers ===\n\n";

    // Vector of immutable values
    std::vector<fxt::immutable<std::string>> immut_strings;
    immut_strings.push_back(fxt::immutable<std::string>("first"s));
    immut_strings.push_back(fxt::immutable<std::string>("second"s));
    immut_strings.push_back(fxt::immutable<std::string>("third"s));

    std::cout << "Vector of immutable strings:\n";
    for (const auto& s : immut_strings) {
        std::cout << "  - " << *s << "\n";
    }

    // Map with immutable values
    std::map<std::string, fxt::immutable<int>> config_map;
    config_map.emplace("max_connections", fxt::immutable<int>(100));
    config_map.emplace("timeout", fxt::immutable<int>(30));
    config_map.emplace("retries", fxt::immutable<int>(3));

    std::cout << "\nMap with immutable values:\n";
    for (const auto& [key, value] : config_map) {
        std::cout << "  " << key << ": " << *value << "\n";
    }

    std::cout << "\n";
}

void demonstrate_function_parameters() {
    std::cout << "=== Function Parameters ===\n\n";

    // Function that takes immutable by const reference
    auto process = [](const fxt::immutable<std::vector<int>>& data) {
        std::cout << "Processing " << data->size() << " elements\n";
        int sum = 0;
        for (int x : *data) {
            sum += x;
        }
        std::cout << "Sum: " << sum << "\n";
    };

    // Build data mutably
    std::vector<int> data = {1, 2, 3, 4, 5};

    // Wrap and pass
    fxt::immutable<std::vector<int>> immut_data(std::move(data));
    process(immut_data);

    // Function can't modify the data, even if it wanted to
    // This ensures data integrity

    std::cout << "\n";
}

void demonstrate_vs_const() {
    std::cout << "=== Comparison with const keyword ===\n\n";

    // With const: can't reassign the variable OR modify the value
    const std::vector<int> const_vec = {1, 2, 3};
    std::cout << "const vector size: " << const_vec.size() << "\n";
    // const_vec = {4, 5, 6};  // Error: can't reassign
    // const_vec.push_back(4); // Error: can't modify

    // With immutable: CAN reassign the variable, but CAN'T modify the value
    fxt::immutable<std::vector<int>> immut_vec({1, 2, 3});
    std::cout << "immutable vector size: " << immut_vec->size() << "\n";

    immut_vec = fxt::immutable<std::vector<int>>({4, 5, 6});  // OK: can reassign
    std::cout << "After reassignment: " << immut_vec->size() << "\n";
    // immut_vec->push_back(7);  // Error: can't modify value

    std::cout << "\nKey difference:\n";
    std::cout << "- const: variable AND value are immutable\n";
    std::cout << "- immutable<T>: only the VALUE is immutable, wrapper is mutable\n";

    std::cout << "\n";
}

int main() {
    std::cout << "========================================\n";
    std::cout << "     fxt::immutable Demo Program\n";
    std::cout << "========================================\n\n";

    demonstrate_basic_usage();
    demonstrate_gradual_construction();
    demonstrate_reassignment();
    demonstrate_move_semantics();
    demonstrate_comparison();
    demonstrate_container_usage();
    demonstrate_function_parameters();
    demonstrate_vs_const();

    std::cout << "========================================\n";
    std::cout << "Demo completed successfully!\n";
    std::cout << "========================================\n";

    return 0;
}
