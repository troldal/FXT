// Demo program showing how to use fxt::tuple_contains / fxt::tuple_contains_v
// and fxt::tuple_type_count / fxt::tuple_type_count_v

#include <fxt.hpp>
#include <array>
#include <iostream>
#include <string>
#include <utility>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

// A function constrained on tuple_contains_v — the trait's primary use case:
// gating a template so it only accepts tuples that carry a required type.
template<typename Tuple>
    requires fxt::tuple_contains_v<std::string, Tuple>
void requires_a_string(const Tuple&) {
    std::cout << "  Accepted: tuple contains a std::string.\n";
}

int main() {
    print_separator("fxt::tuple_contains with fxt::tuple");

    using T = fxt::tuple<int, double, std::string>;
    std::cout << std::boolalpha;
    std::cout << "fxt::tuple_contains_v<int, T>:         " << fxt::tuple_contains_v<int, T> << "\n";
    std::cout << "fxt::tuple_contains_v<double, T>:      " << fxt::tuple_contains_v<double, T> << "\n";
    std::cout << "fxt::tuple_contains_v<std::string, T>: " << fxt::tuple_contains_v<std::string, T> << "\n";
    std::cout << "fxt::tuple_contains_v<float, T>:       " << fxt::tuple_contains_v<float, T> << "  (not present)\n";

    print_separator("fxt::tuple_contains with fxt::flat_tuple");

    using FT = fxt::flat_tuple<int, double>;
    std::cout << "fxt::tuple_contains_v<int, FT>:    " << fxt::tuple_contains_v<int, FT> << "\n";
    std::cout << "fxt::tuple_contains_v<double, FT>: " << fxt::tuple_contains_v<double, FT> << "\n";
    std::cout << "fxt::tuple_contains_v<char, FT>:   " << fxt::tuple_contains_v<char, FT> << "  (not present)\n";

    print_separator("fxt::tuple_contains with any fxt::tuple_like type");

    using P = std::pair<int, double>;
    using A = std::array<int, 3>;
    std::cout << "fxt::tuple_contains_v<int, std::pair<int,double>>:    " << fxt::tuple_contains_v<int, P> << "\n";
    std::cout << "fxt::tuple_contains_v<double, std::pair<int,double>>: " << fxt::tuple_contains_v<double, P> << "\n";
    std::cout << "fxt::tuple_contains_v<int, std::array<int,3>>:        " << fxt::tuple_contains_v<int, A> << "\n";
    std::cout << "fxt::tuple_contains_v<double, std::array<int,3>>:     " << fxt::tuple_contains_v<double, A> << "  (not present)\n";

    print_separator("fxt::tuple_type_count — counting duplicates");

    using D = fxt::tuple<int, int, double, int>;
    std::cout << "Tuple: fxt::tuple<int, int, double, int>\n\n";
    std::cout << "fxt::tuple_type_count_v<int, D>:    " << fxt::tuple_type_count_v<int, D> << "  (appears 3 times)\n";
    std::cout << "fxt::tuple_type_count_v<double, D>: " << fxt::tuple_type_count_v<double, D> << "  (appears once)\n";
    std::cout << "fxt::tuple_type_count_v<char, D>:   " << fxt::tuple_type_count_v<char, D> << "  (absent)\n";
    std::cout << "\nfxt::tuple_contains_v<int, D> is simply (count > 0): "
              << fxt::tuple_contains_v<int, D> << "\n";

    print_separator("Using tuple_contains as a requires-clause constraint");

    fxt::tuple<int, std::string> with_string{42, "hello"};
    std::cout << "Calling requires_a_string() on fxt::tuple<int, std::string>:\n";
    requires_a_string(with_string);

    std::cout << "\n(fxt::tuple<int, double> would fail to compile if passed —\n"
                 " it does not satisfy fxt::tuple_contains_v<std::string, Tuple>.)\n";

    print_separator("Demo Complete");
    return 0;
}
