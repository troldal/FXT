// Demo program showing implicit conversion between fxt::tuple and std::tuple

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <tuple>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

// A function that expects std::tuple
void process_std_tuple(const std::tuple<int, double, std::string>& t) {
    auto [a, b, c] = t;
    std::cout << "  Received std::tuple: (" << a << ", " << b << ", \"" << c << "\")\n";
}

// A function that expects std::tuple by value
std::tuple<int, double> compute_std_tuple(std::tuple<int, int> t) {
    auto [a, b] = t;
    return std::make_tuple(a + b, static_cast<double>(a) / b);
}

// A function that returns std::tuple
std::tuple<int, std::string> get_std_tuple() {
    return std::make_tuple(42, "hello");
}

int main()
{
    print_separator("FXT Tuple Conversion Demo");

    print_separator("1. Implicit Conversion to std::tuple");

    std::cout << "Creating fxt::tuple and passing to function expecting std::tuple:\n\n";

    fxt::tuple<int, double, std::string> fxt_t{10, 3.14, "world"};
    std::cout << "  fxt::tuple<int, double, std::string> fxt_t{10, 3.14, \"world\"};\n";

    // Implicit conversion when passing to function
    process_std_tuple(fxt_t);

    print_separator("2. Implicit Conversion from std::tuple");

    std::cout << "Creating std::tuple and assigning to fxt::tuple:\n\n";

    std::tuple<int, std::string> std_t = get_std_tuple();
    std::cout << "  std::tuple<int, std::string> std_t = get_std_tuple();\n";

    // Implicit conversion from std::tuple to fxt::tuple
    fxt::tuple<int, std::string> fxt_t2 = std_t;
    std::cout << "  fxt::tuple<int, std::string> fxt_t2 = std_t;\n";

    auto [x, y] = fxt_t2;
    std::cout << "  Content: (" << x << ", \"" << y << "\")\n";

    print_separator("3. Bidirectional Conversion");

    std::cout << "Converting back and forth:\n\n";

    fxt::tuple<int, int> fxt_nums{5, 2};
    std::cout << "  fxt::tuple<int, int> fxt_nums{5, 2};\n";

    // fxt::tuple -> std::tuple (implicit)
    std::tuple<int, double> result = compute_std_tuple(fxt_nums);
    std::cout << "  std::tuple<int, double> result = compute_std_tuple(fxt_nums);\n";

    auto [sum, ratio] = result;
    std::cout << "  Result: sum=" << sum << ", ratio=" << ratio << "\n";

    // std::tuple -> fxt::tuple (implicit)
    fxt::tuple<int, double> fxt_result = result;
    std::cout << "\n  fxt::tuple<int, double> fxt_result = result;\n";

    auto [s, r] = fxt_result;
    std::cout << "  Content: sum=" << s << ", ratio=" << r << "\n";

    print_separator("4. Using with FXT Operations");

    std::cout << "Converting and using with FXT pipeline operations:\n\n";

    std::tuple<int, int, int> std_triple{1, 2, 3};
    std::cout << "  std::tuple<int, int, int> std_triple{1, 2, 3};\n";

    // Convert to fxt::tuple and use with FXT operations
    fxt::tuple<int, int, int> fxt_triple = std_triple;
    std::cout << "  fxt::tuple<int, int, int> fxt_triple = std_triple;\n";

    // Use FXT tuple operations
    auto reversed = fxt_triple | fxt::tuple_reverse();
    std::cout << "\n  auto reversed = fxt_triple | fxt::tuple_reverse();\n";

    auto [r1, r2, r3] = reversed;
    std::cout << "  Reversed: (" << r1 << ", " << r2 << ", " << r3 << ")\n";

    // Convert back to std::tuple if needed
    std::tuple<int, int, int> std_reversed = reversed;
    std::cout << "\n  std::tuple<int, int, int> std_reversed = reversed;\n";
    std::cout << "  Seamless conversion!\n";

    print_separator("5. Assignment and Conversion");

    std::cout << "Testing different assignment scenarios:\n\n";

    fxt::tuple<int, double> ft1{10, 2.5};
    std::tuple<int, double> st1;

    std::cout << "  fxt::tuple<int, double> ft1{10, 2.5};\n";
    std::cout << "  std::tuple<int, double> st1;\n";

    // Assignment from fxt::tuple to std::tuple
    st1 = ft1;
    std::cout << "\n  st1 = ft1;  // fxt::tuple -> std::tuple\n";
    std::cout << "  st1 content: (" << std::get<0>(st1) << ", " << std::get<1>(st1) << ")\n";

    std::tuple<int, double> st2{20, 5.0};
    fxt::tuple<int, double> ft2;

    std::cout << "\n  std::tuple<int, double> st2{20, 5.0};\n";
    std::cout << "  fxt::tuple<int, double> ft2;\n";

    // Assignment from std::tuple to fxt::tuple
    ft2 = st2;
    std::cout << "\n  ft2 = st2;  // std::tuple -> fxt::tuple\n";
    auto [val1, val2] = ft2;
    std::cout << "  ft2 content: (" << val1 << ", " << val2 << ")\n";

    print_separator("6. Reference Conversion");

    std::cout << "Working with references:\n\n";

    fxt::tuple<int, std::string> source{99, "test"};
    std::cout << "  fxt::tuple<int, std::string> source{99, \"test\"};\n";

    // Get reference to std::tuple
    std::tuple<int, std::string>& ref = source;
    std::cout << "  std::tuple<int, std::string>& ref = source;\n";

    // Modify through reference
    std::get<0>(ref) = 100;
    std::get<1>(ref) = "modified";
    std::cout << "  std::get<0>(ref) = 100;\n";
    std::cout << "  std::get<1>(ref) = \"modified\";\n";

    // Check original
    auto [orig_val, orig_str] = source;
    std::cout << "\n  Original modified: (" << orig_val << ", \"" << orig_str << "\")\n";

    print_separator("Demo Complete");

    std::cout << "Key takeaways:\n";
    std::cout << "  • fxt::tuple can be implicitly converted to std::tuple\n";
    std::cout << "  • std::tuple can be implicitly converted to fxt::tuple\n";
    std::cout << "  • Conversions work for const&, &, and && references\n";
    std::cout << "  • Enables seamless interoperability with standard library\n";
    std::cout << "  • Can use FXT operations then convert back to std::tuple\n";
    std::cout << "  • Reference conversions allow direct modification\n\n";

    return 0;
}

