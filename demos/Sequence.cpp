//
// Created by kenne on 07/06/2026.
//

#include <fxt.hpp>
#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <format>

// ============================================================================
// Domain: Excel worksheet validation
//
// In the xlthermo add-in, worksheet functions receive raw doubles from Excel
// cells. Before passing them to thermodynamic computations, each value must be
// validated against physical constraints. If any value is invalid the whole
// batch should fail with a descriptive error rather than producing a garbage
// result.
//
// sequence() and traverse() let us express this cleanly:
//
//   traverse(raw_column, validator) -> expected<vector<double>, XLError>
//
// ============================================================================

using XLError = std::string;    // simplified stand-in for the real XLError type

// ---------------------------------------------------------------------------
// Validators — return expected<double, XLError>
// ---------------------------------------------------------------------------

fxt::expected<double, XLError> validate_pressure(double p)
{
    if (p <= 0.0)
        return fxt::unexpected<XLError>(std::format("pressure {} bar is not positive", p));
    if (p > 220.0)
        return fxt::unexpected<XLError>(std::format("pressure {} bar exceeds critical point", p));
    return p;
}

fxt::expected<double, XLError> validate_temperature(double t)
{
    if (t < 0.01)
        return fxt::unexpected<XLError>(std::format("temperature {} °C is below freezing", t));
    if (t > 374.0)
        return fxt::unexpected<XLError>(std::format("temperature {} °C exceeds critical point", t));
    return t;
}

fxt::expected<double, XLError> validate_quality(double x)
{
    if (x < 0.0 || x > 1.0)
        return fxt::unexpected<XLError>(std::format("steam quality {} is outside [0, 1]", x));
    return x;
}

// ---------------------------------------------------------------------------
// Helper: print an expected<vector<double>, XLError>
// ---------------------------------------------------------------------------

template<typename T>
void print_result(const fxt::expected<std::vector<T>, XLError>& r, const std::string& label)
{
    std::cout << "  " << label << ": ";
    if (r.has_value()) {
        std::cout << "[";
        for (std::size_t i = 0; i < r->size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << (*r)[i];
        }
        std::cout << "]\n";
    } else {
        std::cout << "ERROR — " << r.error() << "\n";
    }
}

