/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23
         Demo: Using fxt::attempt for exception-safe operations
*/

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <stdexcept>
#include <cmath>

// A function that may throw an exception
int divide(int numerator, int denominator) {
    if (denominator == 0) {
        throw std::runtime_error("Division by zero");
    }
    return numerator / denominator;
}

// A function that performs complex parsing and may throw
double parseAndCompute(const std::string& input) {
    if (input.empty()) {
        throw std::invalid_argument("Input cannot be empty");
    }
    if (input.find_first_not_of("0123456789.-") != std::string::npos) {
        throw std::invalid_argument("Invalid numeric format");
    }
    double value = std::stod(input);
    if (value < 0) {
        throw std::domain_error("Value must be non-negative for square root");
    }
    return std::sqrt(value);
}

// A void function that may throw
void validateAndPrint(const std::string& message) {
    if (message.empty()) {
        throw std::invalid_argument("Message cannot be empty");
    }
    if (message.length() > 100) {
        throw std::length_error("Message too long (max 100 characters)");
    }
    std::cout << "    Valid message: " << message << std::endl;
}

int main() {
    std::cout << "=== fxt::attempt Demo ===" << std::endl;
    std::cout << std::endl;

    // ===== Basic Usage =====
    std::cout << "1. Basic exception handling:" << std::endl;

    // Success case
    auto result1 = fxt::attempt(divide, 10, 2);
    if (result1) {
        std::cout << "   10 / 2 = " << *result1 << std::endl;
    }

    // Failure case
    auto result2 = fxt::attempt(divide, 10, 0);
    if (!result2) {
        std::cout << "   10 / 0 failed: " << result2.error().message() << std::endl;
    }
    std::cout << std::endl;

    // ===== Handling void functions =====
    std::cout << "2. Handling void functions:" << std::endl;

    auto void_result1 = fxt::attempt(validateAndPrint, "Hello, World!");
    if (void_result1) {
        std::cout << "    Validation succeeded" << std::endl;
    }

    auto void_result2 = fxt::attempt(validateAndPrint, "");
    if (!void_result2) {
        std::cout << "    Validation failed: " << void_result2.error().message() << std::endl;
    }
    std::cout << std::endl;

    // ===== Chaining with and_then =====
    std::cout << "3. Chaining operations with and_then:" << std::endl;

    auto chain_result = fxt::attempt(divide, 100, 5)
        .and_then([](int x) -> fxt::expected<int, fxt::failure> {
            std::cout << "    First division: " << x << std::endl;
            return fxt::attempt(divide, x, 2);
        })
        .and_then([](int x) -> fxt::expected<int, fxt::failure> {
            std::cout << "    Second division: " << x << std::endl;
            return fxt::attempt(divide, x, 5);
        });

    if (chain_result) {
        std::cout << "    Final result: " << *chain_result << std::endl;
    } else {
        std::cout << "    Chain failed: " << chain_result.error().message() << std::endl;
    }
    std::cout << std::endl;

    // ===== Using transform =====
    std::cout << "4. Transforming successful results:" << std::endl;

    auto transform_result = fxt::attempt(divide, 20, 4)
        .transform([](int x) { return x * x; })
        .transform([](int x) { return std::to_string(x) + " squared"; });

    if (transform_result) {
        std::cout << "    Result: " << *transform_result << std::endl;
    }
    std::cout << std::endl;

    // ===== Using or_else for error recovery =====
    std::cout << "5. Error recovery with or_else:" << std::endl;

    auto recovery_result = fxt::attempt(divide, 10, 0)
        .or_else([](const fxt::failure& err) -> fxt::expected<int, fxt::failure> {
            std::cout << "    Caught error: " << err.message() << std::endl;
            std::cout << "    Returning default value" << std::endl;
            return 0; // Return a default value
        });

    std::cout << "    Final result: " << *recovery_result << std::endl;
    std::cout << std::endl;

    // ===== Complex parsing example =====
    std::cout << "6. Complex parsing with multiple failure modes:" << std::endl;

    std::vector<std::string> test_inputs = {"16", "", "abc", "-9", "25"};

    for (const auto& input : test_inputs) {
        auto parse_result = fxt::attempt(parseAndCompute, input);

        if (parse_result) {
            std::cout << "   sqrt(\"" << input << "\") = " << *parse_result << std::endl;
        } else {
            std::cout << "   Parse failed for \"" << input << "\": "
                      << parse_result.error().message() << std::endl;
        }
    }
    std::cout << std::endl;

    // ===== Combining with lambda expressions =====
    std::cout << "7. Using lambdas directly:" << std::endl;

    auto lambda_result = fxt::attempt([](int x) {
        if (x < 0) throw std::domain_error("Negative factorial not defined");
        if (x > 12) throw std::overflow_error("Factorial too large");

        int result = 1;
        for (int i = 2; i <= x; ++i) result *= i;
        return result;
    }, 5);

    if (lambda_result) {
        std::cout << "   5! = " << *lambda_result << std::endl;
    }

    auto lambda_error = fxt::attempt([](int x) {
        if (x < 0) throw std::domain_error("Negative factorial not defined");
        return x;
    }, -1);

    if (!lambda_error) {
        std::cout << "   Factorial error: " << lambda_error.error().message() << std::endl;
    }
    std::cout << std::endl;

    // ===== Value extraction with value_or =====
    std::cout << "8. Using value_or for default values:" << std::endl;

    int safe_result = fxt::attempt(divide, 10, 0).value_or(999);
    std::cout << "   Result with fallback: " << safe_result << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Pipe-adaptor overload: attempt(fn)
    //
    // When fn is not callable with zero arguments, attempt(fn) returns a pipe
    // closure instead of invoking fn immediately.  The closure propagates
    // existing errors unchanged and wraps any exception thrown by fn in
    // fxt::failure — exactly as the free-function form does.
    // =========================================================================

    // ===== Motivation: verbose vs clean =====
    std::cout << "9. Adaptor form replaces verbose and_then wrappers:" << std::endl;

    // Verbose form (before the adaptor overload existed)
    auto verbose = fxt::attempt(divide, 100, 4)
        .and_then([](int x) { return fxt::attempt(divide, x, 5); });

    // Clean form using the pipe-adaptor overload
    auto clean = fxt::attempt(divide, 100, 4)
               | fxt::attempt([](int x) { return divide(x, 5); });

    std::cout << "   Verbose: " << (verbose ? std::to_string(*verbose) : verbose.error().message()) << std::endl;
    std::cout << "   Clean:   " << (clean   ? std::to_string(*clean)   : clean.error().message())   << std::endl;
    std::cout << std::endl;

    // ===== Chaining multiple adaptor steps =====
    std::cout << "10. Chaining several adaptor steps:" << std::endl;

    auto chain = fxt::attempt(divide, 120, 3)     // 40
               | fxt::attempt([](int x) { return divide(x, 2); })     // 20
               | fxt::attempt([](int x) { return divide(x, 4); });    // 5

    if (chain)
        std::cout << "   120 / 3 / 2 / 4 = " << *chain << std::endl;
    std::cout << std::endl;

    // ===== Error propagation: step fails, subsequent steps are skipped =====
    std::cout << "11. Error propagation through adaptor chain:" << std::endl;

    bool step3_called = false;
    auto failed_chain = fxt::attempt(divide, 10, 2)                          // 5
                      | fxt::attempt([](int x) { return divide(x, 0); })     // throws!
                      | fxt::attempt([&step3_called](int x) {
                            step3_called = true;
                            return x;
                        });

    std::cout << "   Result: error — " << failed_chain.error().message() << std::endl;
    std::cout << "   Step 3 called: " << (step3_called ? "yes" : "no") << std::endl;
    std::cout << std::endl;

    // ===== Combined with transform and or_else =====
    std::cout << "12. Adaptor interoperates with transform and or_else:" << std::endl;

    auto mixed = fxt::attempt(divide, 20, 4)
               | fxt::attempt([](int x) { return divide(x, 0); })    // throws
               | fxt::or_else([](const fxt::failure& err) {
                     std::cout << "   Caught: " << err.message() << " — recovering\n";
                     return fxt::expected<int, fxt::failure>{ 99 };
                 })
               | fxt::transform([](int x) { return "recovered value: " + std::to_string(x); });

    if (mixed)
        std::cout << "   " << *mixed << std::endl;
    std::cout << std::endl;

    // ===== Stored adaptor reused across multiple inputs =====
    std::cout << "13. Reusable stored adaptor:" << std::endl;

    auto safe_sqrt = fxt::attempt([](double x) -> double {
        if (x < 0) throw std::domain_error("negative input");
        return std::sqrt(x);
    });

    for (double v : {25.0, -4.0, 9.0}) {
        auto r = fxt::attempt([v]{ return v; }) | safe_sqrt;
        if (r)
            std::cout << "   sqrt(" << v << ") = " << *r << std::endl;
        else
            std::cout << "   sqrt(" << v << ") failed: " << r.error().message() << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== Demo Complete ===" << std::endl;
    return 0;
}

