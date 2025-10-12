#include <fxt.hpp>
#include <iostream>
#include <string>

// Error type for expected examples
struct Error {
    std::string message;
};

void print_separator(const std::string& title) {
    std::cout << "\n=== " << title << " ===" << std::endl;
}

int main() {
    std::cout << "FXT Tuple Concatenation Demo\n";
    std::cout << "============================\n";

    // ========================================================================
    // Part 1: fxt::tuple_cat with fxt::tuple
    // ========================================================================
    print_separator("fxt::tuple_cat with fxt::tuple");

    // Direct call
    auto t1 = fxt::make_tuple(1, 2, 3);
    auto t2 = fxt::make_tuple(4.0, 5.0);
    auto result1 = fxt::tuple_cat(t1, t2);
    std::cout << "Direct call: tuple_cat(t1, t2)" << std::endl;
    std::cout << "  Result: (" << fxt::get<0>(result1) << ", "
              << fxt::get<1>(result1) << ", "
              << fxt::get<2>(result1) << ", "
              << fxt::get<3>(result1) << ", "
              << fxt::get<4>(result1) << ")" << std::endl;

    // Pipe operator
    auto t3 = fxt::make_tuple(10, 20);
    auto t4 = fxt::make_tuple(30.5, 40.5, 50.5);
    auto result2 = t3 | fxt::tuple_cat(t4);
    std::cout << "\nPipe operator: t3 | tuple_cat(t4)" << std::endl;
    std::cout << "  Result: (" << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", "
              << fxt::get<2>(result2) << ", "
              << fxt::get<3>(result2) << ", "
              << fxt::get<4>(result2) << ")" << std::endl;

    // ========================================================================
    // Part 2: fxt::tuple_cat with fxt::flat_tuple
    // ========================================================================
    print_separator("fxt::tuple_cat with fxt::flat_tuple");

    // Direct call
    auto ft1 = fxt::make_flat_tuple(100, 200);
    auto ft2 = fxt::make_flat_tuple(3.14, 2.71);
    auto result3 = fxt::tuple_cat(ft1, ft2);
    std::cout << "Direct call: tuple_cat(ft1, ft2)" << std::endl;
    std::cout << "  Result: (" << fxt::get<0>(result3) << ", "
              << fxt::get<1>(result3) << ", "
              << fxt::get<2>(result3) << ", "
              << fxt::get<3>(result3) << ")" << std::endl;

    // Pipe operator
    auto ft3 = fxt::make_flat_tuple(1.1, 2.2, 3.3);
    auto ft4 = fxt::make_flat_tuple(4.4, 5.5);
    auto result4 = ft3 | fxt::tuple_cat(ft4);
    std::cout << "\nPipe operator: ft3 | tuple_cat(ft4)" << std::endl;
    std::cout << "  Result: (" << fxt::get<0>(result4) << ", "
              << fxt::get<1>(result4) << ", "
              << fxt::get<2>(result4) << ", "
              << fxt::get<3>(result4) << ", "
              << fxt::get<4>(result4) << ")" << std::endl;

    // ========================================================================
    // Part 3: fxt::mtuple_cat with fxt::expected containing fxt::tuple
    // ========================================================================
    print_separator("fxt::mtuple_cat with fxt::expected<fxt::tuple>");

    // Concatenate expected<tuple> with plain tuple - direct call
    auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
    auto plain_t1 = fxt::make_tuple(3.0, 4.0);
    auto result5 = fxt::mtuple_cat(exp1, plain_t1);
    std::cout << "Direct call: mtuple_cat(expected<tuple>, tuple)" << std::endl;
    if (result5.has_value()) {
        auto val = result5.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ")" << std::endl;
    }

    // Pipe operator with expected<tuple> and plain tuple
    auto exp2 = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(10, 20, 30)};
    auto plain_t2 = fxt::make_tuple(40.5, 50.5);
    auto result6 = exp2 | fxt::mtuple_cat(plain_t2);
    std::cout << "\nPipe operator: expected<tuple> | mtuple_cat(tuple)" << std::endl;
    if (result6.has_value()) {
        auto val = result6.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
    }

    // Concatenate two expected<tuple> - direct call
    auto exp3 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(100, 200)};
    auto exp4 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::make_tuple(3.14, 2.71)};
    auto result7 = fxt::mtuple_cat(exp3, exp4);
    std::cout << "\nDirect call: mtuple_cat(expected<tuple>, expected<tuple>)" << std::endl;
    if (result7.has_value()) {
        auto val = result7.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ")" << std::endl;
    }

    // Pipe operator with two expected<tuple>
    auto exp5 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(5, 6)};
    auto exp6 = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(7, 8, 9)};
    auto result8 = exp5 | fxt::mtuple_cat(exp6);
    std::cout << "\nPipe operator: expected<tuple> | mtuple_cat(expected<tuple>)" << std::endl;
    if (result8.has_value()) {
        auto val = result8.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
    }

    // Error propagation example
    auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{"Something went wrong"}}};
    auto result9 = exp_err | fxt::mtuple_cat(fxt::make_tuple(1.0, 2.0));
    std::cout << "\nError propagation: expected<error> | mtuple_cat(tuple)" << std::endl;
    if (!result9.has_value()) {
        std::cout << "  Result: Error - " << result9.error().message << std::endl;
    }

    // ========================================================================
    // Part 4: fxt::mtuple_cat with fxt::optional containing fxt::tuple
    // ========================================================================
    print_separator("fxt::mtuple_cat with fxt::optional<fxt::tuple>");

    // Concatenate optional<tuple> with plain tuple - direct call
    auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
    auto plain_t3 = fxt::make_tuple(3.0, 4.0, 5.0);
    auto result10 = fxt::mtuple_cat(opt1, plain_t3);
    std::cout << "Direct call: mtuple_cat(optional<tuple>, tuple)" << std::endl;
    if (result10.has_value()) {
        auto val = result10.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
    }

    // Pipe operator with optional<tuple> and plain tuple
    auto opt2 = fxt::optional<fxt::tuple<double, double>>{fxt::make_tuple(1.1, 2.2)};
    auto plain_t4 = fxt::make_tuple(3, 4);
    auto result11 = opt2 | fxt::mtuple_cat(plain_t4);
    std::cout << "\nPipe operator: optional<tuple> | mtuple_cat(tuple)" << std::endl;
    if (result11.has_value()) {
        auto val = result11.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ")" << std::endl;
    }

    // Concatenate two optional<tuple>
    auto opt3 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(10, 20)};
    auto opt4 = fxt::optional<fxt::tuple<double, double>>{fxt::make_tuple(30.5, 40.5)};
    auto result12 = opt3 | fxt::mtuple_cat(opt4);
    std::cout << "\nPipe operator: optional<tuple> | mtuple_cat(optional<tuple>)" << std::endl;
    if (result12.has_value()) {
        auto val = result12.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ")" << std::endl;
    }

    // Empty optional propagation
    auto opt_empty = fxt::optional<fxt::tuple<int, int>>{};
    auto result13 = opt_empty | fxt::mtuple_cat(fxt::make_tuple(1.0, 2.0));
    std::cout << "\nEmpty optional propagation: optional<empty> | mtuple_cat(tuple)" << std::endl;
    if (!result13.has_value()) {
        std::cout << "  Result: Empty optional" << std::endl;
    }

    // ========================================================================
    // Part 5: fxt::mtuple_cat with fxt::flat_tuple
    // ========================================================================
    print_separator("fxt::mtuple_cat with fxt::expected<fxt::flat_tuple>");

    // Concatenate expected<flat_tuple> with plain flat_tuple
    auto exp_ft1 = fxt::expected<fxt::flat_tuple<int, int>, Error>{fxt::make_flat_tuple(1, 2)};
    auto plain_ft1 = fxt::make_flat_tuple(3.0, 4.0);
    auto result14 = exp_ft1 | fxt::mtuple_cat(plain_ft1);
    std::cout << "Pipe operator: expected<flat_tuple> | mtuple_cat(flat_tuple)" << std::endl;
    if (result14.has_value()) {
        auto val = result14.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ")" << std::endl;
    }

    // Concatenate optional<flat_tuple> with plain flat_tuple
    auto opt_ft1 = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.5, 2.5)};
    auto plain_ft2 = fxt::make_flat_tuple(3.5, 4.5, 5.5);
    auto result15 = opt_ft1 | fxt::mtuple_cat(plain_ft2);
    std::cout << "\nPipe operator: optional<flat_tuple> | mtuple_cat(flat_tuple)" << std::endl;
    if (result15.has_value()) {
        auto val = result15.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
    }

    // Concatenate two expected<flat_tuple>
    auto exp_ft2 = fxt::expected<fxt::flat_tuple<int, int>, Error>{fxt::make_flat_tuple(100, 200)};
    auto exp_ft3 = fxt::expected<fxt::flat_tuple<int, int, int>, Error>{fxt::make_flat_tuple(300, 400, 500)};
    auto result16 = exp_ft2 | fxt::mtuple_cat(exp_ft3);
    std::cout << "\nPipe operator: expected<flat_tuple> | mtuple_cat(expected<flat_tuple>)" << std::endl;
    if (result16.has_value()) {
        auto val = result16.value();
        std::cout << "  Result: (" << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
    }

    std::cout << "\n=== Demo completed successfully ===" << std::endl;
    return 0;
}

