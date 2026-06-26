//
// Created by kenne on 05/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <tuple>

// Helper function to print std::tuples
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

// Helper function to print fxt::flat_tuple
template<typename... Args>
void print_flat_tuple(const fxt::flat_tuple<Args...>& t) {
    std::cout << "(";
    [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        std::size_t n = 0;
        ((std::cout << fxt::get<Is>(t) << (++n != sizeof...(Args) ? ", " : "")), ...);
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
    std::cout << "=== fxt::drop, fxt::drop_last, fxt::mdrop, and fxt::mdrop_last Demo ===\n\n";

    // ========================================================================
    // Part 1: drop and drop_last with std::tuple (direct calls)
    // ========================================================================

    std::cout << "PART 1: drop/drop_last with std::tuple (direct calls)\n";
    std::cout << "======================================================\n\n";

    // Example 1: Basic drop - removing elements from the beginning
    std::cout << "1. Basic drop - removing first N elements:\n";
    auto tuple1 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple1);
    std::cout << "\n";

    auto drop1 = fxt::tuple_drop<2>(tuple1);
    std::cout << "   After drop<2>: ";
    print_tuple(drop1);
    std::cout << "\n\n";

    // Example 2: Basic drop_last - removing elements from the end
    std::cout << "2. Basic drop_last - removing last N elements:\n";
    auto tuple2 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple2);
    std::cout << "\n";

    auto drop_last2 = fxt::tuple_drop_last<2>(tuple2);
    std::cout << "   After drop_last<2>: ";
    print_tuple(drop_last2);
    std::cout << "\n\n";

    // Example 3: Drop with mixed types
    std::cout << "3. drop with mixed types:\n";
    auto mixed = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed);
    std::cout << "\n";

    auto mixed_drop = fxt::tuple_drop<2>(mixed);
    std::cout << "   After drop<2>: ";
    print_tuple(mixed_drop);
    std::cout << "\n\n";

    // ========================================================================
    // Part 2: drop and drop_last with fxt::tuple (direct calls)
    // ========================================================================

    std::cout << "PART 2: drop/drop_last with fxt::tuple (direct calls)\n";
    std::cout << "======================================================\n\n";

    // Example 4: Basic drop with fxt::tuple
    std::cout << "4. Basic drop with fxt::tuple:\n";
    auto t1 = fxt::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_fxt_tuple(t1);
    std::cout << "\n";

    auto t1_drop = fxt::tuple_drop<2>(t1);
    std::cout << "   After drop<2>: ";
    print_fxt_tuple(t1_drop);
    std::cout << "\n\n";

    // Example 5: Basic drop_last with fxt::tuple
    std::cout << "5. Basic drop_last with fxt::tuple:\n";
    auto t2 = fxt::make_tuple(10, 20, 30, 40, 50);
    std::cout << "   Original: ";
    print_fxt_tuple(t2);
    std::cout << "\n";

    auto t2_drop_last = fxt::tuple_drop_last<2>(t2);
    std::cout << "   After drop_last<2>: ";
    print_fxt_tuple(t2_drop_last);
    std::cout << "\n\n";

    // Example 6: Drop zero elements (identity)
    std::cout << "6. Drop zero elements (identity operation):\n";
    auto t3 = fxt::make_tuple(10, 20, 30);
    std::cout << "   Original: ";
    print_fxt_tuple(t3);
    std::cout << "\n";

    auto no_drop = fxt::tuple_drop<0>(t3);
    std::cout << "   After drop<0>: ";
    print_fxt_tuple(no_drop);
    std::cout << "\n\n";

    // ========================================================================
    // Part 3: drop and drop_last with fxt::flat_tuple (direct calls)
    // ========================================================================

    std::cout << "PART 3: drop/drop_last with fxt::flat_tuple (direct calls)\n";
    std::cout << "===========================================================\n\n";

    // Example 7: Basic drop with flat_tuple
    std::cout << "7. Basic drop with flat_tuple:\n";
    auto ft1 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft1);
    std::cout << "\n";

    auto ft1_drop = fxt::tuple_drop<2>(ft1);
    std::cout << "   After drop<2>: ";
    print_flat_tuple(ft1_drop);
    std::cout << "\n\n";

    // Example 8: Basic drop_last with flat_tuple
    std::cout << "8. Basic drop_last with flat_tuple:\n";
    auto ft2 = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0, 50.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft2);
    std::cout << "\n";

    auto ft2_drop_last = fxt::tuple_drop_last<2>(ft2);
    std::cout << "   After drop_last<2>: ";
    print_flat_tuple(ft2_drop_last);
    std::cout << "\n\n";

    // ========================================================================
    // Part 4: drop and drop_last with pipe operator
    // ========================================================================

    std::cout << "PART 4: drop/drop_last with pipe operator\n";
    std::cout << "==========================================\n\n";

    // Example 9: Pipe operator with fxt::drop on tuple
    std::cout << "9. Pipe operator with fxt::drop on tuple:\n";
    auto t4 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "   Original: ";
    print_fxt_tuple(t4);
    std::cout << "\n";

    auto t4_result = t4 | fxt::tuple_drop<2>();
    std::cout << "   t | fxt::tuple_drop<2>(): ";
    print_fxt_tuple(t4_result);
    std::cout << "\n\n";

    // Example 10: Pipe operator with fxt::drop_last on tuple
    std::cout << "10. Pipe operator with fxt::drop_last on tuple:\n";
    auto t5 = fxt::make_tuple(5, 10, 15, 20, 25, 30);
    std::cout << "    Original: ";
    print_fxt_tuple(t5);
    std::cout << "\n";

    auto t5_result = t5 | fxt::tuple_drop_last<2>();
    std::cout << "    t | fxt::tuple_drop_last<2>(): ";
    print_fxt_tuple(t5_result);
    std::cout << "\n\n";

    // Example 11: Chaining drop operations with pipe operator
    std::cout << "11. Chaining drop operations with pipe operator:\n";
    auto t6 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "    Original: ";
    print_fxt_tuple(t6);
    std::cout << "\n";

    auto t6_result = t6 | fxt::tuple_drop<2>() | fxt::tuple_drop_last<3>();
    std::cout << "    t | drop<2>() | drop_last<3>(): ";
    print_fxt_tuple(t6_result);
    std::cout << " (elements 3-7)\n\n";

    // Example 12: Chaining with flat_tuple
    std::cout << "12. Chaining drop operations on flat_tuple:\n";
    auto ft3 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft3);
    std::cout << "\n";

    auto ft3_result = ft3 | fxt::tuple_drop<2>() | fxt::tuple_drop_last<2>();
    std::cout << "    ft | drop<2>() | drop_last<2>(): ";
    print_flat_tuple(ft3_result);
    std::cout << "\n\n";

    // ========================================================================
    // Part 5: mdrop and mdrop_last with fxt::expected (direct calls)
    // ========================================================================

    std::cout << "PART 5: mdrop/mdrop_last with fxt::expected<tuple> (direct calls)\n";
    std::cout << "===================================================================\n\n";

    // Example 13: mdrop with expected containing tuple (success case)
    std::cout << "13. mdrop with expected containing tuple (success case):\n";
    auto exp1 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(10, 20, 30, 40, 50)
    };
    std::cout << "    Original: ";
    print_expected(exp1);
    std::cout << "\n";

    auto exp1_result = fxt::mtuple_drop<2>(exp1);
    std::cout << "    After mdrop<2>: ";
    print_expected(exp1_result);
    std::cout << "\n\n";

    // Example 14: mdrop_last with expected containing tuple (success case)
    std::cout << "14. mdrop_last with expected containing tuple (success case):\n";
    auto exp2 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(100, 200, 300, 400, 500)
    };
    std::cout << "    Original: ";
    print_expected(exp2);
    std::cout << "\n";

    auto exp2_result = fxt::mtuple_drop_last<2>(exp2);
    std::cout << "    After mdrop_last<2>: ";
    print_expected(exp2_result);
    std::cout << "\n\n";

    // Example 15: mdrop with expected containing error
    std::cout << "15. mdrop with expected containing error (error propagation):\n";
    auto exp3 = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
        fxt::unexpected{std::string("Something went wrong")}
    };
    std::cout << "    Original: ";
    print_expected(exp3);
    std::cout << "\n";

    auto exp3_result = fxt::mtuple_drop<2>(exp3);
    std::cout << "    After mdrop<2>: ";
    print_expected(exp3_result);
    std::cout << " (error preserved)\n\n";

    // ========================================================================
    // Part 6: mdrop and mdrop_last with fxt::expected<flat_tuple>
    // ========================================================================

    std::cout << "PART 6: mdrop/mdrop_last with fxt::expected<flat_tuple>\n";
    std::cout << "========================================================\n\n";

    // Example 16: mdrop with expected containing flat_tuple
    std::cout << "16. mdrop with expected containing flat_tuple:\n";
    auto exp_ft1 = fxt::expected<fxt::flat_tuple<double, double, double, double, double>, std::string>{
        fxt::make_flat_tuple(1.1, 2.2, 3.3, 4.4, 5.5)
    };
    std::cout << "    Original: ";
    print_expected(exp_ft1);
    std::cout << "\n";

    auto exp_ft1_result = fxt::mtuple_drop<2>(exp_ft1);
    std::cout << "    After mdrop<2>: ";
    print_expected(exp_ft1_result);
    std::cout << "\n\n";

    // Example 17: mdrop_last with expected containing flat_tuple
    std::cout << "17. mdrop_last with expected containing flat_tuple:\n";
    auto exp_ft2 = fxt::expected<fxt::flat_tuple<double, double, double, double>, std::string>{
        fxt::make_flat_tuple(10.5, 20.5, 30.5, 40.5)
    };
    std::cout << "    Original: ";
    print_expected(exp_ft2);
    std::cout << "\n";

    auto exp_ft2_result = fxt::mtuple_drop_last<2>(exp_ft2);
    std::cout << "    After mdrop_last<2>: ";
    print_expected(exp_ft2_result);
    std::cout << "\n\n";

    // ========================================================================
    // Part 7: mdrop and mdrop_last with pipe operator
    // ========================================================================

    std::cout << "PART 7: mdrop/mdrop_last with pipe operator\n";
    std::cout << "============================================\n\n";

    // Example 18: Pipe operator with mdrop on expected
    std::cout << "18. Pipe operator with mdrop on expected:\n";
    auto exp4 = fxt::expected<fxt::tuple<int, int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5, 6)
    };
    std::cout << "    Original: ";
    print_expected(exp4);
    std::cout << "\n";

    auto exp4_result = exp4 | fxt::mtuple_drop<2>();
    std::cout << "    exp | fxt::mtuple_drop<2>(): ";
    print_expected(exp4_result);
    std::cout << "\n\n";

    // Example 19: Pipe operator with mdrop_last on expected
    std::cout << "19. Pipe operator with mdrop_last on expected:\n";
    auto exp5 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(10, 20, 30, 40, 50)
    };
    std::cout << "    Original: ";
    print_expected(exp5);
    std::cout << "\n";

    auto exp5_result = exp5 | fxt::mtuple_drop_last<2>();
    std::cout << "    exp | fxt::mtuple_drop_last<2>(): ";
    print_expected(exp5_result);
    std::cout << "\n\n";

    // Example 20: Chaining mdrop operations with pipe operator
    std::cout << "20. Chaining mdrop operations with pipe operator:\n";
    auto exp6 = fxt::expected<fxt::tuple<int, int, int, int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8)
    };
    std::cout << "    Original: ";
    print_expected(exp6);
    std::cout << "\n";

    auto exp6_result = exp6 | fxt::mtuple_drop<2>() | fxt::mtuple_drop_last<2>();
    std::cout << "    exp | mdrop<2>() | mdrop_last<2>(): ";
    print_expected(exp6_result);
    std::cout << " (elements 3-6)\n\n";

    // Example 21: Chaining with error in the pipeline
    std::cout << "21. Chaining with error in the pipeline:\n";
    auto exp7 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::unexpected{std::string("Pipeline error")}
    };
    std::cout << "    Original: ";
    print_expected(exp7);
    std::cout << "\n";

    auto exp7_result = exp7 | fxt::mtuple_drop<1>() | fxt::mtuple_drop_last<1>();
    std::cout << "    exp | mdrop<1>() | mdrop_last<1>(): ";
    print_expected(exp7_result);
    std::cout << " (error propagates)\n\n";

    // ========================================================================
    // Part 8: Comparison between drop and mdrop
    // ========================================================================

    std::cout << "PART 8: Comparison between drop and mdrop\n";
    std::cout << "==========================================\n\n";

    // Example 22: Direct tuple vs monadic tuple
    std::cout << "22. Comparing drop (direct) vs mdrop (monadic):\n";
    auto direct_tuple = fxt::make_tuple(1, 2, 3, 4, 5);
    auto monadic_tuple = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3, 4, 5)
    };

    std::cout << "    Direct tuple: ";
    print_fxt_tuple(direct_tuple);
    std::cout << "\n";
    std::cout << "    Direct tuple | drop<2>(): ";
    print_fxt_tuple(direct_tuple | fxt::tuple_drop<2>());
    std::cout << "\n\n";

    std::cout << "    Monadic tuple: ";
    print_expected(monadic_tuple);
    std::cout << "\n";
    std::cout << "    Monadic tuple | mdrop<2>(): ";
    print_expected(monadic_tuple | fxt::mtuple_drop<2>());
    std::cout << "\n\n";

    // ========================================================================
    // Part 9: Practical use cases
    // ========================================================================

    std::cout << "PART 9: Practical use cases\n";
    std::cout << "===========================\n\n";

    // Example 23: Parsing a protocol message
    std::cout << "23. Practical use case - parsing a message:\n";
    // Simulating a message: [header, version, type, data1, data2, data3, checksum]
    auto message = fxt::make_tuple(
        std::string("HDR"),
        1,
        std::string("CMD"),
        42,
        3.14,
        std::string("payload"),
        0xABCD
    );
    std::cout << "    Full message: ";
    print_fxt_tuple(message);
    std::cout << "\n";

    // Extract payload by dropping header fields and checksum
    auto without_header = message | fxt::tuple_drop<3>();  // Skip header, version, type
    auto payload = without_header | fxt::tuple_drop_last<1>();  // Skip checksum
    std::cout << "    Extracted payload (data only): ";
    print_fxt_tuple(payload);
    std::cout << "\n\n";

    // Example 24: Extracting middle elements
    std::cout << "24. Extracting middle elements from a tuple:\n";
    auto data = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "    Original: ";
    print_fxt_tuple(data);
    std::cout << "\n";

    // Drop first 2 elements, then drop last 2 elements
    auto middle = data | fxt::tuple_drop<2>() | fxt::tuple_drop_last<2>();
    std::cout << "    Middle elements (drop<2> then drop_last<2>): ";
    print_fxt_tuple(middle);
    std::cout << "\n\n";

    // ========================================================================
    // Part 10: Edge cases
    // ========================================================================

    std::cout << "PART 10: Edge cases\n";
    std::cout << "===================\n\n";

    // Example 25: Drop all elements
    std::cout << "25. Edge cases - drop all elements:\n";
    auto t7 = fxt::make_tuple(10, 20, 30);
    auto exp8 = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::make_tuple(10, 20, 30)};

    auto t7_empty = fxt::tuple_drop<3>(t7);
    auto exp8_empty = exp8 | fxt::mtuple_drop<3>();
    std::cout << "    drop<3> on 3-element tuple: ";
    print_fxt_tuple(t7_empty);
    std::cout << " (empty)\n";
    std::cout << "    mdrop<3> on 3-element expected: ";
    print_expected(exp8_empty);
    std::cout << " (empty)\n\n";

    // Example 26: Complex chaining
    std::cout << "26. Complex chaining demonstration:\n";
    auto complex_tuple = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
    auto complex_ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);

    std::cout << "    Tuple chain: ";
    print_fxt_tuple(complex_tuple);
    auto complex_result1 = complex_tuple | fxt::tuple_drop<2>() | fxt::tuple_drop_last<2>();
    std::cout << " -> drop<2>() -> drop_last<2>() = ";
    print_fxt_tuple(complex_result1);
    std::cout << "\n";

    std::cout << "    FlatTuple chain: ";
    print_flat_tuple(complex_ft);
    auto complex_result2 = complex_ft | fxt::tuple_drop<2>() | fxt::tuple_drop_last<2>();
    std::cout << " -> drop<2>() -> drop_last<2>() = ";
    print_flat_tuple(complex_result2);
    std::cout << "\n\n";

    std::cout << "=== Demo Complete ===\n";

    return 0;
}
