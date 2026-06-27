// Demo program showing how to use fxt::holds_alternative and fxt::mholds_alternative
// Both with and without the pipe operator

#include <fxt.hpp>
#include <iostream>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

int main() {
    using Var = fxt::variant<int, double, std::string>;

    print_separator("Basic fxt::holds_alternative()");

    Var v_int{42};
    Var v_double{3.14};
    Var v_string{std::string{"Hello, FXT!"}};

    std::cout << "Created three variants:\n";
    std::cout << "  v_int    holds int    (42)\n";
    std::cout << "  v_double holds double (3.14)\n";
    std::cout << "  v_string holds string (\"Hello, FXT!\")\n\n";

    // Direct call (via using std::holds_alternative)
    std::cout << "fxt::holds_alternative<int>(v_int):       " << std::boolalpha << fxt::holds_alternative<int>(v_int) << "\n";
    std::cout << "fxt::holds_alternative<double>(v_int):    " << fxt::holds_alternative<double>(v_int) << "\n";
    std::cout << "fxt::holds_alternative<double>(v_double): " << fxt::holds_alternative<double>(v_double) << "\n";
    std::cout << "fxt::holds_alternative<string>(v_string): " << fxt::holds_alternative<std::string>(v_string) << "\n";

    print_separator("holds_alternative() with pipe operator");

    std::cout << "v_int    | fxt::holds_alternative<int>():    " << (v_int    | fxt::holds_alternative<int>()) << "\n";
    std::cout << "v_int    | fxt::holds_alternative<double>(): " << (v_int    | fxt::holds_alternative<double>()) << "\n";
    std::cout << "v_double | fxt::holds_alternative<double>(): " << (v_double | fxt::holds_alternative<double>()) << "\n";
    std::cout << "v_string | fxt::holds_alternative<string>(): " << (v_string | fxt::holds_alternative<std::string>()) << "\n";

    print_separator("mholds_alternative() with fxt::optional<variant>");

    fxt::optional<Var> opt_int{v_int};
    fxt::optional<Var> opt_double{v_double};
    fxt::optional<Var> opt_empty{};

    std::cout << "Created three optionals:\n";
    std::cout << "  opt_int    contains v_int\n";
    std::cout << "  opt_double contains v_double\n";
    std::cout << "  opt_empty  is empty\n\n";

    // Direct call
    std::cout << "fxt::mholds_alternative<int>(opt_int):    " << fxt::mholds_alternative<int>(opt_int) << "\n";
    std::cout << "fxt::mholds_alternative<double>(opt_int): " << fxt::mholds_alternative<double>(opt_int) << "\n";
    std::cout << "fxt::mholds_alternative<int>(opt_empty):  " << fxt::mholds_alternative<int>(opt_empty) << "\n";

    std::cout << "\nUsing pipe operator:\n";
    std::cout << "opt_int    | fxt::mholds_alternative<int>():    " << (opt_int    | fxt::mholds_alternative<int>()) << "\n";
    std::cout << "opt_double | fxt::mholds_alternative<double>(): " << (opt_double | fxt::mholds_alternative<double>()) << "\n";
    std::cout << "opt_empty  | fxt::mholds_alternative<int>():    " << (opt_empty  | fxt::mholds_alternative<int>()) << "\n";

    print_separator("mholds_alternative() with fxt::expected<variant, E>");

    fxt::expected<Var, std::string> exp_int{v_int};
    fxt::expected<Var, std::string> exp_string{v_string};
    fxt::expected<Var, std::string> exp_error{fxt::unexpected(std::string{"Something went wrong!"})};

    std::cout << "Created three expected values:\n";
    std::cout << "  exp_int    contains v_int\n";
    std::cout << "  exp_string contains v_string\n";
    std::cout << "  exp_error  contains an error\n\n";

    // Direct call
    std::cout << "fxt::mholds_alternative<int>(exp_int):    " << fxt::mholds_alternative<int>(exp_int) << "\n";
    std::cout << "fxt::mholds_alternative<string>(exp_int): " << fxt::mholds_alternative<std::string>(exp_int) << "\n";
    std::cout << "fxt::mholds_alternative<int>(exp_error):  " << fxt::mholds_alternative<int>(exp_error) << "\n";

    std::cout << "\nUsing pipe operator:\n";
    std::cout << "exp_int    | fxt::mholds_alternative<int>():    " << (exp_int    | fxt::mholds_alternative<int>()) << "\n";
    std::cout << "exp_string | fxt::mholds_alternative<string>(): " << (exp_string | fxt::mholds_alternative<std::string>()) << "\n";
    std::cout << "exp_error  | fxt::mholds_alternative<int>():    " << (exp_error  | fxt::mholds_alternative<int>()) << "\n";

    print_separator("Pipeline chaining example");

    // Combine mwhen (side-effect) with mholds_alternative (predicate) in a pipeline
    std::cout << "Checking and logging whether opt_int holds int:\n";
    bool seen_int = false;
    bool holds = opt_int
               | fxt::mwhen<int>([&](int x) {
                     std::cout << "  mwhen<int> fired: value = " << x << "\n";
                     seen_int = true;
                 })
               | fxt::mholds_alternative<int>();

    std::cout << "Pipeline result (holds int): " << holds << "\n";
    std::cout << "Side-effect was triggered:   " << seen_int << "\n";

    print_separator("Demo Complete");
    return 0;
}
