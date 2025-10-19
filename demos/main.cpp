#include "fxt.hpp"
#include <iostream>
#include <expected>
#include <optional>
#include <string>

int main() {
    using namespace fxt;

    std::cout << "=== fxt::mapply Demo ===\n\n";

    // 1. Basic usage with fxt::optional and regular return type
    std::cout << "1. Optional with regular function:\n";
    auto opt_tuple = fxt::optional{fxt::tuple{3, 4}};
    auto result1 = opt_tuple | mapply([](int a, int b) {
        return a + b;
    });
    std::cout << "   Result: " << (result1 ? std::to_string(*result1) : "nullopt") << "\n\n";

    // 2. With empty optional
    std::cout << "2. Empty optional:\n";
    fxt::optional<fxt::tuple<int, int>> empty_opt;
    auto result2 = empty_opt | mapply([](int a, int b) { return a * b; });
    std::cout << "   Result: " << (result2 ? std::to_string(*result2) : "nullopt") << "\n\n";

    // 3. With fxt::expected and regular return
    std::cout << "3. Expected with regular function:\n";
    auto exp_tuple = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 5}};
    auto result3 = exp_tuple | mapply([](int a, int b) {
        return a / b;
    });
    std::cout << "   Result: " << (result3 ? std::to_string(*result3) : result3.error()) << "\n\n";

    // 4. Function returning monadic type (optional)
    std::cout << "4. Function returning optional:\n";
    auto opt_div = fxt::optional{fxt::tuple{10, 2}}
                 | mapply([](int a, int b) -> fxt::optional<double> {
                       if (b == 0) return std::nullopt;
                       return static_cast<double>(a) / b;
                   });
    std::cout << "   Result: " << (opt_div ? std::to_string(*opt_div) : "nullopt") << "\n\n";

    // 5. Function returning expected (division by zero)
    std::cout << "5. Function returning expected (error case):\n";
    auto exp_div = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 0}}
                 | mapply([](int a, int b) -> fxt::expected<double, std::string> {
                       if (b == 0) return std::unexpected{"Division by zero"};
                       return static_cast<double>(a) / b;
                   });
    std::cout << "   Result: " << (exp_div ? std::to_string(*exp_div) : exp_div.error()) << "\n\n";

    // 6. Void function with optional
    std::cout << "6. Void function with optional:\n";
    auto opt_print = fxt::optional{fxt::tuple{42, "Hello"}}
                   | mapply([](int num, const char* str) {
                         std::cout << "   Inside lambda: " << str << " " << num << "\n";
                     });
    std::cout << "   Result type is optional<monostate>: "
              << (opt_print.has_value() ? "has value" : "nullopt") << "\n\n";

    // 7. Using fxt::flat_tuple
    std::cout << "7. With flat_tuple:\n";
    auto flat_opt = fxt::optional{make_flat_tuple(5.0, 3.0)};
    auto result7 = flat_opt | mapply([](double a, double b) {
        return a * b;
    });
    std::cout << "   Result: " << (result7 ? std::to_string(*result7) : "nullopt") << "\n\n";

    // 8. Chain multiple operations
    std::cout << "8. Chained operations:\n";
    auto chained = fxt::optional{fxt::tuple{2, 3}}
                 | mapply([](int a, int b) { return a + b; })
                 | std::views::transform([](int x) { return x * 2; });
    std::cout << "   Result: " << (chained ? std::to_string(*chained) : "nullopt") << "\n";

    return 0;
}