//
// Created by kenne on 07/06/2026.
//

// Demonstrates fxt::sequence and fxt::traverse for fxt::tuple —
// the heterogeneous, compile-time variant of the applicative operations.
//
// The primary use case is validating a fixed set of named arguments to an
// Excel-facing thermodynamic function, where each argument has a distinct type
// and the result type is known at compile time.

#include <fxt.hpp>
#include <format>
#include <iostream>
#include <string>

// ============================================================================
// Domain: XLThermo Excel function arguments
// ============================================================================

using XLError = std::string;

fxt::expected<double, XLError> validate_pressure(double p)
{
    if (p <= 0.0)  return fxt::unexpected<XLError>(std::format("pressure {} bar is not positive", p));
    if (p > 220.0) return fxt::unexpected<XLError>(std::format("pressure {} bar exceeds critical point", p));
    return p;
}

fxt::expected<double, XLError> validate_temperature(double t)
{
    if (t < 0.01)  return fxt::unexpected<XLError>(std::format("temperature {} °C is below freezing", t));
    if (t > 374.0) return fxt::unexpected<XLError>(std::format("temperature {} °C exceeds critical point", t));
    return t;
}

fxt::expected<double, XLError> validate_quality(double x)
{
    if (x < 0.0 || x > 1.0)
        return fxt::unexpected<XLError>(std::format("steam quality {} is outside [0, 1]", x));
    return x;
}

// Pure computation — receives already-validated values
double compute_enthalpy(double p, double t, double x)
{
    return p * 0.1 + t * 2.0 + x * 500.0;   // simplified
}

// ============================================================================
// Helper
// ============================================================================

template<typename T>
void print(const fxt::expected<T, XLError>& r, const std::string& label)
{
    std::cout << "  " << label << ": ";
    if (r.has_value()) {
        if constexpr (std::is_floating_point_v<T>) {
            std::cout << std::format("Ok({:.3f})", *r);
        } else {
            std::cout << "Ok";
        }
    } else {
        std::cout << "Error: " << r.error();
    }
    std::cout << "\n";
}

