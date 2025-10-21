#include <fxt.hpp>
#include <iostream>
#include <string>
#include <variant>

int main()
{
    // Create a variant with different types
    fxt::variant<int, double, std::string> v1{42};
    fxt::variant<int, double, std::string> v2{3.14};
    fxt::variant<int, double, std::string> v3{"hello"};

    // Direct call to index()
    std::cout << "Direct index calls:\n";
    std::cout << "v1 index: " << fxt::index(v1) << "\n";
    std::cout << "v2 index: " << fxt::index(v2) << "\n";
    std::cout << "v3 index: " << fxt::index(v3) << "\n\n";

    // Using pipe operator with index()
    std::cout << "Pipe operator with index():\n";
    auto idx1 = v1 | fxt::index();
    auto idx2 = v2 | fxt::index();
    auto idx3 = v3 | fxt::index();
    std::cout << "v1 | index(): " << idx1 << "\n";
    std::cout << "v2 | index(): " << idx2 << "\n";
    std::cout << "v3 | index(): " << idx3 << "\n\n";

    // Using mindex() with optional<variant>
    std::cout << "mindex() with optional<variant>:\n";
    fxt::optional<fxt::variant<int, double, std::string>> opt1{v1};
    fxt::optional<fxt::variant<int, double, std::string>> opt2{v2};
    fxt::optional<fxt::variant<int, double, std::string>> opt3{v3};
    fxt::optional<fxt::variant<int, double, std::string>> opt_empty{};

    // Direct call to mindex()
    auto opt_idx1 = fxt::mindex(opt1);
    auto opt_idx2 = fxt::mindex(opt2);
    auto opt_idx3 = fxt::mindex(opt3);
    auto opt_idx_empty = fxt::mindex(opt_empty);

    std::cout << "opt1 mindex: " << (opt_idx1 ? std::to_string(*opt_idx1) : "empty") << "\n";
    std::cout << "opt2 mindex: " << (opt_idx2 ? std::to_string(*opt_idx2) : "empty") << "\n";
    std::cout << "opt3 mindex: " << (opt_idx3 ? std::to_string(*opt_idx3) : "empty") << "\n";
    std::cout << "opt_empty mindex: " << (opt_idx_empty ? std::to_string(*opt_idx_empty) : "empty") << "\n\n";

    // Using pipe operator with mindex()
    std::cout << "Pipe operator with mindex():\n";
    auto result1 = opt1 | fxt::mindex();
    auto result2 = opt2 | fxt::mindex();
    auto result3 = opt_empty | fxt::mindex();

    std::cout << "opt1 | mindex(): " << (result1 ? std::to_string(*result1) : "empty") << "\n";
    std::cout << "opt2 | mindex(): " << (result2 ? std::to_string(*result2) : "empty") << "\n";
    std::cout << "opt_empty | mindex(): " << (result3 ? std::to_string(*result3) : "empty") << "\n\n";

    // Using mindex() with expected<variant, E>
    std::cout << "mindex() with expected<variant, E>:\n";
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp1{v1};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp2{v2};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_err{fxt::unexpected("error")};

    // Direct call to mindex()
    auto exp_idx1 = fxt::mindex(exp1);
    auto exp_idx2 = fxt::mindex(exp2);
    auto exp_idx_err = fxt::mindex(exp_err);

    std::cout << "exp1 mindex: " << (exp_idx1 ? std::to_string(*exp_idx1) : "error: " + exp_idx1.error()) << "\n";
    std::cout << "exp2 mindex: " << (exp_idx2 ? std::to_string(*exp_idx2) : "error: " + exp_idx2.error()) << "\n";
    std::cout << "exp_err mindex: " << (exp_idx_err ? std::to_string(*exp_idx_err) : "error: " + exp_idx_err.error()) << "\n\n";

    // Using pipe operator with mindex() on expected
    std::cout << "Pipe operator with mindex() on expected:\n";
    auto exp_result1 = exp1 | fxt::mindex();
    auto exp_result2 = exp_err | fxt::mindex();

    std::cout << "exp1 | mindex(): " << (exp_result1 ? std::to_string(*exp_result1) : "error: " + exp_result1.error()) << "\n";
    std::cout << "exp_err | mindex(): " << (exp_result2 ? std::to_string(*exp_result2) : "error: " + exp_result2.error()) << "\n\n";

    // Chaining mindex() in a pipeline
    std::cout << "Chaining mindex() in a pipeline:\n";
    auto pipeline_result = opt1
                         | fxt::mindex()
                         | fxt::transform([](std::size_t idx) { return idx * 10; });

    std::cout << "opt1 | mindex() | transform(*10): "
              << (pipeline_result ? std::to_string(*pipeline_result) : "empty") << "\n";

    return 0;
}