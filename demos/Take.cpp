//
// Created by kenne on 05/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <tuple>

// Helper function to print tuples
template<typename... Args>
void print_tuple(const std::tuple<Args...>& t) {
    std::cout << "(";
    fxt::apply([](const auto&... args) {
        std::size_t n = 0;
        ((std::cout << args << (++n != sizeof...(args) ? ", " : "")), ...);
    }, t);
    std::cout << ")";
}

// Helper function to print fxt::tuples
template<typename... Args>
void print_fxt_tuple(const fxt::tuple<Args...>& t) {
    std::cout << "(";
    fxt::apply([](const auto&... args) {
        std::size_t n = 0;
        ((std::cout << args << (++n != sizeof...(args) ? ", " : "")), ...);
    }, t);
    std::cout << ")";
}

// Helper function to print flat_tuples
template<typename... Args>
void print_flat_tuple(const fxt::flat_tuple<Args...>& ft) {
    std::cout << "(";
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        std::size_t n = 0;
        ((std::cout << fxt::get<Is>(ft) << (++n != sizeof...(Is) ? ", " : "")), ...);
    }(std::index_sequence_for<Args...>{});
    std::cout << ")";
}

// Helper function to print expected results
template<typename T, typename E>
void print_expected(const fxt::expected<T, E>& exp, const std::string& prefix = "") {
    std::cout << prefix;
    if (exp.has_value()) {
        if constexpr (requires { print_fxt_tuple(*exp); }) {
            print_fxt_tuple(*exp);
        } else if constexpr (requires { print_flat_tuple(*exp); }) {
            print_flat_tuple(*exp);
        }
    } else {
        std::cout << "Error: " << exp.error();
    }
}

