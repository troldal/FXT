#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    std::cout << "=== FXT when<T> and mwhen<T> Demo ===\n\n";

    // ===== Part 1: when<T> - Direct Usage =====
    std::cout << "1. when<T>() - Direct calls (side-effects only):\n";

    fxt::variant<int, double, std::string> v1{42};
    std::cout << "   Testing v1 (int = 42):\n";

    fxt::when<int>(v1, [](int x) { std::cout << "     Found int: " << x << "\n"; });
    fxt::when<double>(v1, [](double d) { std::cout << "     Found double: " << d << "\n"; });
    fxt::when<std::string>(v1, [](const std::string& s) { std::cout << "     Found string: " << s << "\n"; });
    std::cout << "   (only int handler executed)\n\n";

    // ===== Part 2: when<T> - Pipeline Usage =====
    std::cout << "2. when<T>() - Pipeline usage:\n";

    fxt::variant<int, double, std::string> v2{3.14};
    std::cout << "   Testing v2 (double = 3.14):\n";

    v2 | fxt::when<int>([](int x) { std::cout << "     int: " << x << "\n"; })
       | fxt::when<double>([](double d) { std::cout << "     double: " << d << "\n"; })
       | fxt::when<std::string>([](const std::string& s) { std::cout << "     string: " << s << "\n"; });
    std::cout << "   (only double handler executed)\n\n";

    // ===== Part 3: when<T> - Chaining multiple handlers =====
    std::cout << "3. when<T>() - Chaining for logging/validation:\n";

    fxt::variant<int, double, std::string> v3{std::string{"hello"}};
    std::cout << "   Testing v3 (string = \"hello\"):\n";

    v3 | fxt::when<int>([](int x) {
           std::cout << "     [VALIDATE] Integer value: " << x << "\n";
       })
       | fxt::when<double>([](double d) {
           std::cout << "     [VALIDATE] Double value: " << d << "\n";
       })
       | fxt::when<std::string>([](const std::string& s) {
           std::cout << "     [VALIDATE] String value: \"" << s << "\" (length: " << s.length() << ")\n";
       });
    std::cout << "\n";

    // ===== Part 4: when<T> - Multiple side effects on same type =====
    std::cout << "4. when<T>() - Multiple side effects on same type:\n";

    fxt::variant<int, double, std::string> v4{99};
    std::cout << "   Processing v4 (int = 99):\n";

    v4 | fxt::when<int>([](int x) { std::cout << "     [LOG] Processing value: " << x << "\n"; })
       | fxt::when<int>([](int x) {
           if (x > 100) {
               std::cout << "     [WARN] Value exceeds threshold!\n";
           } else {
               std::cout << "     [INFO] Value within acceptable range\n";
           }
       })
       | fxt::when<int>([](int x) { std::cout << "     [DEBUG] Hex representation: 0x" << std::hex << x << std::dec << "\n"; });
    std::cout << "\n";

    // ===== Part 5: mwhen<T> with optional - Direct Usage =====
    std::cout << "5. mwhen<T>() with optional - Direct calls:\n";

    using MyVariant = fxt::variant<int, double, std::string>;
    fxt::optional<MyVariant> opt1{MyVariant{42}};

    std::cout << "   opt1 has value (int = 42):\n";
    fxt::mwhen<int>(opt1, [](int x) { std::cout << "     Found int in optional: " << x << "\n"; });
    fxt::mwhen<double>(opt1, [](double d) { std::cout << "     Found double in optional: " << d << "\n"; });

    fxt::optional<MyVariant> opt_empty{};
    std::cout << "   opt_empty has no value:\n";
    fxt::mwhen<int>(opt_empty, [](int x) { std::cout << "     Found int: " << x << "\n"; });
    std::cout << "     (nothing printed - optional is empty)\n\n";

    // ===== Part 6: mwhen<T> with optional - Pipeline Usage =====
    std::cout << "6. mwhen<T>() with optional - Pipeline usage:\n";

    fxt::optional<MyVariant> opt2{MyVariant{std::string{"world"}}};
    std::cout << "   opt2 has value (string = \"world\"):\n";

    opt2 | fxt::mwhen<int>([](int x) { std::cout << "     int handler: " << x << "\n"; })
         | fxt::mwhen<double>([](double d) { std::cout << "     double handler: " << d << "\n"; })
         | fxt::mwhen<std::string>([](const std::string& s) {
             std::cout << "     string handler: \"" << s << "\"\n";
         });
    std::cout << "\n";

    // ===== Part 7: mwhen<T> with expected - Direct Usage =====
    std::cout << "7. mwhen<T>() with expected - Direct calls:\n";

    fxt::expected<MyVariant, std::string> exp1{MyVariant{3.14159}};
    std::cout << "   exp1 has value (double = 3.14159):\n";
    fxt::mwhen<double>(exp1, [](double d) {
        std::cout << "     Found double in expected: " << d << "\n";
    });

    fxt::expected<MyVariant, std::string> exp_err{fxt::unexpected("error occurred")};
    std::cout << "   exp_err has error:\n";
    fxt::mwhen<double>(exp_err, [](double d) {
        std::cout << "     Found double: " << d << "\n";
    });
    std::cout << "     (nothing printed - expected has error)\n\n";

    // ===== Part 8: mwhen<T> with expected - Pipeline Usage =====
    std::cout << "8. mwhen<T>() with expected - Pipeline usage:\n";

    fxt::expected<MyVariant, std::string> exp2{MyVariant{100}};
    std::cout << "   exp2 has value (int = 100):\n";

    exp2 | fxt::mwhen<int>([](int x) { std::cout << "     Processing int: " << x << "\n"; })
         | fxt::mwhen<double>([](double d) { std::cout << "     Processing double: " << d << "\n"; })
         | fxt::mwhen<std::string>([](const std::string& s) {
             std::cout << "     Processing string: " << s << "\n";
         });
    std::cout << "\n";

    // ===== Part 9: Practical example - Logging in a processing pipeline =====
    std::cout << "9. Practical example - Logging and validation pipeline:\n";

    fxt::variant<int, double, std::string> data{42};
    std::cout << "   Processing data (int = 42):\n";

    auto result = data
        | fxt::when<int>([](int x) { std::cout << "     [LOG] Received integer: " << x << "\n"; })
        | fxt::when<int>([](int x) {
            if (x < 0) {
                std::cout << "     [ERROR] Negative value!\n";
            } else if (x > 1000) {
                std::cout << "     [WARN] Very large value!\n";
            } else {
                std::cout << "     [INFO] Value is valid\n";
            }
        })
        | fxt::when<double>([](double d) { std::cout << "     [LOG] Received double: " << d << "\n"; })
        | fxt::when<std::string>([](const std::string& s) {
            std::cout << "     [LOG] Received string: \"" << s << "\"\n";
        })
        | fxt::visit([](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        });

    std::cout << "   Final result as string: \"" << result << "\"\n\n";

    // ===== Part 10: Complex monadic pipeline =====
    std::cout << "10. Complex monadic pipeline with mwhen<T>():\n";

    auto process_data = [](int x) -> fxt::expected<MyVariant, std::string> {
        if (x < 0) {
            return fxt::unexpected("Negative input");
        }
        if (x % 2 == 0) {
            return MyVariant{x * 2};
        } else {
            return MyVariant{std::to_string(x)};
        }
    };

    auto result1 = process_data(10);
    std::cout << "   Processing 10 (even number):\n";
    result1 | fxt::mwhen<int>([](int x) { std::cout << "     Got int result: " << x << "\n"; })
            | fxt::mwhen<std::string>([](const std::string& s) {
                std::cout << "     Got string result: \"" << s << "\"\n";
            });

    auto result2 = process_data(7);
    std::cout << "   Processing 7 (odd number):\n";
    result2 | fxt::mwhen<int>([](int x) { std::cout << "     Got int result: " << x << "\n"; })
            | fxt::mwhen<std::string>([](const std::string& s) {
                std::cout << "     Got string result: \"" << s << "\"\n";
            });

    auto result3 = process_data(-5);
    std::cout << "   Processing -5 (negative):\n";
    result3 | fxt::mwhen<int>([](int x) { std::cout << "     Got int result: " << x << "\n"; })
            | fxt::mwhen<std::string>([](const std::string& s) {
                std::cout << "     Got string result: \"" << s << "\"\n";
            });
    std::cout << "     (nothing printed - expected has error: " << result3.error() << ")\n\n";

    return 0;
}

