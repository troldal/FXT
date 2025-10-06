//
// Created by AI Assistant
//

#include <fxt/Failure.hpp>
#include <fxt/Expected.hpp>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <unordered_map>

// Helper function to simulate file operations
fxt::expected<std::string, fxt::Failure> read_file(const std::string& filename)
{
    if (filename.empty()) {
        return fxt::unexpected(fxt::Failure("Filename cannot be empty"));
    }

    if (filename == "missing.txt") {
        return fxt::unexpected(fxt::Failure("File not found: " + filename));
    }

    if (filename == "locked.txt") {
        try {
            throw std::runtime_error("Permission denied: file is locked");
        }
        catch (...) {
            return fxt::unexpected(fxt::Failure::from_current_exception());
        }
    }

    return "File contents: Hello from " + filename;
}

// Helper function to simulate parsing operations
fxt::expected<int, fxt::Failure> parse_number(const std::string& str)
{
    if (str.empty()) {
        return fxt::unexpected(fxt::Failure::from_message("Empty string cannot be parsed"));
    }

    try {
        return std::stoi(str);
    }
    catch (const std::exception& e) {
        return fxt::unexpected(fxt::Failure::from_exception(std::current_exception()));
    }
}

// Helper function to demonstrate error propagation
fxt::expected<double, fxt::Failure> divide(double a, double b)
{
    if (b == 0.0) {
        return fxt::unexpected(fxt::Failure("Division by zero"));
    }
    return a / b;
}

