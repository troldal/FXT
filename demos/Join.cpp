//
// Created by kenne on 09/06/2026.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

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
    std::cout << "=== fxt::join Monadic Flatten Demo ===\n\n";

    // fxt::join is the canonical monadic join (flatten): it collapses a
    // doubly-wrapped expected<expected<T, E>, E> into a single expected<T, E>.
    //
    //   - outer error  -> propagated
    //   - inner error  -> propagated
    //   - both present -> the inner value survives
    //
    // It is constrained (via expected_like on both layers) so it only accepts a
    // genuinely nested expected; calling it on anything else is a compile error.

    using Inner  = fxt::expected<int, std::string>;
    using Nested = fxt::expected<Inner, std::string>;

    // ========================================================================
    // Part 1: The Fundamentals
    // ========================================================================

    std::cout << "PART 1: Fundamentals\n";
    std::cout << "====================\n\n";

    // Example 1: Both layers present — inner value survives
    std::cout << "1. Outer value + inner value:\n";
    {
        Nested nested{Inner{42}};
        auto flat = fxt::join()(nested);            // call syntax
        std::cout << "   join(expected{expected{42}}) -> "; print_result(flat); std::cout << "\n\n";
    }

    // Example 2: Inner error is propagated
    std::cout << "2. Outer value + inner error:\n";
    {
        Nested nested{Inner{fxt::unexpected<std::string>("inner failure")}};
        auto flat = nested | fxt::join();           // pipe syntax
        std::cout << "   join(expected{unexpected}) -> "; print_result(flat); std::cout << "\n\n";
    }

    // Example 3: Outer error is propagated
    std::cout << "3. Outer error:\n";
    {
        Nested nested{fxt::unexpected<std::string>("outer failure")};
        auto flat = nested | fxt::join();
        std::cout << "   join(unexpected) -> "; print_result(flat); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 2: Completing a curry | with() Pipeline (Primary Use Case)
    // ========================================================================

    std::cout << "PART 2: Completing a curry | with() Pipeline\n";
    std::cout << "============================================\n\n";

    // When a curried callable *itself* returns expected<R, E>, a with() chain
    // ends up producing expected<expected<R, E>, E>. join() flattens it back to
    // the single-layer expected<R, E> that the rest of a pipeline expects.

    auto divide = [](double a, double b) -> fxt::expected<double, std::string> {
        if (b == 0.0) return fxt::unexpected<std::string>("division by zero");
        return a / b;
    };

    // Example 4: All arguments valid, callable succeeds
    std::cout << "4. curry(divide) | with(10) | with(2) | join:\n";
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::join();

        std::cout << "   10 / 2 = "; print_result(result); std::cout << "  (flattened, single layer)\n\n";
    }

    // Example 5: Callable itself fails — inner error surfaces after join
    std::cout << "5. Callable fails (divide by zero):\n";
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{0.0})
            | fxt::join();

        std::cout << "   10 / 0 = "; print_result(result); std::cout << "\n\n";
    }

    // Example 6: An argument is invalid — outer error short-circuits
    std::cout << "6. Invalid argument short-circuits before the callable runs:\n";
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("bad numerator")})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::join();

        std::cout << "   "; print_result(result); std::cout << "\n\n";
    }

    // ========================================================================
    // Part 3: Composing with Other Adaptors
    // ========================================================================

    std::cout << "PART 3: Composing with Other Adaptors\n";
    std::cout << "=====================================\n\n";

    // The result of join() is an ordinary expected<T, E>, so it composes freely
    // with transform(), and_then(), etc.

    // Example 7: join then transform
    std::cout << "7. join then transform:\n";
    {
        Nested nested{Inner{20}};
        auto result = nested
            | fxt::join()
            | fxt::transform([](int x) { return x + 1; });

        std::cout << "   join then +1 -> "; print_result(result); std::cout << "\n\n";
    }

    // Example 8: inner error flows through a later transform
    std::cout << "8. inner error flows through a later transform:\n";
    {
        Nested nested{Inner{fxt::unexpected<std::string>("boom")}};
        auto result = nested
            | fxt::join()
            | fxt::transform([](int x) { return x + 1; });

        std::cout << "   "; print_result(result); std::cout << "  (transform skipped)\n\n";
    }

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
