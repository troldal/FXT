//
// Created by kenne on 28/06/2026.
//

#include <fxt.hpp>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

auto safe_divide(double a, double b) -> fxt::expected<double, std::string>
{
    if (b == 0.0) return fxt::unexpected<std::string>{"division by zero"};
    return a / b;
}

auto parse_positive(const std::string& s) -> fxt::optional<int>
{
    try {
        int v = std::stoi(s);
        return v > 0 ? fxt::optional<int>{v} : fxt::nullopt;
    } catch (...) { return fxt::nullopt; }
}

int main()
{
    std::cout << "=== Format support for fxt::optional and fxt::expected ===\n\n";

    // -------------------------------------------------------------------------
    std::cout << "--- std::format for optional ---\n";

    for (const char* s : {"42", "-1", "bad"}) {
        auto opt = parse_positive(s);
        std::cout << "  parse_positive(\"" << s << "\") = " << std::format("{}", opt) << "\n";
    }

    // Format spec forwarded to the value formatter
    fxt::optional<double> pi = 3.14159;
    fxt::optional<double> none;
    std::cout << "  pi (2dp):   " << std::format("{:.2f}", pi)   << "\n";
    std::cout << "  none (2dp): " << std::format("{:.2f}", none)  << "\n";

    // -------------------------------------------------------------------------
    std::cout << "\n--- std::format for expected ---\n";

    for (auto [a, b] : {std::pair{10.0, 4.0}, {10.0, 0.0}}) {
        auto exp = safe_divide(a, b);
        std::cout << "  safe_divide(" << a << ", " << b << ") = "
                  << std::format("{}", exp) << "\n";
    }

    // Format spec forwarded to value formatter
    fxt::expected<double, std::string> result = 2.71828;
    std::cout << "  e (3dp):  " << std::format("{:.3f}", result) << "\n";

    fxt::expected<double, std::string> error = fxt::unexpected<std::string>{"overflow"};
    std::cout << "  error:    " << std::format("{:.3f}", error)  << "\n";

    // -------------------------------------------------------------------------
    std::cout << "\n--- std::format for expected<void, E> ---\n";
    {
        fxt::expected<void, std::string> ok;
        fxt::expected<void, std::string> err = fxt::unexpected<std::string>{"not permitted"};
        std::cout << "  success: " << std::format("{}", ok)  << "\n";
        std::cout << "  failure: " << std::format("{}", err) << "\n";
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- std::format in fxt::tap callbacks (the primary use case) ---\n";

    // Before Format.hpp: manual formatting inside tee was required
    // After Format.hpp: just use std::format("{}", v) directly
    auto pipeline_result =
        safe_divide(100.0, 4.0)
        | fxt::tap([](const auto& v) {
              std::cout << "  [tee] after divide:    " << std::format("{:.1f}", v) << "\n";
          })
        | fxt::transform([](double x) { return x - 5.0; })
        | fxt::tap([](const auto& v) {
              std::cout << "  [tee] after subtract:  " << std::format("{:.1f}", v) << "\n";
          })
        | fxt::ensure([](double x) { return x > 0.0; }, std::string{"result not positive"})
        | fxt::tap([](const auto& v) {
              std::cout << "  [tee] after ensure:    " << std::format("{}", v) << "\n";
          });

    std::cout << "  final: " << std::format("{:.1f}", pipeline_result) << "\n";

    // -------------------------------------------------------------------------
    std::cout << "\n--- operator<< (requires 'using fxt::operator<<') ---\n";
    {
        using fxt::operator<<;

        fxt::optional<int> a = 7, b;
        fxt::expected<int, std::string> ok = 99;
        fxt::expected<int, std::string> err = fxt::unexpected<std::string>{"bad input"};

        std::cout << "  optional{7}:    " << a   << "\n";
        std::cout << "  optional{}:     " << b   << "\n";
        std::cout << "  expected{99}:   " << ok  << "\n";
        std::cout << "  expected{err}:  " << err << "\n";

        // Tee with operator<<
        safe_divide(30.0, 6.0)
            | fxt::tap([](const auto& v) {
                  using fxt::operator<<;
                  std::cout << "  [tee] value = " << v << "\n";
              });
    }

    // -------------------------------------------------------------------------
    std::cout << "\n--- Building log messages with std::format ---\n";
    {
        std::vector<fxt::expected<double, std::string>> results = {
            safe_divide(10.0, 2.0),
            safe_divide(10.0, 0.0),
            safe_divide(7.5, 3.0),
        };

        for (std::size_t i = 0; i < results.size(); ++i)
            std::cout << "  result[" << i << "]: " << std::format("{:.2f}", results[i]) << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
