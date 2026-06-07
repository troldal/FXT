//
// Created by kenne on 07/06/2026.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <format>

// Helper: print an expected<T, string> on one line
template<typename T>
void print_result(const fxt::expected<T, std::string>& r) {
    if (r.has_value()) {
        if constexpr (std::is_same_v<T, std::string>)
            std::cout << "Value: \"" << *r << "\"";
        else
            std::cout << "Value: " << *r;
    } else {
        std::cout << "Error: \"" << r.error() << "\"";
    }
}

int main() {
    std::cout << "=== fxt::with Applicative Pattern Demo ===\n\n";

    // The applicative pattern extends the monadic pipeline to handle
    // multi-argument functions where every argument may independently fail.
    //
    //   fxt::curry(f) | fxt::with(arg1) | fxt::with(arg2) | ...
    //
    // Short-circuit semantics: the first error encountered is propagated and
    // subsequent arguments are not evaluated.

    // ========================================================================
    // Part 1: The Fundamentals
    // ========================================================================

    std::cout << "PART 1: Fundamentals\n";
    std::cout << "====================\n\n";

    // Example 1: Single-argument function — entry point
    std::cout << "1. Single-argument function:\n";
    {
        auto square = [](double x) { return x * x; };

        auto ok  = fxt::curry(square) | fxt::with(fxt::expected<double, std::string>{5.0});
        auto err = fxt::curry(square) | fxt::with(fxt::expected<double, std::string>{
                                            fxt::unexpected<std::string>("bad input")});

        std::cout << "   curry(square) | with(5.0)        -> "; print_result(ok);  std::cout << "\n";
        std::cout << "   curry(square) | with(\"bad input\") -> "; print_result(err); std::cout << "\n\n";
    }

    // Example 2: Two-argument function, step by step
    std::cout << "2. Two-argument function, step by step:\n";
    {
        auto add = [](double a, double b) { return a + b; };

        // Step 1: curry(add) | with(10.0)  =>  expected<partially-applied-add, string>
        auto step1 = fxt::curry(add) | fxt::with(fxt::expected<double, std::string>{10.0});
        std::cout << "   After step 1 (first arg applied): step1.has_value() = "
                  << (step1.has_value() ? "true" : "false") << " (holds partial fn)\n";

        // Step 2: step1 | with(5.0)  =>  expected<double, string>
        auto result = step1 | fxt::with(fxt::expected<double, std::string>{5.0});
        std::cout << "   After step 2 (second arg applied): "; print_result(result); std::cout << "\n\n";
    }

    // Example 3: The compact pipeline form (most common usage)
    std::cout << "3. Compact two-argument pipeline:\n";
    {
        auto add = [](double a, double b) { return a + b; };

        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{5.0});

        std::cout << "   curry(add) | with(10.0) | with(5.0) -> "; print_result(result); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 2: Three-Argument Pipeline (Primary Use Case)
    // ========================================================================

    std::cout << "PART 2: Three-Argument Pipeline\n";
    std::cout << "================================\n\n";

    // Example 4: Three-argument function, all valid
    std::cout << "4. Three-argument function, all valid:\n";
    {
        auto volume = [](double width, double height, double depth) {
            return width * height * depth;
        };

        auto result = fxt::curry(volume)
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::with(fxt::expected<double, std::string>{5.0});

        std::cout << "   volume(3, 4, 5) = "; print_result(result); std::cout << "\n\n";
    }

    // Example 5: Four-argument function
    std::cout << "5. Four-argument function:\n";
    {
        auto weighted_sum = [](double a, double b, double c, double w) {
            return (a + b + c) * w;
        };

        auto result = fxt::curry(weighted_sum)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{10.0});

        std::cout << "   (1 + 2 + 3) * 10 = "; print_result(result); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 3: Error Propagation
    // ========================================================================

    std::cout << "PART 3: Error Propagation\n";
    std::cout << "=========================\n\n";

    // Short-circuit semantics: the pipeline stops at the first error.
    // Subsequent with() steps are skipped and the error is forwarded.

    auto sum3 = [](double a, double b, double c) { return a + b + c; };

    auto valid   = [](double v)             { return fxt::expected<double, std::string>{v}; };
    auto invalid = [](const std::string& e) { return fxt::expected<double, std::string>{fxt::unexpected(e)}; };

    // Example 6: Error in first argument
    std::cout << "6. Error in first argument:\n";
    {
        auto result = fxt::curry(sum3)
            | fxt::with(invalid("pressure out of range"))
            | fxt::with(valid(2.0))
            | fxt::with(valid(3.0));

        std::cout << "   "; print_result(result); std::cout << "\n\n";
    }

    // Example 7: Error in middle argument
    std::cout << "7. Error in middle argument:\n";
    {
        auto result = fxt::curry(sum3)
            | fxt::with(valid(1.0))
            | fxt::with(invalid("temperature out of range"))
            | fxt::with(valid(3.0));

        std::cout << "   "; print_result(result); std::cout << "\n\n";
    }

    // Example 8: Error in last argument
    std::cout << "8. Error in last argument:\n";
    {
        auto result = fxt::curry(sum3)
            | fxt::with(valid(1.0))
            | fxt::with(valid(2.0))
            | fxt::with(invalid("quality out of range"));

        std::cout << "   "; print_result(result); std::cout << "\n\n";
    }

    // Example 9: Multiple invalid arguments — first error wins
    std::cout << "9. Multiple invalid arguments — first error wins:\n";
    {
        auto result = fxt::curry(sum3)
            | fxt::with(invalid("first failed"))
            | fxt::with(invalid("second failed"))
            | fxt::with(invalid("third failed"));

        std::cout << "   "; print_result(result);
        std::cout << "  (only the first error is reported)\n\n";
    }

    // ========================================================================
    // Part 4: Mixed Argument Types
    // ========================================================================

    std::cout << "PART 4: Mixed Argument Types\n";
    std::cout << "============================\n\n";

    // with() is not limited to homogeneous types — each argument can have a
    // different value type T, provided all share the same error type E.

    // Example 10: int and string arguments
    std::cout << "10. int and string arguments:\n";
    {
        auto label = [](int code, const std::string& unit, double scale) {
            return std::format("code={}, unit={}, scaled={:.2f}", code, unit, code * scale);
        };

        auto result = fxt::curry(label)
            | fxt::with(fxt::expected<int,         std::string>{42})
            | fxt::with(fxt::expected<std::string, std::string>{"bar"})
            | fxt::with(fxt::expected<double,      std::string>{0.01});

        std::cout << "    "; print_result(result); std::cout << "\n\n";
    }

    // Example 11: Error on the string argument
    std::cout << "11. Error on the string argument:\n";
    {
        auto label = [](int code, const std::string& unit, double scale) {
            return std::format("code={}, unit={}, scaled={:.2f}", code, unit, code * scale);
        };

        auto result = fxt::curry(label)
            | fxt::with(fxt::expected<int,         std::string>{42})
            | fxt::with(fxt::expected<std::string, std::string>{fxt::unexpected<std::string>("unknown unit")})
            | fxt::with(fxt::expected<double,      std::string>{0.01});

        std::cout << "    "; print_result(result); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 5: Practical Validation Pipeline (xlthermo-style)
    // ========================================================================

    std::cout << "PART 5: Practical Validation Pipeline\n";
    std::cout << "=====================================\n\n";

    // The primary intended use case: each raw input from an external source
    // (e.g., an Excel cell) is validated into expected<T, Error> and then
    // fed into a pure computation function via curry + with().
    //
    // This keeps validation and computation cleanly separated.

    // Define validators that return expected<double, string>
    auto validate_range = [](const std::string& name, double lo, double hi, double v)
        -> fxt::expected<double, std::string>
    {
        if (v < lo || v > hi)
            return fxt::unexpected(std::format("{} = {:.3f} is outside [{:.3f}, {:.3f}]", name, v, lo, hi));
        return v;
    };

    auto validate_positive = [](const std::string& name, double v)
        -> fxt::expected<double, std::string>
    {
        if (v <= 0.0)
            return fxt::unexpected(std::format("{} must be positive, got {:.3f}", name, v));
        return v;
    };

    // Pure computation function — no validation logic inside
    auto compute_heat_transfer = [](double area, double delta_T, double conductivity) {
        return area * delta_T * conductivity;   // Q = A * dT * k  (simplified)
    };

    // Example 12: All inputs valid
    std::cout << "12. All inputs valid:\n";
    {
        auto result = fxt::curry(compute_heat_transfer)
            | fxt::with(validate_range("area",         0.0, 100.0, 2.5))
            | fxt::with(validate_range("delta_T",    -50.0, 200.0, 80.0))
            | fxt::with(validate_positive("conductivity",  0.45));

        std::cout << "    Q = A * dT * k = "; print_result(result); std::cout << " W\n\n";
    }

    // Example 13: Invalid area
    std::cout << "13. Invalid area:\n";
    {
        auto result = fxt::curry(compute_heat_transfer)
            | fxt::with(validate_range("area",         0.0, 100.0, -5.0))
            | fxt::with(validate_range("delta_T",    -50.0, 200.0, 80.0))
            | fxt::with(validate_positive("conductivity",  0.45));

        std::cout << "    "; print_result(result); std::cout << "\n\n";
    }

    // Example 14: Invalid conductivity (negative)
    std::cout << "14. Invalid conductivity:\n";
    {
        auto result = fxt::curry(compute_heat_transfer)
            | fxt::with(validate_range("area",         0.0, 100.0, 2.5))
            | fxt::with(validate_range("delta_T",    -50.0, 200.0, 80.0))
            | fxt::with(validate_positive("conductivity", -0.45));

        std::cout << "    "; print_result(result); std::cout << "\n\n";
    }

    // Example 15: Two inputs invalid — first error reported
    std::cout << "15. Area and conductivity both invalid — first error reported:\n";
    {
        auto result = fxt::curry(compute_heat_transfer)
            | fxt::with(validate_range("area",         0.0, 100.0, 150.0))  // too large
            | fxt::with(validate_range("delta_T",    -50.0, 200.0, 80.0))
            | fxt::with(validate_positive("conductivity", -0.45));            // also bad

        std::cout << "    "; print_result(result); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 6: Composing with Other Adaptors
    // ========================================================================

    std::cout << "PART 6: Composing with Other Adaptors\n";
    std::cout << "======================================\n\n";

    // The result of a with() chain is an ordinary expected<T, E>, so it can be
    // further composed with transform(), and_then(), or_else(), etc.

    // Example 16: with() chain followed by transform()
    std::cout << "16. with() chain followed by transform():\n";
    {
        auto add = [](double a, double b) { return a + b; };

        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::transform([](double x) { return x * x; });   // square the sum

        std::cout << "    (3 + 4)^2 = "; print_result(result); std::cout << "\n\n";
    }

    // Example 17: with() chain followed by and_then() for further validation
    std::cout << "17. with() chain followed by and_then() for further validation:\n";
    {
        auto add = [](double a, double b) { return a + b; };

        auto check_finite = [](double x) -> fxt::expected<double, std::string> {
            if (x > 1e6) return fxt::unexpected<std::string>("result overflow");
            return x;
        };

        auto ok_result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::and_then(check_finite);

        auto big_result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{9e5})
            | fxt::with(fxt::expected<double, std::string>{2e5})
            | fxt::and_then(check_finite);

        std::cout << "    1 + 2 then check_finite: "; print_result(ok_result);  std::cout << "\n";
        std::cout << "    9e5 + 2e5 then check_finite: "; print_result(big_result); std::cout << "\n\n";
    }

    // Example 18: with() error propagates through subsequent transform()
    std::cout << "18. with() error propagates through subsequent transform():\n";
    {
        auto add = [](double a, double b) { return a + b; };

        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("arg1 invalid")})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::transform([](double x) { return x * x; });

        std::cout << "    "; print_result(result); std::cout << "  (error skips transform)\n\n";
    }

    // ========================================================================
    // Part 7: Contrast with and_then
    // ========================================================================

    std::cout << "PART 7: Contrast with and_then\n";
    std::cout << "===============================\n\n";

    // and_then is for functions that *themselves* return expected — they handle
    // validation or fallible operations internally:
    //
    //   value | fxt::and_then(validate_and_compute)
    //
    // with() is for functions that are *pure* — validation happens outside,
    // in the arguments, and the function just computes:
    //
    //   fxt::curry(pure_compute) | fxt::with(validate(a)) | fxt::with(validate(b))
    //
    // Both short-circuit on error; with() composes better when multiple
    // independent arguments need validation.

    std::cout << "19. Same result with and_then vs with():\n";
    {
        auto validate_positive_d = [](double v) -> fxt::expected<double, std::string> {
            if (v <= 0) return fxt::unexpected<std::string>("must be positive");
            return v;
        };

        double raw_a = 3.0, raw_b = 4.0;

        // Style A: and_then chains — each step validates and passes to the next
        auto with_and_then = fxt::expected<double, std::string>{raw_a}
            | fxt::and_then(validate_positive_d)
            | fxt::and_then([&](double a) {
                return fxt::expected<double, std::string>{raw_b}
                    | fxt::and_then(validate_positive_d)
                    | fxt::transform([a](double b) { return a + b; });
              });

        // Style B: curry + with — validation separated from computation
        auto with_applicative = fxt::curry([](double a, double b) { return a + b; })
            | fxt::with(validate_positive_d(raw_a))
            | fxt::with(validate_positive_d(raw_b));

        std::cout << "    and_then style:     "; print_result(with_and_then);   std::cout << "\n";
        std::cout << "    applicative style:  "; print_result(with_applicative); std::cout << "\n";
        std::cout << "    (same result; applicative is flatter for independent args)\n\n";
    }

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
