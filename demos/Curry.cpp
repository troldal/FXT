//
// Created by kenne on 05/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== fxt::curry Demo ===\n\n";

    // Example 1: Basic currying with two-argument function
    std::cout << "1. Basic currying with two arguments:\n";
    auto add = [](int a, int b) { return a + b; };
    auto curried_add = fxt::curry(add);

    // Call with all arguments at once
    std::cout << "   add(5, 10) = " << curried_add(5, 10) << "\n";

    // Call with one argument at a time
    auto add5 = curried_add(5);
    std::cout << "   add5(10) = " << add5(10) << "\n";
    std::cout << "   add5(20) = " << add5(20) << "\n\n";

    // Example 2: Currying with three arguments
    std::cout << "2. Currying with three arguments:\n";
    auto add_three = [](int a, int b, int c) { return a + b + c; };
    auto curried_add_three = fxt::curry(add_three);

    std::cout << "   All at once: " << curried_add_three(1, 2, 3) << "\n";
    std::cout << "   One at a time: " << curried_add_three(1)(2)(3) << "\n";
    std::cout << "   Two then one: " << curried_add_three(1, 2)(3) << "\n\n";

    // Example 3: Creating reusable partial applications
    std::cout << "3. Creating reusable partial applications:\n";
    auto multiply = [](int a, int b, int c) { return a * b * c; };
    auto curried_multiply = fxt::curry(multiply);

    auto double_then_multiply = curried_multiply(2);
    std::cout << "   2 * 3 * 4 = " << double_then_multiply(3, 4) << "\n";
    std::cout << "   2 * 5 * 6 = " << double_then_multiply(5, 6) << "\n\n";

    // Example 4: String concatenation
    std::cout << "4. String concatenation with currying:\n";
    auto concat = [](const std::string& a, const std::string& b, const std::string& c) {
        return a + b + c;
    };
    auto curried_concat = fxt::curry(concat);

    auto hello = curried_concat("Hello");
    auto hello_space = hello(" ");
    std::cout << "   " << hello_space("World") << "\n";
    std::cout << "   " << hello_space("C++") << "\n\n";

    // Example 5: Mixed types
    std::cout << "5. Currying with mixed types:\n";
    auto format = [](const std::string& prefix, int value, double multiplier) {
        return prefix + std::to_string(static_cast<int>(value * multiplier));
    };
    auto curried_format = fxt::curry(format);

    auto format_result = curried_format("Result: ");
    auto format_result_10 = format_result(10);
    std::cout << "   " << format_result_10(2.5) << "\n";
    std::cout << "   " << format_result_10(3.0) << "\n\n";

    // Example 6: Generic lambda currying
    std::cout << "6. Generic lambda currying:\n";
    auto generic_add = [](auto a, auto b) { return a + b; };
    auto curried_generic = fxt::curry(generic_add);

    std::cout << "   Integers: " << curried_generic(5)(10) << "\n";
    std::cout << "   Doubles: " << curried_generic(3.5)(2.5) << "\n";
    std::cout << "   Strings: " << curried_generic(std::string("Hello "))(std::string("World")) << "\n\n";

    // Example 7: Function composition with curried functions
    std::cout << "7. Function composition:\n";
    auto add_curried = fxt::curry([](int x, int y) { return x + y; });
    auto multiply_curried = fxt::curry([](int x, int y) { return x * y; });

    auto add10 = add_curried(10);
    auto times3 = multiply_curried(3);

    int value = 5;
    int result = times3(add10(value));  // (5 + 10) * 3 = 45
    std::cout << "   (5 + 10) * 3 = " << result << "\n\n";

    // Example 8: Building custom operations
    std::cout << "8. Building custom operations:\n";
    auto calculate = [](const std::string& op, int a, int b) -> int {
        if (op == "add") return a + b;
        if (op == "sub") return a - b;
        if (op == "mul") return a * b;
        if (op == "div") return a / b;
        return 0;
    };
    auto curried_calc = fxt::curry(calculate);

    auto add_op = curried_calc("add");
    auto mul_op = curried_calc("mul");

    std::cout << "   add(15, 5) = " << add_op(15, 5) << "\n";
    std::cout << "   mul(15, 5) = " << mul_op(15, 5) << "\n\n";

    // Example 9: Four-argument currying with various patterns
    std::cout << "9. Four-argument currying patterns:\n";
    auto sum_four = [](int a, int b, int c, int d) { return a + b + c + d; };
    auto curried_sum = fxt::curry(sum_four);

    std::cout << "   All at once: " << curried_sum(1, 2, 3, 4) << "\n";
    std::cout << "   One by one: " << curried_sum(1)(2)(3)(4) << "\n";
    std::cout << "   (2,2): " << curried_sum(1, 2)(3, 4) << "\n";
    std::cout << "   (3,1): " << curried_sum(1, 2, 3)(4) << "\n";
    std::cout << "   (1,3): " << curried_sum(1)(2, 3, 4) << "\n\n";

    // Example 10: Practical use case - configuration builder
    std::cout << "10. Practical use case - configuration builder:\n";
    auto create_config = [](const std::string& name, int version, bool debug, double scale) {
        return "Config{name=" + name +
               ", version=" + std::to_string(version) +
               ", debug=" + (debug ? "true" : "false") +
               ", scale=" + std::to_string(scale) + "}";
    };
    auto config_builder = fxt::curry(create_config);

    // Build a partial configuration template
    auto app_config = config_builder("MyApp")(1);

    // Create different configurations from the template
    std::cout << "   Debug build: " << app_config(true, 1.0) << "\n";
    std::cout << "   Release build: " << app_config(false, 1.5) << "\n";

    return 0;
}

