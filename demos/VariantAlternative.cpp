// Demo program showing how to use fxt::is_variant_alternative / fxt::is_variant_alternative_v
// and fxt::variant_alternative_count / fxt::variant_alternative_count_v

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <variant>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

// A function constrained on is_variant_alternative_v — the trait's primary use
// case: gating a template so it only accepts variants that could hold a required type.
template<typename Variant>
    requires fxt::is_variant_alternative_v<std::string, Variant>
void requires_string_alternative(const Variant&) {
    std::cout << "  Accepted: this variant type can hold a std::string.\n";
}

int main() {
    print_separator("fxt::is_variant_alternative with fxt::variant");

    using V = fxt::variant<int, double, std::string>;
    std::cout << std::boolalpha;
    std::cout << "fxt::is_variant_alternative_v<int, V>:         " << fxt::is_variant_alternative_v<int, V> << "\n";
    std::cout << "fxt::is_variant_alternative_v<double, V>:      " << fxt::is_variant_alternative_v<double, V> << "\n";
    std::cout << "fxt::is_variant_alternative_v<std::string, V>: " << fxt::is_variant_alternative_v<std::string, V> << "\n";
    std::cout << "fxt::is_variant_alternative_v<float, V>:       " << fxt::is_variant_alternative_v<float, V> << "  (not an alternative)\n";

    print_separator("Works directly on std::variant, not just fxt::variant");

    using SV = std::variant<int, double>;
    std::cout << "fxt::is_variant_alternative_v<int, std::variant<int,double>>:  " << fxt::is_variant_alternative_v<int, SV> << "\n";
    std::cout << "fxt::is_variant_alternative_v<char, std::variant<int,double>>: " << fxt::is_variant_alternative_v<char, SV> << "  (not an alternative)\n";

    print_separator("fxt::variant_alternative_count — counting duplicate alternatives");

    // std::variant permits the same type to appear more than once; it is only
    // distinguished by index, not by std::get<T>/std::holds_alternative<T>.
    using D = fxt::variant<int, int, double, int>;
    std::cout << "Variant: fxt::variant<int, int, double, int>\n\n";
    std::cout << "fxt::variant_alternative_count_v<int, D>:    " << fxt::variant_alternative_count_v<int, D> << "  (appears 3 times)\n";
    std::cout << "fxt::variant_alternative_count_v<double, D>: " << fxt::variant_alternative_count_v<double, D> << "  (appears once)\n";
    std::cout << "fxt::variant_alternative_count_v<char, D>:   " << fxt::variant_alternative_count_v<char, D> << "  (absent)\n";
    std::cout << "\nfxt::is_variant_alternative_v<int, D> is simply (count > 0): "
              << fxt::is_variant_alternative_v<int, D> << "\n";

    print_separator("Type-level question, independent of the currently held value");

    V v{42};
    std::cout << "V v{42}; — v currently holds an int.\n\n";
    std::cout << "fxt::is_variant_alternative_v<std::string, V> (can V's TYPE hold a string?): "
              << fxt::is_variant_alternative_v<std::string, V> << "\n";
    std::cout << "fxt::holds_alternative<std::string>(v) (does v hold one RIGHT NOW?):        "
              << fxt::holds_alternative<std::string>(v) << "\n";
    std::cout << "\n  is_variant_alternative is a compile-time property of the TYPE.\n"
                 "  holds_alternative is a runtime property of a specific INSTANCE.\n"
                 "  This is why the trait is not named variant_contains or similar —\n"
                 "  that phrasing invites confusion with holds_alternative's semantics.\n";

    print_separator("Using is_variant_alternative as a requires-clause constraint");

    fxt::variant<int, std::string> with_string{"hello"};
    std::cout << "Calling requires_string_alternative() on fxt::variant<int, std::string>:\n";
    requires_string_alternative(with_string);

    std::cout << "\n(fxt::variant<int, double> would fail to compile if passed —\n"
                 " it does not satisfy fxt::is_variant_alternative_v<std::string, Variant>.)\n";

    print_separator("Demo Complete");
    return 0;
}
