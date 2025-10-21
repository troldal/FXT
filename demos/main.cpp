#include <fxt.hpp>
    #include <iostream>
    #include <string>

    int main()
    {
        std::cout << "=== FXT Variant Visit Demo ===\n\n";

        // Create variants with different types
        fxt::variant<int, double, std::string> v1{42};
        fxt::variant<int, double, std::string> v2{3.14};
        fxt::variant<int, double, std::string> v3{"hello"};

        // Define a visitor that converts to string
        auto to_string_visitor = [](auto&& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else {
                return std::to_string(val);
            }
        };

        // 1. Direct call to visit()
        std::cout << "1. Direct visit() calls:\n";
        std::cout << "   v1 (int): " << fxt::visit(to_string_visitor, v1) << "\n";
        std::cout << "   v2 (double): " << fxt::visit(to_string_visitor, v2) << "\n";
        std::cout << "   v3 (string): " << fxt::visit(to_string_visitor, v3) << "\n\n";

        // 2. Using pipe operator with visit()
        std::cout << "2. Pipe operator with visit():\n";
        std::cout << "   v1 | visit: " << (v1 | fxt::visit(to_string_visitor)) << "\n";
        std::cout << "   v2 | visit: " << (v2 | fxt::visit(to_string_visitor)) << "\n";
        std::cout << "   v3 | visit: " << (v3 | fxt::visit(to_string_visitor)) << "\n\n";

        // 3. Using mvisit() with optional<variant>
        std::cout << "3. mvisit() with optional<variant>:\n";
        fxt::optional<fxt::variant<int, double, std::string>> opt1{v1};
        fxt::optional<fxt::variant<int, double, std::string>> opt2{v2};
        fxt::optional<fxt::variant<int, double, std::string>> opt_empty{};

        // Direct call
        auto opt_result1 = fxt::mvisit(to_string_visitor, opt1);
        auto opt_result2 = fxt::mvisit(to_string_visitor, opt2);
        auto opt_result_empty = fxt::mvisit(to_string_visitor, opt_empty);

        std::cout << "   Direct: opt1 = " << (opt_result1 ? *opt_result1 : "empty") << "\n";
        std::cout << "   Direct: opt2 = " << (opt_result2 ? *opt_result2 : "empty") << "\n";
        std::cout << "   Direct: opt_empty = " << (opt_result_empty ? *opt_result_empty : "empty") << "\n\n";

        // 4. Using pipe operator with mvisit() on optional
        std::cout << "4. Pipe operator with mvisit() on optional:\n";
        auto pipe_opt1 = opt1 | fxt::mvisit(to_string_visitor);
        auto pipe_opt_empty = opt_empty | fxt::mvisit(to_string_visitor);

        std::cout << "   opt1 | mvisit = " << (pipe_opt1 ? *pipe_opt1 : "empty") << "\n";
        std::cout << "   opt_empty | mvisit = " << (pipe_opt_empty ? *pipe_opt_empty : "empty") << "\n\n";

        // 5. Using mvisit() with expected<variant, E>
        std::cout << "5. mvisit() with expected<variant, E>:\n";
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp1{v1};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp2{v3};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_err{fxt::unexpected("error occurred")};

        // Direct call
        auto exp_result1 = fxt::mvisit(to_string_visitor, exp1);
        auto exp_result2 = fxt::mvisit(to_string_visitor, exp2);
        auto exp_result_err = fxt::mvisit(to_string_visitor, exp_err);

        std::cout << "   Direct: exp1 = " << (exp_result1 ? *exp_result1 : "error: " + exp_result1.error()) << "\n";
        std::cout << "   Direct: exp2 = " << (exp_result2 ? *exp_result2 : "error: " + exp_result2.error()) << "\n";
        std::cout << "   Direct: exp_err = " << (exp_result_err ? *exp_result_err : "error: " + exp_result_err.error()) << "\n\n";

        // 6. Using pipe operator with mvisit() on expected
        std::cout << "6. Pipe operator with mvisit() on expected:\n";
        auto pipe_exp1 = exp1 | fxt::mvisit(to_string_visitor);
        auto pipe_exp_err = exp_err | fxt::mvisit(to_string_visitor);

        std::cout << "   exp1 | mvisit = " << (pipe_exp1 ? *pipe_exp1 : "error: " + pipe_exp1.error()) << "\n";
        std::cout << "   exp_err | mvisit = " << (pipe_exp_err ? *pipe_exp_err : "error: " + pipe_exp_err.error()) << "\n\n";

        // 7. Chaining mvisit() in a pipeline
        std::cout << "7. Chaining mvisit() in a pipeline:\n";
        auto double_visitor = [](auto&& val) -> double {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_arithmetic_v<T>) {
                return static_cast<double>(val);
            } else {
                return static_cast<double>(val.length());
            }
        };

        auto pipeline_result = opt1
                             | fxt::mvisit(double_visitor)
                             | fxt::transform([](double d) { return d * 2; });

        std::cout << "   opt1 | mvisit(to_double) | transform(*2) = "
                  << (pipeline_result ? std::to_string(*pipeline_result) : "empty") << "\n";

        return 0;
    }