int main() {
    std::cout << "=== fxt::take, fxt::take_last, fxt::mtake, and fxt::mtake_last Demo ===\n\n";

    // ========================================================================
    // Part 1: take and take_last with fxt::tuple (direct calls)
    // ========================================================================

    std::cout << "PART 1: take/take_last with fxt::tuple (direct calls)\n";
    std::cout << "======================================================\n\n";

    // Example 1: Basic take - extracting first N elements
    std::cout << "1. Basic take - extracting first N elements:\n";
    auto tuple1 = fxt::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_fxt_tuple(tuple1);
    std::cout << "\n";

    auto take2 = fxt::take<2>(tuple1);
    std::cout << "   After take<2>: ";
    print_fxt_tuple(take2);
    std::cout << "\n\n";

    // Example 2: Basic take_last - extracting last N elements
    std::cout << "2. Basic take_last - extracting last N elements:\n";
    auto tuple2 = fxt::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_fxt_tuple(tuple2);
    std::cout << "\n";

    auto take_last2 = fxt::take_last<2>(tuple2);
    std::cout << "   After take_last<2>: ";
    print_fxt_tuple(take_last2);
    std::cout << "\n\n";

    // Example 3: Take with mixed types
    std::cout << "3. take with mixed types:\n";
    auto mixed = fxt::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_fxt_tuple(mixed);
    std::cout << "\n";

    auto mixed_take = fxt::take<3>(mixed);
    std::cout << "   After take<3>: ";
    print_fxt_tuple(mixed_take);
    std::cout << "\n\n";

    // ========================================================================
    // Part 2: take and take_last with fxt::flat_tuple (direct calls)
    // ========================================================================

    std::cout << "PART 2: take/take_last with fxt::flat_tuple (direct calls)\n";
    std::cout << "===========================================================\n\n";

    // Example 4: Basic take with flat_tuple
    std::cout << "4. Basic take with flat_tuple:\n";
    auto ft1 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft1);
    std::cout << "\n";

    auto ft_take2 = fxt::take<2>(ft1);
    std::cout << "   After take<2>: ";
    print_flat_tuple(ft_take2);
    std::cout << "\n\n";

    // Example 5: Basic take_last with flat_tuple
    std::cout << "5. Basic take_last with flat_tuple:\n";
    auto ft2 = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0, 50.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft2);
    std::cout << "\n";

    auto ft_take_last3 = fxt::take_last<3>(ft2);
    std::cout << "   After take_last<3>: ";
    print_flat_tuple(ft_take_last3);
    std::cout << "\n\n";

    // ========================================================================
    // Part 3: take and take_last with pipe operator
    // ========================================================================

    std::cout << "PART 3: take/take_last with pipe operator\n";
    std::cout << "==========================================\n\n";

    // Example 6: Pipe operator with fxt::take on tuple
    std::cout << "6. Pipe operator with fxt::take on tuple:\n";
    auto t3 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "   Original: ";
    print_fxt_tuple(t3);
    std::cout << "\n";

    auto t3_result = t3 | fxt::take<4>();
    std::cout << "   t | fxt::take<4>(): ";
    print_fxt_tuple(t3_result);
    std::cout << "\n\n";

    // Example 7: Pipe operator with fxt::take_last on tuple
    std::cout << "7. Pipe operator with fxt::take_last on tuple:\n";
    auto t4 = fxt::make_tuple(5, 10, 15, 20, 25, 30);
    std::cout << "   Original: ";
    print_fxt_tuple(t4);
    std::cout << "\n";

    auto t4_result = t4 | fxt::take_last<3>();
    std::cout << "   t | fxt::take_last<3>(): ";
    print_fxt_tuple(t4_result);
    std::cout << "\n\n";

    // Example 8: Chaining take operations with pipe operator
    std::cout << "8. Chaining take operations with pipe operator:\n";
    auto t5 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original: ";
    print_fxt_tuple(t5);
    std::cout << "\n";

    auto t5_result = t5 | fxt::take<7>() | fxt::take_last<4>();
    std::cout << "   t | take<7>() | take_last<4>(): ";
    print_fxt_tuple(t5_result);
    std::cout << " (elements 4-7)\n\n";

    // ========================================================================
    // Part 4: mtake and mtake_last with fxt::expected (direct calls)
    // ========================================================================

    std::cout << "PART 4: mtake/mtake_last with fxt::expected<tuple> (direct calls)\n";
    std::cout << "===================================================================\n\n";

    // Example 9: mtake with expected containing tuple (success case)
    std::cout << "9. mtake with expected containing tuple (success case):\n";
    auto exp1 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(10, 20, 30, 40, 50)
    };
    std::cout << "   Original: ";
    print_expected(exp1);
    std::cout << "\n";

    auto exp1_result = fxt::mtake<3>(exp1);
    std::cout << "   After mtake<3>: ";
    print_expected(exp1_result);
    std::cout << "\n\n";

    // Example 10: mtake_last with expected containing tuple (success case)
    std::cout << "10. mtake_last with expected containing tuple (success case):\n";
    auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(100, 200, 300, 400, 500)
    };
    std::cout << "    Original: ";
    print_expected(exp2);
    std::cout << "\n";

    auto exp2_result = fxt::mtake_last<2>(exp2);
    std::cout << "    After mtake_last<2>: ";
    print_expected(exp2_result);
    std::cout << "\n\n";

    // Example 11: mtake with expected containing error
    std::cout << "11. mtake with expected containing error (error propagation):\n";
    auto exp3 = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
        fxt::unexpected{std::string("Something went wrong")}
    };
    std::cout << "    Original: ";
    print_expected(exp3);
    std::cout << "\n";

    auto exp3_result = fxt::mtake<2>(exp3);
    std::cout << "    After mtake<2>: ";
    print_expected(exp3_result);
    std::cout << " (error preserved)\n\n";

    // ========================================================================
    // Part 5: mtake and mtake_last with fxt::expected<flat_tuple>
    // ========================================================================

    std::cout << "PART 5: mtake/mtake_last with fxt::expected<flat_tuple>\n";
    std::cout << "========================================================\n\n";

    // Example 12: mtake with expected containing flat_tuple
    std::cout << "12. mtake with expected containing flat_tuple:\n";
    auto exp_ft1 = fxt::expected<fxt::flat_tuple<double, double, double, double, double>, std::string>{
        fxt::make_flat_tuple(1.1, 2.2, 3.3, 4.4, 5.5)
    };
    std::cout << "    Original: ";
    print_expected(exp_ft1);
    std::cout << "\n";

    auto exp_ft1_result = fxt::mtake<3>(exp_ft1);
    std::cout << "    After mtake<3>: ";
    print_expected(exp_ft1_result);
    std::cout << "\n\n";

    // Example 13: mtake_last with expected containing flat_tuple
    std::cout << "13. mtake_last with expected containing flat_tuple:\n";
    auto exp_ft2 = fxt::expected<fxt::flat_tuple<double, double, double, double>, std::string>{
        fxt::make_flat_tuple(10.5, 20.5, 30.5, 40.5)
    };
    std::cout << "    Original: ";
    print_expected(exp_ft2);
    std::cout << "\n";

    auto exp_ft2_result = fxt::mtake_last<2>(exp_ft2);
    std::cout << "    After mtake_last<2>: ";
    print_expected(exp_ft2_result);
    std::cout << "\n\n";

    // ========================================================================
    // Part 6: mtake and mtake_last with pipe operator
    // ========================================================================

    std::cout << "PART 6: mtake/mtake_last with pipe operator\n";
    std::cout << "============================================\n\n";

    // Example 14: Pipe operator with mtake on expected
    std::cout << "14. Pipe operator with mtake on expected:\n";
    auto exp4 = fxt::expected<fxt::tuple<int, int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5, 6)
    };
    std::cout << "    Original: ";
    print_expected(exp4);
    std::cout << "\n";

    auto exp4_result = exp4 | fxt::mtake<4>();
    std::cout << "    exp | fxt::mtake<4>(): ";
    print_expected(exp4_result);
    std::cout << "\n\n";

    // Example 15: Pipe operator with mtake_last on expected
    std::cout << "15. Pipe operator with mtake_last on expected:\n";
    auto exp5 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(10, 20, 30, 40, 50)
    };
    std::cout << "    Original: ";
    print_expected(exp5);
    std::cout << "\n";

    auto exp5_result = exp5 | fxt::mtake_last<3>();
    std::cout << "    exp | fxt::mtake_last<3>(): ";
    print_expected(exp5_result);
    std::cout << "\n\n";

    // Example 16: Chaining mtake operations with pipe operator
    std::cout << "16. Chaining mtake operations with pipe operator:\n";
    auto exp6 = fxt::expected<fxt::tuple<int, int, int, int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8)
    };
    std::cout << "    Original: ";
    print_expected(exp6);
    std::cout << "\n";

    auto exp6_result = exp6 | fxt::mtake<6>() | fxt::mtake_last<3>();
    std::cout << "    exp | mtake<6>() | mtake_last<3>(): ";
    print_expected(exp6_result);
    std::cout << " (elements 4-6)\n\n";

    // Example 17: Chaining with error in the pipeline
    std::cout << "17. Chaining with error in the pipeline:\n";
    auto exp7 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::unexpected{std::string("Pipeline error")}
    };
    std::cout << "    Original: ";
    print_expected(exp7);
    std::cout << "\n";

    auto exp7_result = exp7 | fxt::mtake<3>() | fxt::mtake_last<2>();
    std::cout << "    exp | mtake<3>() | mtake_last<2>(): ";
    print_expected(exp7_result);
    std::cout << " (error propagates)\n\n";

    // ========================================================================
    // Part 7: Comparison between take and mtake
    // ========================================================================

    std::cout << "PART 7: Comparison between take and mtake\n";
    std::cout << "==========================================\n\n";

    // Example 18: Direct tuple vs monadic tuple
    std::cout << "18. Comparing take (direct) vs mtake (monadic):\n";
    auto direct_tuple = fxt::make_tuple(1, 2, 3, 4, 5);
    auto monadic_tuple = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5)
    };

    std::cout << "    Direct tuple: ";
    print_fxt_tuple(direct_tuple);
    std::cout << "\n";
    std::cout << "    Direct tuple | take<3>(): ";
    print_fxt_tuple(direct_tuple | fxt::take<3>());
    std::cout << "\n\n";

    std::cout << "    Monadic tuple: ";
    print_expected(monadic_tuple);
    std::cout << "\n";
    std::cout << "    Monadic tuple | mtake<3>(): ";
    print_expected(monadic_tuple | fxt::mtake<3>());
    std::cout << "\n\n";

    // ========================================================================
    // Part 8: Edge cases
    // ========================================================================

    std::cout << "PART 8: Edge cases\n";
    std::cout << "==================\n\n";

    // Example 19: Take zero elements
    std::cout << "19. Edge cases - take zero elements:\n";
    auto t7 = fxt::make_tuple(10, 20, 30);
    auto exp8 = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(10, 20, 30)};

    auto t7_empty = fxt::take<0>(t7);
    auto exp8_empty = exp8 | fxt::mtake<0>();
    std::cout << "    take<0> on tuple: ";
    print_fxt_tuple(t7_empty);
    std::cout << " (empty)\n";
    std::cout << "    mtake<0> on expected: ";
    print_expected(exp8_empty);
    std::cout << " (empty)\n\n";

    // Example 20: Take all elements
    std::cout << "20. Edge cases - take all elements:\n";
    auto t8_all = fxt::take<3>(t7);
    auto exp9_all = exp8 | fxt::mtake<3>();
    std::cout << "    take<3> on 3-element tuple: ";
    print_fxt_tuple(t8_all);
    std::cout << " (all)\n";
    std::cout << "    mtake<3> on 3-element expected: ";
    print_expected(exp9_all);
    std::cout << " (all)\n\n";

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