int main()
{
    std::cout << "=== fxt::sequence and fxt::traverse for fxt::tuple Demo ===\n\n";

    // ========================================================================
    // PART 1: sequence() for named heterogeneous arguments
    //
    // Each argument is validated independently. sequence() collects the
    // results into expected<tuple<...>, E>, enabling the computation to
    // receive all arguments at once via std::apply.
    // ========================================================================

    std::cout << "PART 1: fxt::sequence — named heterogeneous arguments\n";
    std::cout << "======================================================\n\n";

    // 1a. All three arguments valid
    std::cout << "1a. All three arguments valid:\n";
    {
        double raw_p = 5.0, raw_t = 200.0, raw_q = 0.8;

        auto args = fxt::make_tuple(
            validate_pressure(raw_p),
            validate_temperature(raw_t),
            validate_quality(raw_q)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto validated) -> fxt::expected<double, XLError> {
                return compute_enthalpy(
                    fxt::get<0>(validated),
                    fxt::get<1>(validated),
                    fxt::get<2>(validated)
                );
              });

        print(result, "enthalpy");
        std::cout << "\n";
    }

    // 1b. Pressure out of range — short-circuits at first argument
    std::cout << "1b. Pressure out of range (short-circuits at first argument):\n";
    {
        double raw_p = -5.0, raw_t = 200.0, raw_q = 0.8;

        auto args = fxt::make_tuple(
            validate_pressure(raw_p),
            validate_temperature(raw_t),
            validate_quality(raw_q)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto validated) -> fxt::expected<double, XLError> {
                return compute_enthalpy(
                    std::get<0>(validated),
                    std::get<1>(validated),
                    std::get<2>(validated)
                );
              });

        print(result, "enthalpy");
        std::cout << "\n";
    }

    // 1c. Temperature invalid — second argument
    std::cout << "1c. Temperature above critical point (second argument):\n";
    {
        auto args = fxt::make_tuple(
            validate_pressure(5.0),
            validate_temperature(400.0),  // invalid: > 374 °C
            validate_quality(0.8)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto v) -> fxt::expected<double, XLError> {
                return compute_enthalpy(std::get<0>(v), std::get<1>(v), std::get<2>(v));
              });

        print(result, "enthalpy");
        std::cout << "\n";
    }

    // 1d. Quality invalid — third argument
    std::cout << "1d. Quality out of [0,1] (third argument):\n";
    {
        auto args = fxt::make_tuple(
            validate_pressure(5.0),
            validate_temperature(200.0),
            validate_quality(1.5)          // invalid
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto v) -> fxt::expected<double, XLError> {
                return compute_enthalpy(std::get<0>(v), std::get<1>(v), std::get<2>(v));
              });

        print(result, "enthalpy");
        std::cout << "\n";
    }

    // ========================================================================
    // PART 2: Using std::apply for clean argument unpacking
    //
    // Since sequence() produces expected<tuple<T0,T1,...>,E>, and the
    // computation takes the same T0, T1, ... as individual arguments,
    // std::apply connects them without manual get<> calls.
    // ========================================================================

    std::cout << "PART 2: sequence + std::apply for argument unpacking\n";
    std::cout << "=======================================================\n\n";

    std::cout << "2a. Valid inputs via std::apply:\n";
    {
        auto args = fxt::make_tuple(
            validate_pressure(10.0),
            validate_temperature(300.0),
            validate_quality(0.5)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::transform([](auto validated) {
                return fxt::apply(compute_enthalpy, validated);
              });

        print(result, "enthalpy");
        std::cout << "\n";
    }

    // ========================================================================
    // PART 3: traverse() for homogeneous arguments in a tuple
    //
    // When the same validator applies to all elements (e.g., all are doubles),
    // traverse() is more concise than building a tuple of expected values
    // manually.
    // ========================================================================

    std::cout << "PART 3: fxt::traverse — same validator over a tuple of doubles\n";
    std::cout << "================================================================\n\n";

    // 3a. Three pressure readings, all valid
    std::cout << "3a. Three pressure readings, all valid:\n";
    {
        auto pressures = fxt::make_tuple(10.0, 20.0, 30.0);
        auto result = pressures | fxt::traverse(validate_pressure);
        // result : expected<tuple<double, double, double>, XLError>

        if (result.has_value()) {
            std::cout << std::format("    validated: ({:.1f}, {:.1f}, {:.1f})\n",
                std::get<0>(*result), std::get<1>(*result), std::get<2>(*result));
        } else {
            std::cout << "    Error: " << result.error() << "\n";
        }
        std::cout << "\n";
    }

    // 3b. One invalid pressure — short-circuits
    std::cout << "3b. One invalid pressure in the tuple:\n";
    {
        auto pressures = fxt::make_tuple(10.0, -5.0, 30.0);  // second is invalid
        auto result = pressures | fxt::traverse(validate_pressure);

        if (result.has_value()) {
            std::cout << "    (unexpected success)\n";
        } else {
            std::cout << "    Error: " << result.error() << "\n";
        }
        std::cout << "\n";
    }

    // 3c. Heterogeneous tuple — polymorphic lambda validates each type independently
    std::cout << "3c. Heterogeneous tuple with polymorphic validate_positive lambda:\n";
    {
        auto validate_pos = [](auto v) -> fxt::expected<std::decay_t<decltype(v)>, XLError> {
            if (v <= 0) return fxt::unexpected<XLError>("non-positive");
            return v;
        };

        auto mixed = fxt::make_tuple(5, 3.14, 7);   // int, double, int
        auto result = mixed | fxt::traverse(validate_pos);
        // result : expected<tuple<int, double, int>, XLError>

        if (result.has_value()) {
            std::cout << std::format("    validated: ({}, {:.2f}, {})\n",
                std::get<0>(*result), std::get<1>(*result), std::get<2>(*result));
        } else {
            std::cout << "    Error: " << result.error() << "\n";
        }
        std::cout << "\n";
    }

    // 3d. Mixed numeric + non-numeric tuple — fxt::overload dispatches per element type
    std::cout << "3d. Heterogeneous tuple (double, string, int) with fxt::overload validator:\n";
    {
        // Each overload handles its own type and returns the same error type E,
        // which is the only constraint traverse imposes on a heterogeneous tuple.
        auto validator = fxt::overload{
            [](double d)             -> fxt::expected<double,      XLError> {
                if (d <= 0.0) return fxt::unexpected<XLError>(std::format("value {} must be positive", d));
                return d;
            },
            [](const std::string& s) -> fxt::expected<std::string, XLError> {
                if (s.empty()) return fxt::unexpected<XLError>("label must not be empty");
                return s;
            },
            [](int i)                -> fxt::expected<int,         XLError> {
                if (i < 0) return fxt::unexpected<XLError>(std::format("count {} must be non-negative", i));
                return i;
            }
        };

        // All elements valid
        {
            auto mixed  = fxt::make_tuple(3.14, std::string("steam"), 7);
            auto result = mixed | fxt::traverse(validator);
            // result : expected<tuple<double, string, int>, XLError>

            if (result.has_value()) {
                std::cout << std::format("    valid   -> ({:.2f}, \"{}\", {})\n",
                    std::get<0>(*result), std::get<1>(*result), std::get<2>(*result));
            } else {
                std::cout << "    Error: " << result.error() << "\n";
            }
        }

        // Second element (string) invalid — short-circuits there
        {
            auto mixed  = fxt::make_tuple(3.14, std::string(""), 7);
            auto result = mixed | fxt::traverse(validator);

            if (result.has_value()) {
                std::cout << "    (unexpected success)\n";
            } else {
                std::cout << "    invalid -> Error: " << result.error() << "\n";
            }
        }

        std::cout << "\n";
    }

    // ========================================================================
    // PART 4: Contrast — tuple sequence vs with() for the same pipeline
    //
    // Both patterns express "apply a pure function to validated arguments".
    // with() feeds arguments into a curried function one at a time; sequence()
    // collects all first and then passes the validated tuple.
    // ========================================================================

    std::cout << "PART 4: Contrast — sequence vs with() for same computation\n";
    std::cout << "=============================================================\n\n";

    double raw_p = 5.0, raw_t = 200.0, raw_q = 0.8;

    // Via sequence + std::apply
    {
        auto r1 = fxt::sequence(fxt::make_tuple(
                validate_pressure(raw_p),
                validate_temperature(raw_t),
                validate_quality(raw_q)))
            | fxt::transform([](auto v) {
                return std::apply(compute_enthalpy, v);
              });
        print(r1, "via sequence + std::apply");
    }

    // Via curry + with()
    {
        auto r2 = fxt::curry(compute_enthalpy)
            | fxt::with(validate_pressure(raw_p))
            | fxt::with(validate_temperature(raw_t))
            | fxt::with(validate_quality(raw_q));
        print(r2, "via curry + with()       ");
    }

    std::cout << "    (both produce the same result)\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