void demonstrate_basic_construction()
{
    std::cout << "=== Basic Construction ===" << std::endl;

    // Construct from string message
    fxt::Failure f1("Simple error message");
    std::cout << "f1: " << f1.message() << std::endl;
    std::cout << "f1.what(): " << f1.what() << std::endl;
    std::cout << "Has exception: " << (f1.has_exception() ? "yes" : "no") << std::endl;

    // Construct from exception
    try {
        throw std::runtime_error("Something went wrong!");
    }
    catch (...) {
        fxt::Failure f2(std::current_exception());
        std::cout << "\nf2: " << f2.message() << std::endl;
        std::cout << "Has exception: " << (f2.has_exception() ? "yes" : "no") << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_factory_methods()
{
    std::cout << "=== Factory Methods ===" << std::endl;

    // Using from_message
    auto f1 = fxt::Failure::from_message("Created via factory");
    std::cout << "from_message: " << f1.message() << std::endl;

    // Using from_current_exception
    try {
        throw std::logic_error("Logic error occurred");
    }
    catch (...) {
        auto f2 = fxt::Failure::from_current_exception();
        std::cout << "from_current_exception: " << f2.message() << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_with_expected()
{
    std::cout << "=== Using with fxt::expected ===" << std::endl;

    // Success case
    auto result1 = read_file("data.txt");
    if (result1) {
        std::cout << "Success: " << *result1 << std::endl;
    } else {
        std::cout << "Error: " << result1.error().message() << std::endl;
    }

    // Error cases
    auto result2 = read_file("");
    if (!result2) {
        std::cout << "Error: " << result2.error().message() << std::endl;
    }

    auto result3 = read_file("missing.txt");
    if (!result3) {
        std::cout << "Error: " << result3.error().message() << std::endl;
    }

    auto result4 = read_file("locked.txt");
    if (!result4) {
        fxt::Failure error = result4.error();
        std::cout << "Error: " << error.message() << std::endl;
        std::cout << "Has exception: " << (error.has_exception() ? "yes" : "no") << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_error_propagation()
{
    std::cout << "=== Error Propagation ===" << std::endl;

    // Chain operations
    auto result = parse_number("invalid")
        .and_then([](int num) -> fxt::expected<double, fxt::Failure> {
            return divide(num, 2.0);
        });

    if (!result) {
        std::cout << "Parse error: " << result.error().message() << std::endl;
    }

    // Success chain
    auto result2 = parse_number("42")
        .and_then([](int num) -> fxt::expected<double, fxt::Failure> {
            return divide(num, 2.0);
        });

    if (result2) {
        std::cout << "Division result: " << *result2 << std::endl;
    }

    // Division by zero
    auto result3 = parse_number("10")
        .and_then([](int num) -> fxt::expected<double, fxt::Failure> {
            return divide(num, 0.0);
        });

    if (!result3) {
        std::cout << "Division error: " << result3.error().message() << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_comparison()
{
    std::cout << "=== Comparison Operations ===" << std::endl;

    fxt::Failure f1("Error A");
    fxt::Failure f2("Error A");
    fxt::Failure f3("Error B");

    std::cout << "f1 == f2: " << (f1 == f2 ? "true" : "false") << std::endl;
    std::cout << "f1 == f3: " << (f1 == f3 ? "true" : "false") << std::endl;
    std::cout << "f1 < f3: " << (f1 < f3 ? "true" : "false") << std::endl;
    std::cout << "f3 > f1: " << (f3 > f1 ? "true" : "false") << std::endl;

    std::cout << std::endl;
}

void demonstrate_hash_support()
{
    std::cout << "=== Hash Support ===" << std::endl;

    // Using Failure as a key in unordered_map
    std::unordered_map<fxt::Failure, int> error_counts;

    error_counts[fxt::Failure("Network timeout")] = 5;
    error_counts[fxt::Failure("Invalid input")] = 3;
    error_counts[fxt::Failure("Network timeout")] += 2; // Update count

    std::cout << "Error counts:" << std::endl;
    for (const auto& [error, count] : error_counts) {
        std::cout << "  " << error.message() << ": " << count << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_stream_output()
{
    std::cout << "=== Stream Output ===" << std::endl;

    fxt::Failure f1("Error message");
    std::cout << "Direct output: " << f1 << std::endl;

    // With expected
    auto result = divide(10.0, 0.0);
    if (!result) {
        std::cout << "Error from divide: " << result.error() << std::endl;
    }

    std::cout << std::endl;
}

void demonstrate_conversions()
{
    std::cout << "=== Conversions ===" << std::endl;

    fxt::Failure failure("Convertible error");

    // Convert to string
    std::string str = failure;
    std::cout << "As string: " << str << std::endl;

    // Convert to string_view (zero-copy)
    std::string_view view = failure.message_view();
    std::cout << "As string_view: " << view << std::endl;

    // Boolean conversion
    fxt::Failure empty_failure("");
    std::cout << "empty_failure is error: " << (static_cast<bool>(empty_failure) ? "yes" : "no") << std::endl;
    std::cout << "failure is error: " << (static_cast<bool>(failure) ? "yes" : "no") << std::endl;

    std::cout << std::endl;
}

void demonstrate_exception_rethrowing()
{
    std::cout << "=== Exception Rethrowing ===" << std::endl;

    try {
        throw std::runtime_error("Original exception");
    }
    catch (...) {
        fxt::Failure failure(std::current_exception());
        std::cout << "Captured: " << failure.message() << std::endl;

        // Rethrow the stored exception
        try {
            std::rethrow_exception(failure.exception());
        }
        catch (const std::runtime_error& e) {
            std::cout << "Rethrown and caught: " << e.what() << std::endl;
        }
    }

    std::cout << std::endl;
}

void demonstrate_copy_and_move()
{
    std::cout << "=== Copy and Move Operations ===" << std::endl;

    fxt::Failure f1("Original");

    // Copy
    fxt::Failure f2 = f1;
    std::cout << "After copy - f1: " << f1.message() << ", f2: " << f2.message() << std::endl;

    // Move
    fxt::Failure f3 = std::move(f2);
    std::cout << "After move - f3: " << f3.message() << std::endl;

    // Assignment
    fxt::Failure f4("Different");
    f4 = f1;
    std::cout << "After assignment - f4: " << f4.message() << std::endl;

    // Assign string
    f4 = std::string("New message");
    std::cout << "After string assignment - f4: " << f4.message() << std::endl;

    std::cout << std::endl;
}

int main()
{
    std::cout << "FXT Failure Demo" << std::endl;
    std::cout << "================" << std::endl << std::endl;

    demonstrate_basic_construction();
    demonstrate_factory_methods();
    demonstrate_with_expected();
    demonstrate_error_propagation();
    demonstrate_comparison();
    demonstrate_hash_support();
    demonstrate_stream_output();
    demonstrate_conversions();
    demonstrate_exception_rethrowing();
    demonstrate_copy_and_move();

    std::cout << "Demo completed successfully!" << std::endl;

    return 0;
}

