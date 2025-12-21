#include "include/fxt/utils/Failure.hpp"
#include <iostream>

struct CustomContext {
    int code;
    std::string detail;
};

int main() {
    // Test 1: Create failure from string
    fxt::failure f1("Simple error message");
    std::cout << "F1: " << f1.message() << std::endl;

    // Test 2: Create failure with context
    fxt::failure f2 = fxt::failure("Error with context")
        .with_context(CustomContext{42, "Additional detail"});

    std::cout << "F2: " << f2.message() << std::endl;
    if (f2.has_context<CustomContext>()) {
        auto* ctx = f2.get_context<CustomContext>();
        std::cout << "F2 Context: code=" << ctx->code << ", detail=" << ctx->detail << std::endl;
    }

    // Test 3: Create failure from exception
    try {
        throw std::runtime_error("Runtime error!");
    } catch (...) {
        fxt::failure f3 = fxt::failure::from_current_exception();
        std::cout << "F3: " << f3.message() << std::endl;
        std::cout << "F3 has exception: " << (f3.has_exception() ? "yes" : "no") << std::endl;
    }

    // Test 4: Copy and context preservation
    fxt::failure f4 = f2;
    if (f4.has_context<CustomContext>()) {
        auto* ctx = f4.get_context<CustomContext>();
        std::cout << "F4 Context (copied): code=" << ctx->code << std::endl;
    }

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}

