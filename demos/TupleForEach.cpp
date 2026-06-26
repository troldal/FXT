/*
    Demo: fxt::tuple_for_each and fxt::mtuple_for_each

    fxt::tuple_for_each applies a callable to every element of an fxt::tuple
    or fxt::flat_tuple for its side-effects (printing, accumulation, mutation).
    fxt::mtuple_for_each is the monadic lift: it applies the callable when the
    tuple is held inside an fxt::expected or fxt::optional, propagating errors
    or nullopt without executing the callable.
*/

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// Part 1: Basic element visits
// ============================================================================

void example_basic()
{
    std::cout << "Part 1: Basic element visits\n"
              << "-----------------------------\n";

    auto t = fxt::make_tuple(1, 2.5, std::string("hello"));

    std::cout << "   fxt::tuple{1, 2.5, \"hello\"}:\n   ";
    fxt::tuple_for_each(t, [](const auto& x) { std::cout << x << "  "; });
    std::cout << "\n\n";

    auto ft = fxt::make_flat_tuple(10, 20, 30);
    std::cout << "   fxt::flat_tuple{10, 20, 30}:\n   ";
    fxt::tuple_for_each(ft, [](const auto& x) { std::cout << x << "  "; });
    std::cout << "\n\n";
}

// ============================================================================
// Part 2: Side-effect accumulation
// ============================================================================

void example_accumulation()
{
    std::cout << "Part 2: Accumulation via captured reference\n"
              << "--------------------------------------------\n";

    int sum = 0;
    fxt::tuple_for_each(fxt::make_tuple(10, 20, 30), [&](int x) { sum += x; });
    std::cout << "   sum of {10, 20, 30} = " << sum << "\n\n";

    std::vector<std::string> words;
    fxt::tuple_for_each(
        fxt::make_tuple(std::string("one"), std::string("two"), std::string("three")),
        [&](const std::string& s) { words.push_back(s); });
    std::cout << "   collected strings:";
    for (auto& w : words) std::cout << " \"" << w << "\"";
    std::cout << "\n\n";
}

// ============================================================================
// Part 3: In-place mutation
// ============================================================================

void example_mutation()
{
    std::cout << "Part 3: In-place mutation\n"
              << "--------------------------\n";

    auto t = fxt::make_tuple(1, 2, 3, 4, 5);
    fxt::tuple_for_each(t, [](auto& x) { x *= 2; });

    std::cout << "   after doubling {1,2,3,4,5}:  ";
    fxt::tuple_for_each(t, [](int x) { std::cout << x << "  "; });
    std::cout << "\n\n";
}

// ============================================================================
// Part 4: Pipeline (curried) form
// ============================================================================

void example_pipeline()
{
    std::cout << "Part 4: Pipeline / curried form\n"
              << "--------------------------------\n";

    std::cout << "   elements of {7, 8, 9}:  ";
    fxt::make_tuple(7, 8, 9)
        | fxt::tuple_for_each([](int x) { std::cout << x << "  "; });
    std::cout << "\n";

    // Chain with other tuple operations
    std::cout << "   {1,2,3,4,5} | transform(*2) | for_each(print):  ";
    fxt::make_tuple(1, 2, 3, 4, 5)
        | fxt::tuple_transform([](int x) { return x * 2; })
        | fxt::tuple_for_each([](int x) { std::cout << x << "  "; });
    std::cout << "\n\n";
}

// ============================================================================
// Part 5: Stored adaptor is reusable
// ============================================================================

void example_reusable_adaptor()
{
    std::cout << "Part 5: Stored (reusable) adaptor\n"
              << "----------------------------------\n";

    int total = 0;
    auto accumulate = fxt::tuple_for_each([&](int x) { total += x; });

    fxt::make_tuple(1, 2, 3) | accumulate;
    fxt::make_tuple(10, 20)  | accumulate;

    std::cout << "   applied to {1,2,3} then {10,20} → total = " << total << "\n\n";
}

// ============================================================================
// Part 6: mtuple_for_each — monadic lift (fxt::expected)
// ============================================================================

void example_monadic_expected()
{
    std::cout << "Part 6: mtuple_for_each with fxt::expected\n"
              << "-------------------------------------------\n";

    // Value path — callable is invoked
    auto exp_ok = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3)};

    std::cout << "   expected with value {1,2,3}:  ";
    exp_ok | fxt::mtuple_for_each([](int x) { std::cout << x << "  "; });
    std::cout << "\n";

    // Error path — callable is not invoked
    auto exp_err = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::unexpected("something went wrong")};

    std::cout << "   expected with error → callable invoked? ";
    bool called = false;
    exp_err | fxt::mtuple_for_each([&](int) { called = true; });
    std::cout << (called ? "yes (unexpected!)" : "no (correct)") << "\n";

    // Pipeline continuation — monad passes through unchanged
    auto result = exp_ok
        | fxt::mtuple_for_each([](int) {})                         // side-effect step
        | fxt::mtuple_apply([](int a, int b, int c) { return a + b + c; });

    std::cout << "   {1,2,3} | for_each(noop) | apply(sum) = "
              << (result.has_value() ? std::to_string(*result) : result.error()) << "\n\n";
}

// ============================================================================
// Part 7: mtuple_for_each — monadic lift (fxt::optional)
// ============================================================================

void example_monadic_optional()
{
    std::cout << "Part 7: mtuple_for_each with fxt::optional\n"
              << "-------------------------------------------\n";

    auto opt_ok = fxt::optional<fxt::tuple<double, double>>{
        fxt::make_tuple(1.5, 2.5)};

    double sum = 0.0;
    opt_ok | fxt::mtuple_for_each([&](double x) { sum += x; });
    std::cout << "   optional{1.5, 2.5} → sum = " << sum << "\n";

    fxt::optional<fxt::tuple<double, double>> opt_empty{};
    int count = 0;
    opt_empty | fxt::mtuple_for_each([&](double) { ++count; });
    std::cout << "   empty optional → callable invoked " << count << " time(s)\n\n";
}

// ============================================================================
// Main
// ============================================================================

int main()
{
    std::cout << "\n========================================\n"
              << "  fxt::tuple_for_each Demo\n"
              << "========================================\n\n";

    example_basic();
    example_accumulation();
    example_mutation();
    example_pipeline();
    example_reusable_adaptor();
    example_monadic_expected();
    example_monadic_optional();

    std::cout << "========================================\n"
              << "  All examples completed successfully!\n"
              << "========================================\n\n";
}
