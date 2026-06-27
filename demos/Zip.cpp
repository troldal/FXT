//
// Demo: fxt::mtuple_zip
//
// fxt::mtuple_zip combines N monadic values (all fxt::optional, or all fxt::expected
// with the same error type) into a single monad containing an fxt::tuple of
// their unwrapped values. It short-circuits on the first empty/error input.
//
// Combined with fxt::mapply, it enables a LINQ-style "from ... from ... select"
// idiom for monadic computations:
//
//     var fullName =
//         from f in firstName
//         from l in lastName
//         from t in title
//         select $"{t} {f} {l}";
//
// becomes:
//
//     auto fullName = fxt::zip(firstName, lastName, title)
//                   | fxt::mapply([](auto f, auto l, auto t) {
//                         return std::format("{} {} {}", t, f, l);
//                     });
//

#include <fxt.hpp>

#include <iostream>
#include <sstream>
#include <string>

// ============================================================================
// Example 1: LINQ-style composition with fxt::expected
// ============================================================================

void example_linq_style_expected()
{
    std::cout << "Example 1: LINQ-style composition with fxt::expected\n";
    std::cout << "=====================================================\n\n";

    using E = fxt::expected<std::string, std::string>;

    auto firstName = E{ "John" };
    auto lastName  = E{ "Doe" };
    auto title     = E{ "Dr." };

    auto fullName = fxt::zip(firstName, lastName, title)
                  | fxt::mapply([](const std::string& f,
                                   const std::string& l,
                                   const std::string& t) {
                        std::ostringstream os;
                        os << t << ' ' << f << ' ' << l;
                        return os.str();
                    });

    if (fullName) {
        std::cout << "   Full name: \"" << *fullName << "\"\n\n";
    }
}

// ============================================================================
// Example 2: Heterogeneous types
// ============================================================================

void example_heterogeneous_types()
{
    std::cout << "Example 2: Heterogeneous types\n";
    std::cout << "===============================\n\n";

    auto a = fxt::expected<int,         std::string>{ 42 };
    auto b = fxt::expected<double,      std::string>{ 3.14 };
    auto c = fxt::expected<std::string, std::string>{ "hello" };

    // mzip alone produces a monad<tuple<...>>
    auto zipped = fxt::zip(a, b, c);

    if (zipped) {
        std::cout << "   mzip(a, b, c) = ("
                  << fxt::get<0>(*zipped) << ", "
                  << fxt::get<1>(*zipped) << ", \""
                  << fxt::get<2>(*zipped) << "\")\n\n";
    }
}

// ============================================================================
// Example 3: Short-circuit on first error (fxt::expected)
// ============================================================================

void example_short_circuit_expected()
{
    std::cout << "Example 3: Short-circuit on first error (fxt::expected)\n";
    std::cout << "========================================================\n\n";

    using E = fxt::expected<int, std::string>;

    auto a = E{ 1 };
    auto b = E{ fxt::unexpected{ std::string{ "b failed" } } };
    auto c = E{ fxt::unexpected{ std::string{ "c failed" } } };

    auto result = fxt::zip(a, b, c)
                | fxt::mapply([](int x, int y, int z) { return x + y + z; });

    if (!result) {
        std::cout << "   First encountered error: \"" << result.error() << "\"\n\n";
    }
}

// ============================================================================
// Example 4: Short-circuit on nullopt (fxt::optional)
// ============================================================================

void example_short_circuit_optional()
{
    std::cout << "Example 4: Short-circuit on nullopt (fxt::optional)\n";
    std::cout << "====================================================\n\n";

    auto a = fxt::optional<int>{ 10 };
    auto b = fxt::optional<int>{ 20 };
    auto c = fxt::optional<int>{ 30 };

    auto sum = fxt::zip(a, b, c)
             | fxt::mapply([](int x, int y, int z) { return x + y + z; });

    if (sum) {
        std::cout << "   All present  -> sum = " << *sum << "\n";
    }

    auto bEmpty = fxt::optional<int>{};
    auto sum2   = fxt::zip(a, bEmpty, c)
                | fxt::mapply([](int x, int y, int z) { return x + y + z; });

    if (!sum2) {
        std::cout << "   One missing  -> nullopt\n\n";
    }
}

