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

int main() {
    std::cout << "=== fxt::take and fxt::take_last Demo ===\n\n";

    // ========================================================================
    // Part 1: mtake and mtake_last with fxt::tuple (original examples)
    // ========================================================================

    std::cout << "PART 1: mtake/mtake_last with fxt::tuple\n";
    std::cout << "==========================================\n\n";

    // Example 1: Basic take - extracting first N elements
    std::cout << "1. Basic mtake - extracting first N elements:\n";
    auto tuple1 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple1);
    std::cout << "\n";

    auto take2 = fxt::mtake<2>(tuple1);
    std::cout << "   After mtake<2>: ";
    print_tuple(take2);
    std::cout << "\n\n";

    // Example 2: Basic take_last - extracting last N elements
    std::cout << "2. Basic mtake_last - extracting last N elements:\n";
    auto tuple2 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple2);
    std::cout << "\n";

    auto take_last2 = fxt::mtake_last<2>(tuple2);
    std::cout << "   After mtake_last<2>: ";
    print_tuple(take_last2);
    std::cout << "\n\n";

    // Example 3: Take with mixed types
    std::cout << "3. mtake with mixed types:\n";
    auto mixed = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed);
    std::cout << "\n";

    auto mixed_take = fxt::mtake<3>(mixed);
    std::cout << "   After mtake<3>: ";
    print_tuple(mixed_take);
    std::cout << "\n\n";

    // ========================================================================
    // Part 2: mtake and mtake_last with fxt::flat_tuple
    // ========================================================================

    std::cout << "PART 2: mtake/mtake_last with fxt::flat_tuple\n";
    std::cout << "==============================================\n\n";

    // Example 4: Basic mtake with flat_tuple
    std::cout << "4. Basic mtake with flat_tuple:\n";
    auto ft1 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft1);
    std::cout << "\n";

    auto ft_take2 = fxt::mtake<2>(ft1);
    std::cout << "   After mtake<2>: ";
    print_flat_tuple(ft_take2);
    std::cout << "\n\n";

    // Example 5: Basic mtake_last with flat_tuple
    std::cout << "5. Basic mtake_last with flat_tuple:\n";
    auto ft2 = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0, 50.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft2);
    std::cout << "\n";

    auto ft_take_last3 = fxt::mtake_last<3>(ft2);
    std::cout << "   After mtake_last<3>: ";
    print_flat_tuple(ft_take_last3);
    std::cout << "\n\n";

    // Example 6: Extracting middle elements from flat_tuple
    std::cout << "6. Extracting middle elements from flat_tuple:\n";
    auto ft3 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
    std::cout << "   Original: ";
    print_flat_tuple(ft3);
    std::cout << "\n";

    auto ft_first = fxt::mtake<5>(ft3);
    std::cout << "   After mtake<5>: ";
    print_flat_tuple(ft_first);
    std::cout << "\n";

    auto ft_middle = fxt::mtake_last<3>(ft_first);
    std::cout << "   Then mtake_last<3>: ";
    print_flat_tuple(ft_middle);
    std::cout << " (elements 3-5)\n\n";

    // ========================================================================
    // Part 3: fxt::take and fxt::take_last with fxt::tuple (direct calls)
    // ========================================================================

    std::cout << "PART 3: take/take_last with fxt::tuple (direct calls)\n";
    std::cout << "======================================================\n\n";

    // Example 7: Direct call fxt::take on tuple
    std::cout << "7. Direct call fxt::take on tuple:\n";
    auto t1 = fxt::make_tuple(100, 200, 300, 400, 500);
    std::cout << "   Original: ";
    print_tuple(t1);
    std::cout << "\n";

    auto t1_result = fxt::take<3>(t1);
    std::cout << "   fxt::take<3>(t): ";
    print_tuple(t1_result);
    std::cout << "\n\n";

    // Example 8: Direct call fxt::take_last on tuple
    std::cout << "8. Direct call fxt::take_last on tuple:\n";
    auto t2 = fxt::make_tuple(10, 20, 30, 40, 50, 60);
    std::cout << "   Original: ";
    print_tuple(t2);
    std::cout << "\n";

    auto t2_result = fxt::take_last<4>(t2);
    std::cout << "   fxt::take_last<4>(t): ";
    print_tuple(t2_result);
    std::cout << "\n\n";

    // ========================================================================
    // Part 4: fxt::take and fxt::take_last with fxt::tuple (pipe operator)
    // ========================================================================

    std::cout << "PART 4: take/take_last with fxt::tuple (pipe operator)\n";
    std::cout << "=======================================================\n\n";

    // Example 9: Pipe operator with fxt::take on tuple
    std::cout << "9. Pipe operator with fxt::take on tuple:\n";
    auto t3 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "   Original: ";
    print_tuple(t3);
    std::cout << "\n";

    auto t3_result = t3 | fxt::take<4>();
    std::cout << "   t | fxt::take<4>(): ";
    print_tuple(t3_result);
    std::cout << "\n\n";

    // Example 10: Pipe operator with fxt::take_last on tuple
    std::cout << "10. Pipe operator with fxt::take_last on tuple:\n";
    auto t4 = fxt::make_tuple(5, 10, 15, 20, 25, 30);
    std::cout << "    Original: ";
    print_tuple(t4);
    std::cout << "\n";

    auto t4_result = t4 | fxt::take_last<3>();
    std::cout << "    t | fxt::take_last<3>(): ";
    print_tuple(t4_result);
    std::cout << "\n\n";

    // Example 11: Chaining take operations with pipe operator
    std::cout << "11. Chaining take operations with pipe operator:\n";
    auto t5 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "    Original: ";
    print_tuple(t5);
    std::cout << "\n";

    auto t5_result = t5 | fxt::take<7>() | fxt::take_last<4>();
    std::cout << "    t | take<7>() | take_last<4>(): ";
    print_tuple(t5_result);
    std::cout << " (elements 4-7)\n\n";

    // ========================================================================
    // Part 5: fxt::take and fxt::take_last with fxt::flat_tuple (direct calls)
    // ========================================================================

    std::cout << "PART 5: take/take_last with fxt::flat_tuple (direct calls)\n";
    std::cout << "===========================================================\n\n";

    // Example 12: Direct call fxt::take on flat_tuple
    std::cout << "12. Direct call fxt::take on flat_tuple:\n";
    auto ft4 = fxt::make_flat_tuple(1.5, 2.5, 3.5, 4.5, 5.5);
    std::cout << "    Original: ";
    print_flat_tuple(ft4);
    std::cout << "\n";

    auto ft4_result = fxt::take<3>(ft4);
    std::cout << "    fxt::take<3>(ft): ";
    print_flat_tuple(ft4_result);
    std::cout << "\n\n";

    // Example 13: Direct call fxt::take_last on flat_tuple
    std::cout << "13. Direct call fxt::take_last on flat_tuple:\n";
    auto ft5 = fxt::make_flat_tuple(10.0, 20.0, 30.0, 40.0, 50.0, 60.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft5);
    std::cout << "\n";

    auto ft5_result = fxt::take_last<4>(ft5);
    std::cout << "    fxt::take_last<4>(ft): ";
    print_flat_tuple(ft5_result);
    std::cout << "\n\n";

    // ========================================================================
    // Part 6: fxt::take and fxt::take_last with fxt::flat_tuple (pipe operator)
    // ========================================================================

    std::cout << "PART 6: take/take_last with fxt::flat_tuple (pipe operator)\n";
    std::cout << "============================================================\n\n";

    // Example 14: Pipe operator with fxt::take on flat_tuple
    std::cout << "14. Pipe operator with fxt::take on flat_tuple:\n";
    auto ft6 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft6);
    std::cout << "\n";

    auto ft6_result = ft6 | fxt::take<4>();
    std::cout << "    ft | fxt::take<4>(): ";
    print_flat_tuple(ft6_result);
    std::cout << "\n\n";

    // Example 15: Pipe operator with fxt::take_last on flat_tuple
    std::cout << "15. Pipe operator with fxt::take_last on flat_tuple:\n";
    auto ft7 = fxt::make_flat_tuple(5.0, 10.0, 15.0, 20.0, 25.0, 30.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft7);
    std::cout << "\n";

    auto ft7_result = ft7 | fxt::take_last<3>();
    std::cout << "    ft | fxt::take_last<3>(): ";
    print_flat_tuple(ft7_result);
    std::cout << "\n\n";

    // Example 16: Chaining take operations on flat_tuple with pipe operator
    std::cout << "16. Chaining take operations on flat_tuple:\n";
    auto ft8 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft8);
    std::cout << "\n";

    auto ft8_result = ft8 | fxt::take<7>() | fxt::take_last<4>();
    std::cout << "    ft | take<7>() | take_last<4>(): ";
    print_flat_tuple(ft8_result);
    std::cout << " (elements 4-7)\n\n";

    // ========================================================================
    // Part 7: Comparison and Mixed Usage Examples
    // ========================================================================

    std::cout << "PART 7: Comparison and Mixed Usage\n";
    std::cout << "===================================\n\n";

    // Example 17: Comparing mtake vs take on same data
    std::cout << "17. Comparing mtake vs take on fxt::tuple:\n";
    auto t6 = fxt::make_tuple(1, 2, 3, 4, 5);
    std::cout << "    Original: ";
    print_tuple(t6);
    std::cout << "\n";

    auto t6_mtake = fxt::mtake<3>(t6);
    auto t6_take = fxt::take<3>(t6);
    std::cout << "    mtake<3>: ";
    print_tuple(t6_mtake);
    std::cout << "\n";
    std::cout << "    take<3>:  ";
    print_tuple(t6_take);
    std::cout << " (same result!)\n\n";

    // Example 18: Comparing mtake vs take on flat_tuple
    std::cout << "18. Comparing mtake vs take on fxt::flat_tuple:\n";
    auto ft9 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    std::cout << "    Original: ";
    print_flat_tuple(ft9);
    std::cout << "\n";

    auto ft9_mtake = fxt::mtake<3>(ft9);
    auto ft9_take = fxt::take<3>(ft9);
    std::cout << "    mtake<3>: ";
    print_flat_tuple(ft9_mtake);
    std::cout << "\n";
    std::cout << "    take<3>:  ";
    print_flat_tuple(ft9_take);
    std::cout << " (same result!)\n\n";

    // Example 19: Complex chaining with both tuple types
    std::cout << "19. Complex chaining demonstration:\n";
    auto complex_tuple = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
    auto complex_ft = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0);

    std::cout << "    Tuple chain: ";
    print_tuple(complex_tuple);
    auto complex_result1 = complex_tuple | fxt::take<6>() | fxt::take_last<3>();
    std::cout << " -> take<6>() -> take_last<3>() = ";
    print_tuple(complex_result1);
    std::cout << "\n";

    std::cout << "    FlatTuple chain: ";
    print_flat_tuple(complex_ft);
    auto complex_result2 = complex_ft | fxt::take<6>() | fxt::take_last<3>();
    std::cout << " -> take<6>() -> take_last<3>() = ";
    print_flat_tuple(complex_result2);
    std::cout << "\n\n";

    // Example 20: Edge cases
    std::cout << "20. Edge cases:\n";

    // Take zero elements
    auto t7 = fxt::make_tuple(10, 20, 30);
    auto ft10 = fxt::make_flat_tuple(10.0, 20.0, 30.0);

    auto t7_empty = fxt::take<0>(t7);
    auto ft10_empty = fxt::take<0>(ft10);
    std::cout << "    take<0> on tuple: ";
    print_tuple(t7_empty);
    std::cout << " (empty)\n";
    std::cout << "    take<0> on flat_tuple: ";
    print_flat_tuple(ft10_empty);
    std::cout << " (empty)\n";

    // Take all elements
    auto t8_all = fxt::take<3>(t7);
    auto ft10_all = fxt::take<3>(ft10);
    std::cout << "    take<3> on 3-element tuple: ";
    print_tuple(t8_all);
    std::cout << " (all)\n";
    std::cout << "    take<3> on 3-element flat_tuple: ";
    print_flat_tuple(ft10_all);
    std::cout << " (all)\n\n";

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
