//
// Created by AI Assistant
//

#include <fxt.hpp>
#include <cmath>
#include <iostream>
#include <string>

// Helper function to simulate a database lookup
fxt::optional<std::string> find_user(int id)
{
    if (id == 1) return "Alice";
    if (id == 2) return "Bob";
    if (id == 3) return "Charlie";
    return {};
}

// Helper function to simulate file reading
fxt::expected<std::string, std::string> read_file(const std::string& filename)
{
    if (filename == "config.txt") {
        return "setting1=value1\nsetting2=value2";
    }
    if (filename == "data.txt") {
        return "42,100,200";
    }
    return fxt::unexpected("File not found: " + filename);
}

// Helper function to parse an integer
fxt::optional<int> parse_int(const std::string& str)
{
    try {
        return std::stoi(str);
    }
    catch (...) {
        return {};
    }
}

// Helper function for division
fxt::expected<double, std::string> divide(double a, double b)
{
    if (b == 0.0) {
        return fxt::unexpected("Division by zero");
    }
    return a / b;
}

// Helper function to calculate square root
fxt::expected<double, std::string> safe_sqrt(double x)
{
    if (x < 0.0) {
        return fxt::unexpected("Cannot take square root of negative number");
    }
    return std::sqrt(x);
}

void demonstrate_optional_basic()
{
    std::cout << "=== Basic fxt::optional with fxt::value ===" << std::endl;

    // Success case - extracting value with pipe operator
    fxt::optional<int> opt1 = 42;
    int value1 = opt1 | fxt::value();
    std::cout << "opt1 | fxt::value() = " << value1 << std::endl;

    // With string
    fxt::optional<std::string> opt2 = "Hello, World!";
    std::string value2 = opt2 | fxt::value();
    std::cout << "opt2 | fxt::value() = " << value2 << std::endl;

    // Using find_user
    auto user = find_user(1) | fxt::value();
    std::cout << "User 1: " << user << std::endl;

    // Error case - attempting to extract from empty optional
    std::cout << "\nAttempting to extract from empty optional..." << std::endl;
    try {
        fxt::optional<int> empty;
        int value = empty | fxt::value();
        std::cout << "Value: " << value << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: bad optional access" << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_expected_basic()
{
    std::cout << "=== Basic fxt::expected with fxt::value ===" << std::endl;

    // Success case
    fxt::expected<int, std::string> exp1 = 100;
    int value1 = exp1 | fxt::value();
    std::cout << "exp1 | fxt::value() = " << value1 << std::endl;

    // Using read_file
    auto content = read_file("config.txt") | fxt::value();
    std::cout << "File content:\n" << content << std::endl;

    // Error case - attempting to extract from expected with error
    std::cout << "\nAttempting to extract from expected with error..." << std::endl;
    try {
        auto content = read_file("missing.txt") | fxt::value();
        std::cout << "Content: " << content << std::endl;
    }
    catch (const std::exception& e) {
        std::cout << "Exception caught: bad expected access" << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_chaining_with_transform()
{
    std::cout << "=== Chaining with transform and value ===" << std::endl;

    // Optional chaining
    fxt::optional<int> opt = 5;
    auto result1 = opt
        | fxt::transform([](int x) { return x * x; })
        | fxt::transform([](int x) { return x + 10; })
        | fxt::value();
    std::cout << "5 -> square -> +10 = " << result1 << std::endl;

    // Expected chaining
    fxt::expected<double, std::string> exp = 16.0;
    auto result2 = exp
        | fxt::transform([](double x) { return std::sqrt(x); })
        | fxt::transform([](double x) { return x * 2; })
        | fxt::value();
    std::cout << "16 -> sqrt -> *2 = " << result2 << std::endl;

    std::cout << std::endl;
}

void demonstrate_chaining_with_and_then()
{
    std::cout << "=== Chaining with and_then and value ===" << std::endl;

    // Optional chaining with and_then
    auto result1 = parse_int("123")
        | fxt::and_then([](int x) -> fxt::optional<int> {
            return x > 100 ? fxt::optional<int>(x * 2) : fxt::optional<int>();
        })
        | fxt::value();
    std::cout << "parse('123') -> filter(>100) -> *2 = " << result1 << std::endl;

    // Expected chaining with and_then
    auto result2 = divide(100.0, 4.0)
        | fxt::and_then([](double x) { return safe_sqrt(x); })
        | fxt::value();
    std::cout << "100/4 -> sqrt = " << result2 << std::endl;

    std::cout << std::endl;
}

void demonstrate_practical_example()
{
    std::cout << "=== Practical Example: User Processing Pipeline ===" << std::endl;

    // Process user data through a pipeline
    auto process_user = [](int user_id) {
        std::cout << "Processing user ID: " << user_id << std::endl;

        auto user_name = find_user(user_id)
            | fxt::transform([](const std::string& name) {
                std::cout << "  Found user: " << name << std::endl;
                return name;
            })
            | fxt::transform([](const std::string& name) {
                return "User: " + name;
            })
            | fxt::value();

        std::cout << "  Result: " << user_name << std::endl;
    };

    process_user(1);
    process_user(2);

    std::cout << "\nProcessing non-existent user:" << std::endl;
    try {
        process_user(999);
    }
    catch (const std::exception& e) {
        std::cout << "  Error: User not found" << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_mathematical_pipeline()
{
    std::cout << "=== Mathematical Pipeline with expected ===" << std::endl;

    auto calculate = [](double a, double b, double c) {
        std::cout << "Calculating: (" << a << " / " << b << ") then sqrt, then * " << c << std::endl;

        try {
            auto result = divide(a, b)
                | fxt::and_then([](double x) { return safe_sqrt(x); })
                | fxt::transform([c](double x) { return x * c; })
                | fxt::value();

            std::cout << "  Result: " << result << std::endl;
            return true;
        }
        catch (const std::exception& e) {
            std::cout << "  Error: Computation failed" << std::endl;
            return false;
        }
    };

    calculate(100.0, 4.0, 2.0);  // Success: (100/4) -> sqrt(25) = 5 -> 5*2 = 10
    calculate(100.0, 0.0, 2.0);  // Error: division by zero
    calculate(100.0, -4.0, 2.0); // Error: sqrt of negative

    std::cout << std::endl;
}

void demonstrate_direct_call_syntax()
{
    std::cout << "=== Direct Call Syntax (without pipe operator) ===" << std::endl;

    // Optional
    fxt::optional<int> opt = 42;
    int value1 = fxt::value()(opt);
    std::cout << "fxt::value()(opt) = " << value1 << std::endl;

    // Expected
    fxt::expected<std::string, int> exp = "Success";
    std::string value2 = fxt::value()(exp);
    std::cout << "fxt::value()(exp) = " << value2 << std::endl;

    // With temporaries
    int value3 = fxt::value()(fxt::optional<int>(100));
    std::cout << "fxt::value()(temporary opt) = " << value3 << std::endl;

    int value4 = fxt::value()(fxt::expected<int, std::string>(200));
    std::cout << "fxt::value()(temporary exp) = " << value4 << std::endl;

    std::cout << std::endl;
}

void demonstrate_comparison_with_value_or()
{
    std::cout << "=== Comparison: value vs value_or ===" << std::endl;

    fxt::optional<int> opt1 = 42;
    fxt::optional<int> opt2;

    // Using value - throws on empty
    std::cout << "opt1 | fxt::value() = " << (opt1 | fxt::value()) << std::endl;

    // Using value_or - returns default on empty
    std::cout << "opt2 | fxt::value_or(0) = " << (opt2 | fxt::value_or(0)) << std::endl;

    std::cout << "\nNote: fxt::value() throws an exception on empty/error," << std::endl;
    std::cout << "      while fxt::value_or returns a default value." << std::endl;

    std::cout << std::endl;
}

void demonstrate_const_correctness()
{
    std::cout << "=== Const Correctness ===" << std::endl;

    // Const optional
    const fxt::optional<int> const_opt = 42;
    int value1 = const_opt | fxt::value();
    std::cout << "const_opt | fxt::value() = " << value1 << std::endl;

    // Const expected
    const fxt::expected<std::string, int> const_exp = "Hello";
    std::string value2 = const_exp | fxt::value();
    std::cout << "const_exp | fxt::value() = " << value2 << std::endl;

    // Value can be extracted multiple times
    int value3 = const_opt | fxt::value();
    int value4 = const_opt | fxt::value();
    std::cout << "Multiple extractions: " << value3 << ", " << value4 << std::endl;

    std::cout << std::endl;
}

int main()
{
    std::cout << "FXT Value Demo" << std::endl;
    std::cout << "==============" << std::endl << std::endl;

    demonstrate_optional_basic();
    demonstrate_expected_basic();
    demonstrate_chaining_with_transform();
    demonstrate_chaining_with_and_then();
    demonstrate_practical_example();
    demonstrate_mathematical_pipeline();
    demonstrate_direct_call_syntax();
    demonstrate_comparison_with_value_or();
    demonstrate_const_correctness();

    std::cout << "Demo completed successfully!" << std::endl;

    return 0;
}
