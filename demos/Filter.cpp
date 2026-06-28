//
// Created by kenne on 28/06/2026.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Helper functions for demonstration
fxt::optional<int> parse_int(const std::string& str) {
    try {
        return fxt::optional<int>{ std::stoi(str) };
    } catch (...) {
        return fxt::optional<int>{};
    }
}

fxt::expected<int, std::string> safe_divide(int a, int b) {
    if (b == 0)
        return fxt::unexpected<std::string>{"division by zero"};
    return fxt::expected<int, std::string>{ a / b };
}

int main() {
    std::cout << "=== Filter Demonstration ===\n\n";

    // -------------------------------------------------------------------------
    std::cout << "--- fxt::optional examples ---\n";

    // Example 1: Value satisfies predicate — returned unchanged
    auto opt1 = parse_int("42")
              | fxt::filter([](int x) { return x > 0; });
    std::cout << "parse_int(\"42\") | filter(> 0)  = "
              << (opt1.has_value() ? std::to_string(*opt1) : "nullopt") << "\n";

    // Example 2: Value does not satisfy predicate — returns nullopt
    auto opt2 = parse_int("-7")
              | fxt::filter([](int x) { return x > 0; });
    std::cout << "parse_int(\"-7\") | filter(> 0)  = "
              << (opt2.has_value() ? std::to_string(*opt2) : "nullopt") << "\n";

    // Example 3: Disengaged optional propagates without calling the predicate
    bool called = false;
    auto opt3 = parse_int("invalid")
              | fxt::filter([&called](int) { called = true; return true; });
    std::cout << "parse_int(\"invalid\") | filter(...) = "
              << (opt3.has_value() ? std::to_string(*opt3) : "nullopt")
              << "  (predicate called: " << (called ? "yes" : "no") << ")\n";

    // Example 4: Reusable filter adaptor stored in a variable
    auto even = fxt::filter([](int x) { return x % 2 == 0; });
    std::cout << "\nUsing a reusable 'even' filter:\n";
    for (const char* s : {"4", "7", "12", "abc"}) {
        auto r = parse_int(s) | even;
        std::cout << "  parse_int(\"" << s << "\") | even  = "
                  << (r.has_value() ? std::to_string(*r) : "nullopt") << "\n";
    }

    // Example 5: filter chained with transform and and_then
    auto opt5 = parse_int("6")
              | fxt::filter([](int x) { return x % 2 == 0; })   // keep if even → 6
              | fxt::transform([](int x) { return x * x; });    // square → 36
    std::cout << "\nparse_int(\"6\") | filter(even) | transform(square) = "
              << (opt5.has_value() ? std::to_string(*opt5) : "nullopt") << "\n";

    auto opt6 = parse_int("5")
              | fxt::filter([](int x) { return x % 2 == 0; })   // 5 is odd → nullopt
              | fxt::transform([](int x) { return x * x; });    // skipped
    std::cout << "parse_int(\"5\") | filter(even) | transform(square) = "
              << (opt6.has_value() ? std::to_string(*opt6) : "nullopt") << "\n";

    // -------------------------------------------------------------------------
    std::cout << "\n--- fxt::expected examples ---\n";

    // Example 6: Value satisfies predicate — returned unchanged
    auto exp1 = safe_divide(20, 4)
              | fxt::filter([](int x) { return x > 0; }, std::string{"result not positive"});
    std::cout << "safe_divide(20, 4) | filter(> 0)  = ";
    if (exp1.has_value()) std::cout << *exp1 << "\n";
    else                  std::cout << "error: " << exp1.error() << "\n";

    // Example 7: Value fails predicate — replaced with the supplied error
    auto exp2 = safe_divide(20, 4)
              | fxt::filter([](int x) { return x > 10; }, std::string{"result too small"});
    std::cout << "safe_divide(20, 4) | filter(> 10) = ";
    if (exp2.has_value()) std::cout << *exp2 << "\n";
    else                  std::cout << "error: " << exp2.error() << "\n";

    // Example 8: Pre-existing error propagates unchanged — predicate is not called
    called = false;
    auto exp3 = safe_divide(10, 0)
              | fxt::filter([&called](int) { called = true; return true; }, std::string{"new error"});
    std::cout << "safe_divide(10, 0) | filter(...)  = ";
    if (exp3.has_value()) std::cout << *exp3 << "\n";
    else                  std::cout << "error: " << exp3.error()
                                    << "  (predicate called: " << (called ? "yes" : "no") << ")\n";

    // Example 9: Reusable filter adaptor for expected
    auto positive = fxt::filter([](int x) { return x > 0; }, std::string{"not positive"});
    std::cout << "\nUsing a reusable 'positive' filter on expected:\n";
    for (auto [a, b] : std::initializer_list<std::pair<int,int>>{{10, 2}, {-6, 2}, {5, 0}}) {
        auto r = safe_divide(a, b) | positive;
        std::cout << "  safe_divide(" << a << ", " << b << ") | positive  = ";
        if (r.has_value()) std::cout << *r << "\n";
        else               std::cout << "error: " << r.error() << "\n";
    }

    // Example 10: Validation pipeline — successive filters narrow a value
    std::cout << "\nValidation pipeline (value must be positive, even, and <= 100):\n";
    auto validate = [](int raw) {
        return fxt::expected<int, std::string>{ raw }
             | fxt::filter([](int x) { return x > 0;      }, std::string{"must be positive"})
             | fxt::filter([](int x) { return x % 2 == 0; }, std::string{"must be even"})
             | fxt::filter([](int x) { return x <= 100;   }, std::string{"must be <= 100"});
    };

    for (int v : {42, -3, 7, 200}) {
        auto r = validate(v);
        std::cout << "  validate(" << v << ") = ";
        if (r.has_value()) std::cout << *r << "  (ok)\n";
        else               std::cout << "error: " << r.error() << "\n";
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- Mixed pipeline (optional + transform + filter) ---\n";

    // Example 11: parse, scale, then keep only if in range
    auto in_range = fxt::filter([](int x) { return x >= 10 && x <= 50; });
    for (const char* s : {"3", "15", "60", "bad"}) {
        auto r = parse_int(s)
               | fxt::transform([](int x) { return x * 3; })
               | in_range;
        std::cout << "  parse(\"" << s << "\") * 3 in [10,50]: "
                  << (r.has_value() ? std::to_string(*r) : "nullopt") << "\n";
    }

    return 0;
}
