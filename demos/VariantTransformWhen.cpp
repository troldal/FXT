#include <fxt.hpp>
#include <iostream>
#include <string>
#include <cmath>

int main()
{
    std::cout << "=== FXT transform_when<T> and mtransform_when<T> Demo ===\n\n";

    // ===== Part 1: transform_when<T> - Direct Usage =====
    std::cout << "1. transform_when<T>() - Direct calls:\n";

    fxt::variant<int, double, std::string> v1{42};
    std::cout << "   Original v1 (int): " << std::get<int>(v1) << "\n";

    fxt::transform_when<int>(v1, [](int x) { return x * 2; });
    std::cout << "   After transform_when<int>: " << std::get<int>(v1) << "\n";

    fxt::transform_when<double>(v1, [](double d) { return d * 3.0; });
    std::cout << "   After transform_when<double> (skipped): " << std::get<int>(v1) << "\n\n";

    // ===== Part 2: transform_when<T> - Pipeline Usage =====
    std::cout << "2. transform_when<T>() - Pipeline usage:\n";

    fxt::variant<int, double, std::string> v2{100};
    std::cout << "   Original v2 (int): " << std::get<int>(v2) << "\n";

    v2 | fxt::transform_when<int>([](int x) { return x + 50; })
       | fxt::transform_when<double>([](double d) { return d * 2.0; })
       | fxt::transform_when<int>([](int x) { return x * 2; });

    std::cout << "   After pipeline: " << std::get<int>(v2) << "\n";
    std::cout << "   (100 + 50 = 150, then 150 * 2 = 300)\n\n";

    // ===== Part 3: transform_when<T> with different types =====
    std::cout << "3. transform_when<T>() - Multiple types:\n";

    fxt::variant<int, double, std::string> v3{3.14};
    std::cout << "   Original v3 (double): " << std::get<double>(v3) << "\n";

    v3 | fxt::transform_when<int>([](int x) { return x * 10; })
       | fxt::transform_when<double>([](double d) { return d * 2.0; })
       | fxt::transform_when<std::string>([](std::string s) { return s + "!"; });

    std::cout << "   After pipeline: " << std::get<double>(v3) << "\n";
    std::cout << "   (only double transformation applied)\n\n";

    fxt::variant<int, double, std::string> v4{std::string{"hello"}};
    std::cout << "   Original v4 (string): " << std::get<std::string>(v4) << "\n";

    v4 | fxt::transform_when<int>([](int x) { return x * 10; })
       | fxt::transform_when<double>([](double d) { return d * 2.0; })
       | fxt::transform_when<std::string>([](std::string s) { return s + " world"; });

    std::cout << "   After pipeline: " << std::get<std::string>(v4) << "\n\n";

    // ===== Part 4: transform_when<T> - Multiple transformations on same type =====
    std::cout << "4. transform_when<T>() - Chaining same-type transformations:\n";

    fxt::variant<int, double, std::string> v5{10};
    std::cout << "   Original v5 (int): " << std::get<int>(v5) << "\n";

    v5 | fxt::transform_when<int>([](int x) { return x * 2; })
       | fxt::transform_when<int>([](int x) { return x + 5; })
       | fxt::transform_when<int>([](int x) { return x * 3; });

    std::cout << "   After pipeline: " << std::get<int>(v5) << "\n";
    std::cout << "   ((10 * 2) + 5) * 3 = 75\n\n";

    // ===== Part 5: mtransform_when<T> with optional - Direct Usage =====
    std::cout << "5. mtransform_when<T>() with optional - Direct calls:\n";

    using MyVariant = fxt::variant<int, double, std::string>;
    fxt::optional<MyVariant> opt1{MyVariant{42}};

    std::cout << "   opt1 has value: " << opt1.has_value() << ", holds int: " << std::get<int>(*opt1) << "\n";

    fxt::mtransform_when<int>(opt1, [](int x) { return x * 3; });
    std::cout << "   After mtransform_when<int>: " << std::get<int>(*opt1) << "\n";

    fxt::optional<MyVariant> opt_empty{};
    std::cout << "   opt_empty before transform: has_value = " << opt_empty.has_value() << "\n";
    fxt::mtransform_when<int>(opt_empty, [](int x) { return x * 3; });
    std::cout << "   opt_empty after transform: has_value = " << opt_empty.has_value() << " (unchanged)\n\n";

    // ===== Part 6: mtransform_when<T> with optional - Pipeline Usage =====
    std::cout << "6. mtransform_when<T>() with optional - Pipeline usage:\n";

    fxt::optional<MyVariant> opt2{MyVariant{100}};
    std::cout << "   Original opt2 (int): " << std::get<int>(*opt2) << "\n";

    opt2 | fxt::mtransform_when<int>([](int x) { return x + 25; })
         | fxt::mtransform_when<double>([](double d) { return d * 2.0; })
         | fxt::mtransform_when<int>([](int x) { return x * 4; });

    std::cout << "   After pipeline: " << std::get<int>(*opt2) << "\n";
    std::cout << "   ((100 + 25) * 4 = 500)\n\n";

    // ===== Part 7: mtransform_when<T> with expected - Direct Usage =====
    std::cout << "7. mtransform_when<T>() with expected - Direct calls:\n";

    fxt::expected<MyVariant, std::string> exp1{MyVariant{42}};
    std::cout << "   exp1 has value: " << exp1.has_value() << ", holds int: " << std::get<int>(*exp1) << "\n";

    fxt::mtransform_when<int>(exp1, [](int x) { return x * 2; });
    std::cout << "   After mtransform_when<int>: " << std::get<int>(*exp1) << "\n";

    fxt::expected<MyVariant, std::string> exp_err{fxt::unexpected("error occurred")};
    std::cout << "   exp_err has error, attempting transform...\n";
    fxt::mtransform_when<int>(exp_err, [](int x) { return x * 2; });
    std::cout << "   exp_err still has error: " << exp_err.error() << " (unchanged)\n\n";

    // ===== Part 8: mtransform_when<T> with expected - Pipeline Usage =====
    std::cout << "8. mtransform_when<T>() with expected - Pipeline usage:\n";

    fxt::expected<MyVariant, std::string> exp2{MyVariant{50}};
    std::cout << "   Original exp2 (int): " << std::get<int>(*exp2) << "\n";

    exp2 | fxt::mtransform_when<int>([](int x) { return x * 2; })
         | fxt::mtransform_when<double>([](double d) { return d / 2.0; })
         | fxt::mtransform_when<int>([](int x) { return x + 10; });

    std::cout << "   After pipeline: " << std::get<int>(*exp2) << "\n";
    std::cout << "   ((50 * 2) + 10 = 110)\n\n";

    // ===== Part 9: Mixing transform_when and when =====
    std::cout << "9. Combining transform_when<T>() and when<T>():\n";

    fxt::variant<int, double, std::string> v6{50};
    std::cout << "   Original v6 (int): " << std::get<int>(v6) << "\n";

    v6 | fxt::transform_when<int>([](int x) { return x * 2; })
       | fxt::when<int>([](int x) { std::cout << "   [LOG] After first transform: " << x << "\n"; })
       | fxt::transform_when<int>([](int x) { return x + 100; })
       | fxt::when<int>([](int x) { std::cout << "   [LOG] After second transform: " << x << "\n"; });

    std::cout << "   Final result: " << std::get<int>(v6) << "\n\n";

    // ===== Part 10: Practical example - Data normalization pipeline =====
    std::cout << "10. Practical example - Data normalization pipeline:\n";

    fxt::optional<MyVariant> user_input{MyVariant{-5}};
    std::cout << "   Processing user input: " << std::get<int>(*user_input) << "\n";

    user_input
        | fxt::mwhen<int>([](int x) {
            if (x < 0) std::cout << "   [WARN] Negative value detected\n";
        })
        | fxt::mtransform_when<int>([](int x) { return std::abs(x); })
        | fxt::mwhen<int>([](int x) {
            std::cout << "   [INFO] Normalized to: " << x << "\n";
        })
        | fxt::mtransform_when<int>([](int x) { return x * 10; })
        | fxt::mwhen<int>([](int x) {
            std::cout << "   [INFO] Scaled to: " << x << "\n";
        });

    std::cout << "   Final result: " << std::get<int>(*user_input) << "\n\n";

    // ===== Part 11: String transformations =====
    std::cout << "11. String-specific transformations:\n";

    fxt::variant<int, double, std::string> v7{std::string{"hello"}};
    std::cout << "   Original v7 (string): \"" << std::get<std::string>(v7) << "\"\n";

    v7 | fxt::transform_when<std::string>([](std::string s) {
            // Convert to uppercase
            for (auto& c : s) c = std::toupper(c);
            return s;
        })
       | fxt::when<std::string>([](const std::string& s) {
            std::cout << "   After uppercase: \"" << s << "\"\n";
        })
       | fxt::transform_when<std::string>([](std::string s) {
            return s + " WORLD!";
        });

    std::cout << "   Final string: \"" << std::get<std::string>(v7) << "\"\n\n";

    // ===== Part 12: Mathematical transformations on doubles =====
    std::cout << "12. Mathematical transformations on doubles:\n";

    fxt::variant<int, double, std::string> v8{2.0};
    std::cout << "   Original v8 (double): " << std::get<double>(v8) << "\n";

    v8 | fxt::transform_when<double>([](double d) { return d * d; })
       | fxt::when<double>([](double d) { std::cout << "   After square: " << d << "\n"; })
       | fxt::transform_when<double>([](double d) { return std::sqrt(d); })
       | fxt::when<double>([](double d) { std::cout << "   After sqrt: " << d << "\n"; })
       | fxt::transform_when<double>([](double d) { return d * 10.0; });

    std::cout << "   Final value: " << std::get<double>(v8) << "\n\n";

    // ===== Part 13: Complex monadic transformation pipeline =====
    std::cout << "13. Complex monadic pipeline with both mwhen and mtransform_when:\n";

    auto process_number = [](int x) -> fxt::expected<MyVariant, std::string> {
        if (x < 0) {
            return fxt::unexpected("Negative input not allowed");
        }
        if (x > 1000) {
            return fxt::unexpected("Value too large");
        }
        return MyVariant{x};
    };

    auto result1 = process_number(25);
    std::cout << "   Processing 25:\n";

    result1
        | fxt::mwhen<int>([](int x) { std::cout << "     [START] Input value: " << x << "\n"; })
        | fxt::mtransform_when<int>([](int x) { return x * 2; })
        | fxt::mwhen<int>([](int x) { std::cout << "     [STEP1] After doubling: " << x << "\n"; })
        | fxt::mtransform_when<int>([](int x) { return x + 100; })
        | fxt::mwhen<int>([](int x) { std::cout << "     [STEP2] After adding 100: " << x << "\n"; })
        | fxt::mtransform_when<int>([](int x) { return x / 5; })
        | fxt::mwhen<int>([](int x) { std::cout << "     [FINAL] After dividing by 5: " << x << "\n"; });

    if (result1.has_value()) {
        std::cout << "   Result: " << std::get<int>(*result1) << "\n";
    }

    auto result2 = process_number(-10);
    std::cout << "\n   Processing -10:\n";

    result2
        | fxt::mwhen<int>([](int x) { std::cout << "     Input value: " << x << "\n"; })
        | fxt::mtransform_when<int>([](int x) { return x * 2; });

    if (!result2.has_value()) {
        std::cout << "     Error: " << result2.error() << "\n";
    }

    std::cout << "\n";

    return 0;
}

