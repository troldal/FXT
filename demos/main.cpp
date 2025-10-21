#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    std::cout << "=== FXT transform_when<T> Combinator Demo ===\n\n";

    // ===== Part 1: transform_when<T> - Direct Usage =====
    std::cout << "1. transform_when<T>() - Direct calls:\n";

    fxt::variant<int, double, std::string> v1 { 42 };
    std::cout << "   Original v1 (int): " << std::get<int>(v1) << "\n";

    fxt::transform_when<int>(v1, [](int x) { return x * 2; });
    std::cout << "   After transform_when<int>: " << std::get<int>(v1) << "\n";

    fxt::transform_when<double>(v1, [](double d) { return d * 3.0; });
    std::cout << "   After transform_when<double> (skipped): " << std::get<int>(v1) << "\n\n";

    // ===== Part 2: transform_when<T> - Pipeline Usage =====
    std::cout << "2. transform_when<T>() - Pipeline usage:\n";

    fxt::variant<int, double, std::string> v2 { 100 };
    std::cout << "   Original v2 (int): " << std::get<int>(v2) << "\n";

    v2  | fxt::transform_when<int>([](int x) { return x + 50; })
        | fxt::transform_when<double>([](double d) { return d * 2.0; })
        | fxt::transform_when<int>([](int x) { return x * 2; });

    std::cout << "   After pipeline: " << std::get<int>(v2) << "\n";
    std::cout << "   (100 + 50 = 150, then 150 * 2 = 300)\n\n";

    // ===== Part 3: transform_when<T> with different types =====
    std::cout << "3. transform_when<T>() - Multiple types:\n";

    fxt::variant<int, double, std::string> v3 { 3.14 };
    std::cout << "   Original v3 (double): " << std::get<double>(v3) << "\n";

    v3  | fxt::transform_when<int>([](int x) { return x * 10; })
        | fxt::transform_when<double>([](double d) { return d * 2.0; })
        | fxt::transform_when<std::string>([](std::string s) { return s + "!"; });

    std::cout << "   After pipeline: " << std::get<double>(v3) << "\n";
    std::cout << "   (only double transformation applied)\n\n";

    fxt::variant<int, double, std::string> v4 { std::string { "hello" } };
    std::cout << "   Original v4 (string): " << std::get<std::string>(v4) << "\n";

    v4  | fxt::transform_when<int>([](int x) { return x * 10; })
        | fxt::transform_when<double>([](double d) { return d * 2.0; })
        | fxt::transform_when<std::string>([](std::string s) { return s + " world"; });

    std::cout << "   After pipeline: " << std::get<std::string>(v4) << "\n\n";

    // ===== Part 4: mtransform_when<T> with optional - Direct Usage =====
    std::cout << "4. mtransform_when<T>() with optional - Direct calls:\n";

    using MyVariant = fxt::variant<int, double, std::string>;
    fxt::optional<MyVariant> opt1 { MyVariant { 42 } };

    std::cout << "   opt1 has value: " << opt1.has_value() << ", holds int: " << std::get<int>(*opt1) << "\n";

    fxt::mtransform_when<int>(opt1, [](int x) { return x * 3; });
    std::cout << "   After mtransform_when<int>: " << std::get<int>(*opt1) << "\n";

    fxt::optional<MyVariant> opt_empty {};
    fxt::mtransform_when<int>(opt_empty, [](int x) { return x * 3; });
    std::cout << "   Empty optional unchanged: has_value = " << opt_empty.has_value() << "\n\n";

    // ===== Part 5: mtransform_when<T> with optional - Pipeline Usage =====
    std::cout << "5. mtransform_when<T>() with optional - Pipeline usage:\n";

    fxt::optional<MyVariant> opt2 { MyVariant { 100 } };
    std::cout << "   Original opt2 (int): " << std::get<int>(*opt2) << "\n";

    opt2 | fxt::mtransform_when<int>([](int x) { return x + 25; })
         | fxt::mtransform_when<double>([](double d) { return d * 2.0; })
         | fxt::mtransform_when<int>([](int x) { return x * 4; });

    std::cout << "   After pipeline: " << std::get<int>(*opt2) << "\n";
    std::cout << "   ((100 + 25) * 4 = 500)\n\n";

    // ===== Part 6: mtransform_when<T> with expected =====
    std::cout << "6. mtransform_when<T>() with expected:\n";

    fxt::expected<MyVariant, std::string> exp1 { MyVariant { 42 } };
    std::cout << "   exp1 has value: " << exp1.has_value() << ", holds int: " << std::get<int>(*exp1) << "\n";

    exp1 | fxt::mtransform_when<int>([](int x) { return x * 2; })
         | fxt::mtransform_when<double>([](double d) { return d / 2.0; });

    std::cout << "   After pipeline: " << std::get<int>(*exp1) << "\n";

    fxt::expected<MyVariant, std::string> exp_err { fxt::unexpected("error occurred") };
    std::cout << "   exp_err has value: " << exp_err.has_value() << "\n";

    exp_err | fxt::mtransform_when<int>([](int x) { return x * 2; });
    std::cout << "   After mtransform_when (skipped): has_value = " << exp_err.has_value() << "\n\n";

    // ===== Part 7: Mixing transform_when and when =====
    std::cout << "7. Combining transform_when<T>() and when<T>():\n";

    fxt::variant<int, double, std::string> v5 { 50 };
    std::cout << "   Original v5 (int): " << std::get<int>(v5) << "\n";

    v5  | fxt::transform_when<int>([](int x) { return x * 2; })
        | fxt::when<int>([](int x) { std::cout << "   [LOG] Value is now: " << x << "\n"; })
        | fxt::transform_when<int>([](int x) { return x + 100; })
        | fxt::when<int>([](int x) { std::cout << "   [LOG] Final value: " << x << "\n"; });

    std::cout << "   Result: " << std::get<int>(v5) << "\n\n";

    // ===== Part 8: Practical example - Data validation and transformation =====
    std::cout << "8. Practical example - Validation and transformation pipeline:\n";

    fxt::optional<MyVariant> user_input { MyVariant { -5 } };

    std::cout << "   Processing user input: " << std::get<int>(*user_input) << "\n";

    user_input | fxt::mwhen<int>([](int x) { if (x < 0) std::cout << "   [WARN] Negative value detected\n";})
               | fxt::mtransform_when<int>([](int x) { return std::abs(x); })
               | fxt::mwhen<int>([](int x) { std::cout << "   [INFO] Normalized to: " << x << "\n"; })
               | fxt::mtransform_when<int>([](int x) { return x * 10; })
               | fxt::mwhen<int>([](int x) { std::cout << "   [INFO] Scaled to: " << x << "\n"; });

    std::cout << "   Final result: " << std::get<int>(*user_input) << "\n\n";

    // ===== Part 9: Type conversion example =====
    std::cout << "9. Type-specific transformations:\n";

    fxt::variant<int, double, std::string> v6 { std::string { "42" } };
    std::cout << "   Original v6 (string): " << std::get<std::string>(v6) << "\n";

    v6  | fxt::transform_when<std::string>([](std::string s) { return "Number: " + s; })
        | fxt::when<std::string>([](const std::string& s) { std::cout << "   Transformed string: " << s << "\n"; });

    std::cout << "\n";

    return 0;
}