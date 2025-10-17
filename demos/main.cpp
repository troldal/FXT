#include <fxt.hpp>
#include <iostream>
#include <stdexcept>

// Function that succeeds
int divide(int a, int b) {
    if (b == 0) {
        throw std::runtime_error("Division by zero");
    }
    return a / b;
}

// Void function that may throw
void printMessage(const std::string& msg) {
    if (msg.empty()) {
        throw std::invalid_argument("Message cannot be empty");
    }
    std::cout << "Message: " << msg << std::endl;
}

int main() {
    // Success case with non-void return
    auto result1 = fxt::attempt(divide, 10, 2);
    if (result1) {
        std::cout << "Success: 10 / 2 = " << *result1 << std::endl;
    }

    // Failure case with non-void return
    auto result2 = fxt::attempt(divide, 10, 0);
    if (!result2) {
        std::cout << "Error: " << result2.error().message() << std::endl;
    }

    // Success case with void return
    auto result3 = fxt::attempt(printMessage, "Hello, World!");
    if (result3) {
        std::cout << "Void function succeeded" << std::endl;
    }

    // Failure case with void return
    auto result4 = fxt::attempt(printMessage, "");
    if (!result4) {
        std::cout << "Error: " << result4.error().message() << std::endl;
    }

    // Lambda example
    auto result5 = fxt::attempt([]() {
        throw std::logic_error("Something went wrong");
        return 42;
    });
    if (!result5) {
        std::cout << "Lambda error: " << result5.error().message() << std::endl;
    }


    return 0;
}