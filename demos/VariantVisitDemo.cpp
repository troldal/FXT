// Demo program showing how to use fxt::visit and fxt::mvisit with variants
// Both with and without the pipe operator

#include <fxt.hpp>
#include <iostream>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

int main()
{
    print_separator("FXT Variant Visit Demo");

    // Create variants with different types
    fxt::variant<int, double, std::string> v1{42};
    fxt::variant<int, double, std::string> v2{3.14};
    fxt::variant<int, double, std::string> v3{"hello"};

    std::cout << "Created three variants:\n";
    std::cout << "  v1 holds an int (42)\n";
    std::cout << "  v2 holds a double (3.14)\n";
    std::cout << "  v3 holds a string (\"hello\")\n";

    // Define a visitor that converts to string
    auto to_string_visitor = [](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else {
            return std::to_string(val);
        }
    };

    print_separator("1. Direct visit() calls");

    std::cout << "Using fxt::visit(visitor, variant):\n\n";
    std::cout << "  fxt::visit(to_string, v1): " << fxt::visit(to_string_visitor, v1) << "\n";
    std::cout << "  fxt::visit(to_string, v2): " << fxt::visit(to_string_visitor, v2) << "\n";
    std::cout << "  fxt::visit(to_string, v3): " << fxt::visit(to_string_visitor, v3) << "\n";

    print_separator("2. Pipe operator with visit()");

    std::cout << "Using variant | fxt::visit(visitor):\n\n";
    std::cout << "  v1 | visit: " << (v1 | fxt::visit(to_string_visitor)) << "\n";
    std::cout << "  v2 | visit: " << (v2 | fxt::visit(to_string_visitor)) << "\n";
    std::cout << "  v3 | visit: " << (v3 | fxt::visit(to_string_visitor)) << "\n";

    print_separator("3. mvisit() with optional<variant> - Direct calls");

    fxt::optional<fxt::variant<int, double, std::string>> opt1{v1};
    fxt::optional<fxt::variant<int, double, std::string>> opt2{v2};
    fxt::optional<fxt::variant<int, double, std::string>> opt_empty{};

    std::cout << "Created three optionals:\n";
    std::cout << "  opt1 contains v1 (int: 42)\n";
    std::cout << "  opt2 contains v2 (double: 3.14)\n";
    std::cout << "  opt_empty is empty\n\n";

    std::cout << "Using fxt::mvisit(visitor, optional):\n\n";

    auto opt_result1 = fxt::mvisit(to_string_visitor, opt1);
    auto opt_result2 = fxt::mvisit(to_string_visitor, opt2);
    auto opt_result_empty = fxt::mvisit(to_string_visitor, opt_empty);

    std::cout << "  fxt::mvisit(to_string, opt1):   "
              << (opt_result1 ? *opt_result1 : "empty") << "\n";
    std::cout << "  fxt::mvisit(to_string, opt2):   "
              << (opt_result2 ? *opt_result2 : "empty") << "\n";
    std::cout << "  fxt::mvisit(to_string, opt_empty): "
              << (opt_result_empty ? *opt_result_empty : "empty") << "\n";

    print_separator("4. Pipe operator with mvisit() on optional");

    std::cout << "Using optional | fxt::mvisit(visitor):\n\n";

    auto pipe_opt1 = opt1 | fxt::mvisit(to_string_visitor);
    auto pipe_opt2 = opt2 | fxt::mvisit(to_string_visitor);
    auto pipe_opt_empty = opt_empty | fxt::mvisit(to_string_visitor);

    std::cout << "  opt1      | mvisit: " << (pipe_opt1 ? *pipe_opt1 : "empty") << "\n";
    std::cout << "  opt2      | mvisit: " << (pipe_opt2 ? *pipe_opt2 : "empty") << "\n";
    std::cout << "  opt_empty | mvisit: " << (pipe_opt_empty ? *pipe_opt_empty : "empty") << "\n";

    print_separator("5. mvisit() with expected<variant, E> - Direct calls");

    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp1{v1};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp2{v3};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_err{
        fxt::unexpected("error occurred")
    };

    std::cout << "Created three expected values:\n";
    std::cout << "  exp1 contains v1 (int: 42)\n";
    std::cout << "  exp2 contains v3 (string: \"hello\")\n";
    std::cout << "  exp_err contains an error\n\n";

    std::cout << "Using fxt::mvisit(visitor, expected):\n\n";

    auto exp_result1 = fxt::mvisit(to_string_visitor, exp1);
    auto exp_result2 = fxt::mvisit(to_string_visitor, exp2);
    auto exp_result_err = fxt::mvisit(to_string_visitor, exp_err);

    std::cout << "  fxt::mvisit(to_string, exp1):     "
              << (exp_result1 ? *exp_result1 : "error: " + exp_result1.error()) << "\n";
    std::cout << "  fxt::mvisit(to_string, exp2):     "
              << (exp_result2 ? *exp_result2 : "error: " + exp_result2.error()) << "\n";
    std::cout << "  fxt::mvisit(to_string, exp_err):  "
              << (exp_result_err ? *exp_result_err : "error: " + exp_result_err.error()) << "\n";

    print_separator("6. Pipe operator with mvisit() on expected");

    std::cout << "Using expected | fxt::mvisit(visitor):\n\n";

    auto pipe_exp1 = exp1 | fxt::mvisit(to_string_visitor);
    auto pipe_exp2 = exp2 | fxt::mvisit(to_string_visitor);
    auto pipe_exp_err = exp_err | fxt::mvisit(to_string_visitor);

    std::cout << "  exp1    | mvisit: "
              << (pipe_exp1 ? *pipe_exp1 : "error: " + pipe_exp1.error()) << "\n";
    std::cout << "  exp2    | mvisit: "
              << (pipe_exp2 ? *pipe_exp2 : "error: " + pipe_exp2.error()) << "\n";
    std::cout << "  exp_err | mvisit: "
              << (pipe_exp_err ? *pipe_exp_err : "error: " + pipe_exp_err.error()) << "\n";

    print_separator("7. Advanced: Type-specific visitation");

    // Define a visitor that handles each type differently
    auto type_specific_visitor = [](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            return "Integer: " + std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return "Double: " + std::to_string(val);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return "String: " + val;
        }
    };

    std::cout << "Using a type-specific visitor:\n\n";
    std::cout << "  v1 | visit: " << (v1 | fxt::visit(type_specific_visitor)) << "\n";
    std::cout << "  v2 | visit: " << (v2 | fxt::visit(type_specific_visitor)) << "\n";
    std::cout << "  v3 | visit: " << (v3 | fxt::visit(type_specific_visitor)) << "\n";

    print_separator("8. Chaining mvisit() in a pipeline");

    auto double_visitor = [](auto&& val) -> double {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_arithmetic_v<T>) {
            return static_cast<double>(val);
        } else {
            return static_cast<double>(val.length());
        }
    };

    std::cout << "Example: Convert variant to double, then multiply by 2\n\n";

    auto pipeline_result1 = opt1
                          | fxt::mvisit(double_visitor)
                          | fxt::transform([](double d) { return d * 2; });

    auto pipeline_result2 = opt2
                          | fxt::mvisit(double_visitor)
                          | fxt::transform([](double d) { return d * 2; });

    std::cout << "  opt1 (int: 42)     | mvisit(to_double) | transform(*2): "
              << (pipeline_result1 ? std::to_string(*pipeline_result1) : "empty") << "\n";
    std::cout << "  opt2 (double: 3.14) | mvisit(to_double) | transform(*2): "
              << (pipeline_result2 ? std::to_string(*pipeline_result2) : "empty") << "\n";

    print_separator("9. Combining visit with match-like functionality");

    // Create a visitor using overloaded lambdas pattern
    auto overloaded_visitor = [](auto&& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) {
            return val % 2 == 0 ? "even integer" : "odd integer";
        } else if constexpr (std::is_same_v<T, double>) {
            return val > 0 ? "positive double" : "negative double";
        } else {
            return val.empty() ? "empty string" : "non-empty string";
        }
    };

    std::cout << "Using pattern-matching style visitor:\n\n";
    std::cout << "  v1 (42):      " << fxt::visit(overloaded_visitor, v1) << "\n";
    std::cout << "  v2 (3.14):    " << fxt::visit(overloaded_visitor, v2) << "\n";
    std::cout << "  v3 (\"hello\"): " << fxt::visit(overloaded_visitor, v3) << "\n";

    fxt::variant<int, double, std::string> v_odd{43};
    fxt::variant<int, double, std::string> v_neg{-2.5};
    fxt::variant<int, double, std::string> v_empty{std::string{}};

    std::cout << "  v_odd (43):   " << (v_odd | fxt::visit(overloaded_visitor)) << "\n";
    std::cout << "  v_neg (-2.5): " << (v_neg | fxt::visit(overloaded_visitor)) << "\n";
    std::cout << "  v_empty (\"\"):  " << (v_empty | fxt::visit(overloaded_visitor)) << "\n";

    print_separator("Demo Complete");

    return 0;
}