// ============================================================================
// Example 5: Single-argument mzip (lift into tuple)
// ============================================================================

void example_single_argument()
{
    std::cout << "Example 5: Single-argument mzip\n";
    std::cout << "================================\n\n";

    auto a = fxt::expected<int, std::string>{ 7 };

    // mzip with a single argument lifts the value into a 1-tuple inside the monad.
    auto lifted = fxt::zip(a);

    if (lifted) {
        std::cout << "   mzip(expected<int>{7}) -> expected<tuple<int>> containing ("
                  << fxt::get<0>(*lifted) << ")\n\n";
    }
}

// ============================================================================
// Example 6: Composing with fxt::mtuple_append
// ============================================================================

void example_compose_with_mtuple_append()
{
    std::cout << "Example 6: Composing with fxt::mtuple_append\n";
    std::cout << "=============================================\n\n";

    auto x = fxt::expected<int, std::string>{ 1 };
    auto y = fxt::expected<int, std::string>{ 2 };
    auto z = fxt::expected<int, std::string>{ 3 };

    // Start with mzip, then keep appending more values (plain or monadic).
    auto result = fxt::zip(x, y)
                | fxt::mtuple_append(z)
                | fxt::mtuple_append(100, 200);

    if (result) {
        std::cout << "   mzip(x, y) | mtuple_append(z) | mtuple_append(100, 200) = ("
                  << fxt::get<0>(*result) << ", "
                  << fxt::get<1>(*result) << ", "
                  << fxt::get<2>(*result) << ", "
                  << fxt::get<3>(*result) << ", "
                  << fxt::get<4>(*result) << ")\n\n";
    }
}

// ============================================================================
// Example 7: Realistic pipeline - parse, validate, combine
// ============================================================================

namespace {
    fxt::expected<int, std::string> parseInt(const std::string& s)
    {
        try {
            size_t pos = 0;
            int    v   = std::stoi(s, &pos);
            if (pos != s.size()) return fxt::unexpected{ "trailing characters in '" + s + "'" };
            return v;
        }
        catch (...) {
            return fxt::unexpected{ "cannot parse '" + s + "' as int" };
        }
    }

    fxt::expected<int, std::string> positive(int v)
    {
        if (v <= 0) return fxt::unexpected{ "value must be positive (got " + std::to_string(v) + ")" };
        return v;
    }
}

void example_realistic_pipeline()
{
    std::cout << "Example 7: Realistic pipeline\n";
    std::cout << "==============================\n\n";

    auto width  = parseInt("1920") | fxt::and_then(positive);
    auto height = parseInt("1080") | fxt::and_then(positive);
    auto depth  = parseInt("24")   | fxt::and_then(positive);

    auto pixels = fxt::zip(width, height, depth)
                | fxt::mapply([](int w, int h, int d) {
                      return static_cast<long long>(w) * h * (d / 8);
                  });

    if (pixels) {
        std::cout << "   Success: total bytes = " << *pixels << "\n";
    }

    // Now make one of them fail.
    auto badHeight = parseInt("-5") | fxt::and_then(positive);
    auto failed    = fxt::zip(width, badHeight, depth)
                   | fxt::mapply([](int w, int h, int d) {
                         return static_cast<long long>(w) * h * (d / 8);
                     });

    if (!failed) {
        std::cout << "   Failure: " << failed.error() << "\n\n";
    }
}

// ============================================================================
// Main
// ============================================================================

int main()
{
    std::cout << "\n=== fxt::mtuple_zip Demo ===\n\n";

    example_linq_style_expected();
    example_heterogeneous_types();
    example_short_circuit_expected();
    example_short_circuit_optional();
    example_single_argument();
    example_compose_with_mtuple_append();
    example_realistic_pipeline();

    std::cout << "=== Demo Complete ===\n\n";
    return 0;
}