int main()
{
    std::cout << "=== fxt::sequence and fxt::traverse Demo ===\n\n";

    // ========================================================================
    // PART 1: sequence() — turn a container of expected into expected<container>
    // ========================================================================

    std::cout << "PART 1: fxt::sequence\n";
    std::cout << "=====================\n\n";

    // 1a. All pressures valid
    std::cout << "1a. All pressures valid:\n";
    {
        std::vector<fxt::expected<double, XLError>> pressures = {
            1.0, 5.5, 100.0, 200.0
        };
        auto result = fxt::sequence(pressures);
        print_result(result, "result");
        std::cout << "\n";
    }

    // 1b. One pressure invalid — sequence short-circuits
    std::cout << "1b. Third pressure is negative (short-circuits at third element):\n";
    {
        std::vector<fxt::expected<double, XLError>> pressures = {
            validate_pressure(1.0),
            validate_pressure(5.5),
            validate_pressure(-3.0),    // invalid
            validate_pressure(200.0)    // never reached
        };
        auto result = fxt::sequence(pressures);
        print_result(result, "result");
        std::cout << "\n";
    }

    // 1c. Empty container -> expected<empty vector>
    std::cout << "1c. Empty container:\n";
    {
        std::vector<fxt::expected<double, XLError>> empty;
        auto result = fxt::sequence(empty);
        std::cout << "  result: " << (result.has_value() ? "OK (empty vector)" : "error") << "\n\n";
    }

    // ========================================================================
    // PART 2: traverse() free function — map-and-sequence in one pass
    // ========================================================================

    std::cout << "PART 2: fxt::traverse (free function)\n";
    std::cout << "======================================\n\n";

    // 2a. Validate a column of raw pressure readings — all valid
    std::cout << "2a. Validate pressure column — all valid:\n";
    {
        std::vector<double> raw_pressures = {10.0, 25.0, 50.0, 100.0};
        auto result = fxt::traverse(raw_pressures, validate_pressure);
        print_result(result, "validated pressures");
        std::cout << "\n";
    }

    // 2b. Validate a column of temperatures — one invalid
    std::cout << "2b. Validate temperature column — one cell above critical point:\n";
    {
        std::vector<double> raw_temperatures = {100.0, 200.0, 380.0, 150.0};
        //                                                       ^ invalid: 380 > 374
        auto result = fxt::traverse(raw_temperatures, validate_temperature);
        print_result(result, "validated temperatures");
        std::cout << "\n";
    }

    // 2c. Validate steam quality fractions
    std::cout << "2c. Validate steam quality column — one cell out of [0,1]:\n";
    {
        std::vector<double> raw_qualities = {0.0, 0.5, 1.2, 0.8};
        //                                             ^ invalid: 1.2 > 1
        auto result = fxt::traverse(raw_qualities, validate_quality);
        print_result(result, "validated qualities");
        std::cout << "\n";
    }

    // ========================================================================
    // PART 3: traverse() pipe form — integrates with pipeline syntax
    // ========================================================================

    std::cout << "PART 3: fxt::traverse (pipe adaptor)\n";
    std::cout << "=====================================\n\n";

    // 3a. Pipe form — identical semantics to free function
    std::cout << "3a. Pipe form, all valid:\n";
    {
        std::vector<double> raw = {1.0, 2.0, 3.0};
        auto result = raw | fxt::traverse(validate_pressure);
        print_result(result, "result");
        std::cout << "\n";
    }

    // 3b. Pipe form — invalid input
    std::cout << "3b. Pipe form, first value invalid:\n";
    {
        std::vector<double> raw = {-5.0, 10.0, 15.0};
        auto result = raw | fxt::traverse(validate_pressure);
        print_result(result, "result");
        std::cout << "\n";
    }

    // 3c. Stored adaptor — reuse the same validator across multiple columns
    std::cout << "3c. Stored adaptor — same validator applied to two different columns:\n";
    {
        auto validate_col = fxt::traverse(validate_pressure);

        std::vector<double> col_A = {1.0, 5.0, 10.0};
        std::vector<double> col_B = {0.5, 250.0, 3.0};   // second value exceeds limit

        auto result_A = col_A | validate_col;
        auto result_B = col_B | validate_col;

        print_result(result_A, "column A");
        print_result(result_B, "column B");
        std::cout << "\n";
    }

    // ========================================================================
    // PART 4: Non-vector containers
    // ========================================================================

    std::cout << "PART 4: Container generality\n";
    std::cout << "============================\n\n";

    // traverse works on any container satisfying sequenceable_container
    std::cout << "4a. std::list of temperatures:\n";
    {
        std::list<double> temps = {100.0, 200.0, 300.0};
        auto result = temps | fxt::traverse(validate_temperature);
        if (result.has_value()) {
            std::cout << "  validated: [";
            for (auto it = result->begin(); it != result->end(); ++it) {
                if (it != result->begin()) std::cout << ", ";
                std::cout << *it;
            }
            std::cout << "]\n";
        } else {
            std::cout << "  ERROR — " << result.error() << "\n";
        }
        std::cout << "\n";
    }

    // ========================================================================
    // PART 5: Practical worksheet function pattern
    // ========================================================================

    std::cout << "PART 5: Realistic worksheet function pattern\n";
    std::cout << "============================================\n\n";

    // Simulates a worksheet function that computes enthalpy for a column of
    // (pressure, temperature, quality) triples, where each column is read from
    // a worksheet range as a vector<double>.

    auto compute_enthalpy_row = [](double p, double t, double x) -> double {
        return p * 0.1 + t * 2.0 + x * 500.0;   // simplified
    };

    std::cout << "5. Validate all three input columns, then compute per-row:\n";
    {
        std::vector<double> raw_p = {1.0, 5.0, 10.0};
        std::vector<double> raw_t = {100.0, 200.0, 300.0};
        std::vector<double> raw_x = {0.0, 0.5, 1.0};

        auto valid_p = raw_p | fxt::traverse(validate_pressure);
        auto valid_t = raw_t | fxt::traverse(validate_temperature);
        auto valid_x = raw_x | fxt::traverse(validate_quality);

        if (valid_p && valid_t && valid_x) {
            std::cout << "  All inputs valid. Computing:\n";
            for (std::size_t i = 0; i < raw_p.size(); ++i) {
                double h = compute_enthalpy_row((*valid_p)[i], (*valid_t)[i], (*valid_x)[i]);
                std::cout << std::format("    row {}: h = {:.2f}\n", i, h);
            }
        } else {
            if (!valid_p) std::cout << "  Pressure error: " << valid_p.error() << "\n";
            if (!valid_t) std::cout << "  Temperature error: " << valid_t.error() << "\n";
            if (!valid_x) std::cout << "  Quality error: " << valid_x.error() << "\n";
        }
        std::cout << "\n";
    }

    // Same scenario but with a bad quality value
    std::cout << "5b. Same scenario with an invalid quality value:\n";
    {
        std::vector<double> raw_p = {1.0, 5.0, 10.0};
        std::vector<double> raw_t = {100.0, 200.0, 300.0};
        std::vector<double> raw_x = {0.0, 1.5, 1.0};     // second quality is out of range

        auto valid_p = raw_p | fxt::traverse(validate_pressure);
        auto valid_t = raw_t | fxt::traverse(validate_temperature);
        auto valid_x = raw_x | fxt::traverse(validate_quality);

        if (valid_p && valid_t && valid_x) {
            std::cout << "  (would compute...)\n";
        } else {
            if (!valid_p) std::cout << "  Pressure error: " << valid_p.error() << "\n";
            if (!valid_t) std::cout << "  Temperature error: " << valid_t.error() << "\n";
            if (!valid_x) std::cout << "  Quality error: " << valid_x.error() << "\n";
        }
        std::cout << "\n";
    }

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
