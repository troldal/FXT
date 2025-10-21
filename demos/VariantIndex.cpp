// Demo program showing how to use fxt::variant with fxt::index and fxt::mindex
// Both with and without the pipe operator

#include <fxt.hpp>
#include <iostream>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

int main() {
    print_separator("Basic fxt::variant with fxt::index()");

    // Create variants with different active alternatives
    fxt::variant<int, double, std::string> v_int{42};
    fxt::variant<int, double, std::string> v_double{3.14};
    fxt::variant<int, double, std::string> v_string{"Hello, FXT!"};

    std::cout << "Created three variants:\n";
    std::cout << "  v_int holds an int (42)\n";
    std::cout << "  v_double holds a double (3.14)\n";
    std::cout << "  v_string holds a string (\"Hello, FXT!\")\n\n";

    // Using index() directly (without pipe operator)
    print_separator("Using index() directly");

    std::cout << "fxt::index(v_int):    " << fxt::index(v_int) << "  (int is at index 0)\n";
    std::cout << "fxt::index(v_double): " << fxt::index(v_double) << "  (double is at index 1)\n";
    std::cout << "fxt::index(v_string): " << fxt::index(v_string) << "  (string is at index 2)\n";

    // Using index() with pipe operator
    print_separator("Using index() with pipe operator");

    auto idx_int = v_int | fxt::index();
    auto idx_double = v_double | fxt::index();
    auto idx_string = v_string | fxt::index();

    std::cout << "v_int    | fxt::index(): " << idx_int << "\n";
    std::cout << "v_double | fxt::index(): " << idx_double << "\n";
    std::cout << "v_string | fxt::index(): " << idx_string << "\n";

    // Using mindex() with fxt::optional<fxt::variant>
    print_separator("Using mindex() with fxt::optional<variant>");

    fxt::optional<fxt::variant<int, double, std::string>> opt_with_int{v_int};
    fxt::optional<fxt::variant<int, double, std::string>> opt_with_double{v_double};
    fxt::optional<fxt::variant<int, double, std::string>> opt_empty{};

    std::cout << "Created three optionals:\n";
    std::cout << "  opt_with_int contains v_int\n";
    std::cout << "  opt_with_double contains v_double\n";
    std::cout << "  opt_empty is empty\n\n";

    // Using mindex() directly (without pipe operator)
    std::cout << "Using mindex() directly:\n";

    auto opt_idx1 = fxt::mindex(opt_with_int);
    auto opt_idx2 = fxt::mindex(opt_with_double);
    auto opt_idx3 = fxt::mindex(opt_empty);

    std::cout << "fxt::mindex(opt_with_int):    "
              << (opt_idx1 ? std::to_string(*opt_idx1) : "empty") << "\n";
    std::cout << "fxt::mindex(opt_with_double): "
              << (opt_idx2 ? std::to_string(*opt_idx2) : "empty") << "\n";
    std::cout << "fxt::mindex(opt_empty):       "
              << (opt_idx3 ? std::to_string(*opt_idx3) : "empty") << "\n";

    // Using mindex() with pipe operator
    std::cout << "\nUsing mindex() with pipe operator:\n";

    auto result1 = opt_with_int | fxt::mindex();
    auto result2 = opt_with_double | fxt::mindex();
    auto result3 = opt_empty | fxt::mindex();

    std::cout << "opt_with_int    | fxt::mindex(): "
              << (result1 ? std::to_string(*result1) : "empty") << "\n";
    std::cout << "opt_with_double | fxt::mindex(): "
              << (result2 ? std::to_string(*result2) : "empty") << "\n";
    std::cout << "opt_empty       | fxt::mindex(): "
              << (result3 ? std::to_string(*result3) : "empty") << "\n";

    // Using mindex() with fxt::expected<fxt::variant, E>
    print_separator("Using mindex() with fxt::expected<variant, E>");

    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_with_int{v_int};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_with_string{v_string};
    fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_error{
        fxt::unexpected("Something went wrong!")
    };

    std::cout << "Created three expected values:\n";
    std::cout << "  exp_with_int contains v_int\n";
    std::cout << "  exp_with_string contains v_string\n";
    std::cout << "  exp_error contains an error\n\n";

    // Using mindex() directly (without pipe operator)
    std::cout << "Using mindex() directly:\n";

    auto exp_idx1 = fxt::mindex(exp_with_int);
    auto exp_idx2 = fxt::mindex(exp_with_string);
    auto exp_idx3 = fxt::mindex(exp_error);

    std::cout << "fxt::mindex(exp_with_int):    "
              << (exp_idx1 ? std::to_string(*exp_idx1) : "error: " + exp_idx1.error()) << "\n";
    std::cout << "fxt::mindex(exp_with_string): "
              << (exp_idx2 ? std::to_string(*exp_idx2) : "error: " + exp_idx2.error()) << "\n";
    std::cout << "fxt::mindex(exp_error):       "
              << (exp_idx3 ? std::to_string(*exp_idx3) : "error: " + exp_idx3.error()) << "\n";

    // Using mindex() with pipe operator
    std::cout << "\nUsing mindex() with pipe operator:\n";

    auto exp_result1 = exp_with_int | fxt::mindex();
    auto exp_result2 = exp_with_string | fxt::mindex();
    auto exp_result3 = exp_error | fxt::mindex();

    std::cout << "exp_with_int    | fxt::mindex(): "
              << (exp_result1 ? std::to_string(*exp_result1) : "error: " + exp_result1.error()) << "\n";
    std::cout << "exp_with_string | fxt::mindex(): "
              << (exp_result2 ? std::to_string(*exp_result2) : "error: " + exp_result2.error()) << "\n";
    std::cout << "exp_error       | fxt::mindex(): "
              << (exp_result3 ? std::to_string(*exp_result3) : "error: " + exp_result3.error()) << "\n";

    // Chaining operations in a pipeline
    print_separator("Chaining with pipe operator");

    std::cout << "Example: Get index from optional<variant> and multiply by 100\n\n";

    auto chained = opt_with_double
                 | fxt::mindex()
                 | fxt::transform([](std::size_t idx) {
                       return idx * 100;
                   });

    std::cout << "opt_with_double | mindex() | transform(*100): "
              << (chained ? std::to_string(*chained) : "empty")
              << "  (index 1 * 100 = 100)\n";

    // Another example with expected
    std::cout << "\nExample: Get index from expected<variant> and convert to string\n\n";

    auto chained_exp = exp_with_string
                     | fxt::mindex()
                     | fxt::transform([](std::size_t idx) {
                           return "Index is: " + std::to_string(idx);
                       });

    std::cout << "exp_with_string | mindex() | transform(to_string): "
              << (chained_exp ? *chained_exp : "error: " + chained_exp.error()) << "\n";

    print_separator("Demo Complete");

    return 0;
}

