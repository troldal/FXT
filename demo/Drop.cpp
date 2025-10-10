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
    std::apply([](const auto&... args) {
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

int main() {
    std::cout << "=== fxt::drop and fxt::drop_last Demo ===\n\n";

    // Example 1: Basic drop - removing elements from the beginning
    std::cout << "1. Basic drop - removing first N elements:\n";
    auto tuple1 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple1);
    std::cout << "\n";

    auto drop1 = fxt::mdrop<2>(tuple1);
    std::cout << "   After drop<2>: ";
    print_tuple(drop1);
    std::cout << "\n\n";

    // Example 2: Basic drop_last - removing elements from the end
    std::cout << "2. Basic drop_last - removing last N elements:\n";
    auto tuple2 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple2);
    std::cout << "\n";

    auto drop_last2 = fxt::mdrop_last<2>(tuple2);
    std::cout << "   After drop_last<2>: ";
    print_tuple(drop_last2);
    std::cout << "\n\n";

    // Example 3: Drop with mixed types
    std::cout << "3. Drop with mixed types:\n";
    auto mixed = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed);
    std::cout << "\n";

    auto mixed_drop = fxt::mdrop<2>(mixed);
    std::cout << "   After drop<2>: ";
    print_tuple(mixed_drop);
    std::cout << "\n\n";

    // Example 4: Drop_last with mixed types
    std::cout << "4. Drop_last with mixed types:\n";
    auto mixed2 = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed2);
    std::cout << "\n";

    auto mixed_drop_last = fxt::mdrop_last<2>(mixed2);
    std::cout << "   After drop_last<2>: ";
    print_tuple(mixed_drop_last);
    std::cout << "\n\n";

    // Example 5: Drop zero elements
    std::cout << "5. Drop zero elements (identity operation):\n";
    auto tuple3 = std::make_tuple(10, 20, 30);
    std::cout << "   Original: ";
    print_tuple(tuple3);
    std::cout << "\n";

    auto no_drop = fxt::mdrop<0>(tuple3);
    std::cout << "   After drop<0>: ";
    print_tuple(no_drop);
    std::cout << "\n\n";

    // Example 6: Drop all elements
    std::cout << "6. Drop all elements (empty tuple):\n";
    auto tuple4 = std::make_tuple(1, 2, 3);
    std::cout << "   Original: ";
    print_tuple(tuple4);
    std::cout << "\n";

    auto all_dropped = fxt::mdrop<3>(tuple4);
    std::cout << "   After drop<3>: ";
    print_tuple(all_dropped);
    std::cout << " (empty tuple)\n\n";

    // Example 7: Combining drop and drop_last
    std::cout << "7. Combining drop and drop_last:\n";
    auto tuple5 = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original: ";
    print_tuple(tuple5);
    std::cout << "\n";

    auto first_drop = fxt::mdrop<2>(tuple5);
    std::cout << "   After drop<2>: ";
    print_tuple(first_drop);
    std::cout << "\n";

    auto both_dropped = fxt::mdrop_last<3>(first_drop);
    std::cout << "   Then drop_last<3>: ";
    print_tuple(both_dropped);
    std::cout << "\n\n";

    // Example 8: Working with strings
    std::cout << "8. Working with strings:\n";
    auto strings = std::make_tuple(
        std::string("first"),
        std::string("second"),
        std::string("third"),
        std::string("fourth"),
        std::string("fifth")
    );
    std::cout << "   Original: ";
    print_tuple(strings);
    std::cout << "\n";

    auto dropped_strings = fxt::mdrop<1>(strings);
    std::cout << "   After drop<1>: ";
    print_tuple(dropped_strings);
    std::cout << "\n";

    auto final_strings = fxt::mdrop_last<1>(dropped_strings);
    std::cout << "   Then drop_last<1>: ";
    print_tuple(final_strings);
    std::cout << "\n\n";

    // Example 9: Using with rvalues
    std::cout << "9. Using with temporary (rvalue) tuples:\n";
    auto result = fxt::mdrop<2>(std::make_tuple(100, 200, 300, 400, 500));
    std::cout << "   drop<2> from temporary: ";
    print_tuple(result);
    std::cout << "\n";

    auto result2 = fxt::mdrop_last<2>(std::make_tuple(100, 200, 300, 400, 500));
    std::cout << "   drop_last<2> from temporary: ";
    print_tuple(result2);
    std::cout << "\n\n";

    // Example 10: Extracting middle elements
    std::cout << "10. Extracting middle elements from a tuple:\n";
    auto data = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "   Original: ";
    print_tuple(data);
    std::cout << "\n";

    // Drop first 2 elements, then drop last 2 elements
    auto middle = fxt::mdrop_last<2>(fxt::mdrop<2>(data));
    std::cout << "   Middle elements (drop<2> then drop_last<2>): ";
    print_tuple(middle);
    std::cout << "\n\n";

    // Example 11: Practical use case - parsing a protocol message
    std::cout << "11. Practical use case - parsing a message:\n";
    // Simulating a message: [header, version, type, data1, data2, data3, checksum]
    auto message = std::make_tuple(
        std::string("HDR"),
        1,
        std::string("CMD"),
        42,
        3.14,
        std::string("payload"),
        0xABCD
    );
    std::cout << "   Full message: ";
    print_tuple(message);
    std::cout << "\n";

    // Extract payload by dropping header fields and checksum
    auto without_header = fxt::mdrop<3>(message);  // Skip header, version, type
    auto payload = fxt::mdrop_last<1>(without_header);  // Skip checksum
    std::cout << "   Extracted payload (data only): ";
    print_tuple(payload);
    std::cout << "\n\n";

    // Example 12: Large tuple manipulation
    std::cout << "12. Working with larger tuples:\n";
    auto large = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    std::cout << "   Original (15 elements): ";
    print_tuple(large);
    std::cout << "\n";

    auto trimmed = fxt::mdrop<5>(large);
    std::cout << "   After drop<5>: ";
    print_tuple(trimmed);
    std::cout << "\n";

    auto final = fxt::mdrop_last<5>(trimmed);
    std::cout << "   Then drop_last<5>: ";
    print_tuple(final);
    std::cout << "\n\n";

    // ===== NEW EXAMPLES: fxt::tuple and fxt::flat_tuple with drop/drop_last =====

    std::cout << "=== Advanced Examples with fxt::tuple and fxt::flat_tuple ===\n\n";

    // Example 13: Using fxt::mdrop and fxt::mdrop_last with fxt::flat_tuple
    std::cout << "13. Using mdrop and mdrop_last with fxt::flat_tuple:\n";
    auto ft1 = fxt::make_flat_tuple(10, 20, 30, 40, 50);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft1);
    std::cout << "\n";

    auto ft1_drop = fxt::mdrop<2>(ft1);
    std::cout << "   After mdrop<2>: ";
    print_flat_tuple(ft1_drop);
    std::cout << "\n";

    auto ft1_drop_last = fxt::mdrop_last<2>(ft1);
    std::cout << "   After mdrop_last<2>: ";
    print_flat_tuple(ft1_drop_last);
    std::cout << "\n\n";

    // Example 14: Using fxt::drop (direct call) with fxt::tuple
    std::cout << "14. Using fxt::drop (direct call) with fxt::tuple:\n";
    auto t1 = fxt::make_tuple(1, 2.0, std::string("three"), 'f', true);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t1);
    std::cout << "\n";

    auto t1_drop = fxt::drop<2>(t1);
    std::cout << "   After fxt::drop<2>(t1): ";
    print_tuple(t1_drop);
    std::cout << "\n\n";

    // Example 15: Using fxt::drop_last (direct call) with fxt::tuple
    std::cout << "15. Using fxt::drop_last (direct call) with fxt::tuple:\n";
    auto t2 = fxt::make_tuple(10, 20, 30, 40, 50);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t2);
    std::cout << "\n";

    auto t2_drop_last = fxt::drop_last<2>(t2);
    std::cout << "   After fxt::drop_last<2>(t2): ";
    print_tuple(t2_drop_last);
    std::cout << "\n\n";

    // Example 16: Using fxt::drop (direct call) with fxt::flat_tuple
    std::cout << "16. Using fxt::drop (direct call) with fxt::flat_tuple:\n";
    auto ft2 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft2);
    std::cout << "\n";

    auto ft2_drop = fxt::drop<2>(ft2);
    std::cout << "   After fxt::drop<2>(ft2): ";
    print_flat_tuple(ft2_drop);
    std::cout << "\n\n";

    // Example 17: Using fxt::drop_last (direct call) with fxt::flat_tuple
    std::cout << "17. Using fxt::drop_last (direct call) with fxt::flat_tuple:\n";
    auto ft3 = fxt::make_flat_tuple(100, 200, 300, 400, 500);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft3);
    std::cout << "\n";

    auto ft3_drop_last = fxt::drop_last<2>(ft3);
    std::cout << "   After fxt::drop_last<2>(ft3): ";
    print_flat_tuple(ft3_drop_last);
    std::cout << "\n\n";

    // Example 18: Using fxt::drop with pipe operator on fxt::tuple
    std::cout << "18. Using fxt::drop with pipe operator on fxt::tuple:\n";
    auto t3 = fxt::make_tuple(1, 2, 3, 4, 5, 6);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t3);
    std::cout << "\n";

    auto t3_piped = t3 | fxt::drop<2>();
    std::cout << "   After t3 | fxt::drop<2>(): ";
    print_tuple(t3_piped);
    std::cout << "\n\n";

    // Example 19: Using fxt::drop_last with pipe operator on fxt::tuple
    std::cout << "19. Using fxt::drop_last with pipe operator on fxt::tuple:\n";
    auto t4 = fxt::make_tuple(10, 20, 30, 40, 50, 60);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t4);
    std::cout << "\n";

    auto t4_piped = t4 | fxt::drop_last<2>();
    std::cout << "   After t4 | fxt::drop_last<2>(): ";
    print_tuple(t4_piped);
    std::cout << "\n\n";

    // Example 20: Using fxt::drop with pipe operator on fxt::flat_tuple
    std::cout << "20. Using fxt::drop with pipe operator on fxt::flat_tuple:\n";
    auto ft4 = fxt::make_flat_tuple(1.1, 2.2, 3.3, 4.4, 5.5, 6.6);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft4);
    std::cout << "\n";

    auto ft4_piped = ft4 | fxt::drop<2>();
    std::cout << "   After ft4 | fxt::drop<2>(): ";
    print_flat_tuple(ft4_piped);
    std::cout << "\n\n";

    // Example 21: Using fxt::drop_last with pipe operator on fxt::flat_tuple
    std::cout << "21. Using fxt::drop_last with pipe operator on fxt::flat_tuple:\n";
    auto ft5 = fxt::make_flat_tuple(11, 22, 33, 44, 55, 66);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft5);
    std::cout << "\n";

    auto ft5_piped = ft5 | fxt::drop_last<2>();
    std::cout << "   After ft5 | fxt::drop_last<2>(): ";
    print_flat_tuple(ft5_piped);
    std::cout << "\n\n";

    // Example 22: Chaining pipe operators with fxt::tuple
    std::cout << "22. Chaining pipe operators with fxt::tuple:\n";
    auto t5 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t5);
    std::cout << "\n";

    auto t5_chained = t5 | fxt::drop<2>() | fxt::drop_last<3>();
    std::cout << "   After t5 | drop<2>() | drop_last<3>(): ";
    print_tuple(t5_chained);
    std::cout << "\n\n";

    // Example 23: Chaining pipe operators with fxt::flat_tuple
    std::cout << "23. Chaining pipe operators with fxt::flat_tuple:\n";
    auto ft6 = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft6);
    std::cout << "\n";

    auto ft6_chained = ft6 | fxt::drop<2>() | fxt::drop_last<3>();
    std::cout << "   After ft6 | drop<2>() | drop_last<3>(): ";
    print_flat_tuple(ft6_chained);
    std::cout << "\n\n";

    // Example 24: Mixed types with fxt::flat_tuple and pipe operators
    std::cout << "24. Mixed types with fxt::flat_tuple and pipe operators:\n";
    auto ft7 = fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'x', true, 99.9f);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft7);
    std::cout << "\n";

    auto ft7_result = ft7 | fxt::drop<1>() | fxt::drop_last<1>();
    std::cout << "   After drop<1>() | drop_last<1>(): ";
    print_flat_tuple(ft7_result);
    std::cout << "\n\n";

    // Example 25: Comparing mdrop vs drop with fxt::tuple
    std::cout << "25. Comparing mdrop vs drop (direct call) with fxt::tuple:\n";
    auto t6 = fxt::make_tuple(5, 10, 15, 20, 25);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t6);
    std::cout << "\n";

    auto t6_mdrop = fxt::mdrop<2>(t6);
    std::cout << "   Using fxt::mdrop<2>(t6): ";
    print_tuple(t6_mdrop);
    std::cout << "\n";

    auto t6_drop = fxt::drop<2>(t6);
    std::cout << "   Using fxt::drop<2>(t6): ";
    print_tuple(t6_drop);
    std::cout << "   (both produce same result)\n\n";

    // Example 26: Comparing mdrop_last vs drop_last with fxt::flat_tuple
    std::cout << "26. Comparing mdrop_last vs drop_last with fxt::flat_tuple:\n";
    auto ft8 = fxt::make_flat_tuple(5.0, 10.0, 15.0, 20.0, 25.0);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft8);
    std::cout << "\n";

    auto ft8_mdrop_last = fxt::mdrop_last<2>(ft8);
    std::cout << "   Using fxt::mdrop_last<2>(ft8): ";
    print_flat_tuple(ft8_mdrop_last);
    std::cout << "\n";

    auto ft8_drop_last = fxt::drop_last<2>(ft8);
    std::cout << "   Using fxt::drop_last<2>(ft8): ";
    print_flat_tuple(ft8_drop_last);
    std::cout << "   (both produce same result)\n\n";

    // Example 27: Using drop with rvalue fxt::tuple and pipe operator
    std::cout << "27. Using drop with rvalue fxt::tuple and pipe operator:\n";
    auto t7_rvalue_result = fxt::make_tuple(100, 200, 300, 400, 500) | fxt::drop<2>();
    std::cout << "   make_tuple(100,200,300,400,500) | drop<2>(): ";
    print_tuple(t7_rvalue_result);
    std::cout << "\n\n";

    // Example 28: Using drop with rvalue fxt::flat_tuple and pipe operator
    std::cout << "28. Using drop with rvalue fxt::flat_tuple and pipe operator:\n";
    auto ft9_rvalue_result = fxt::make_flat_tuple(100, 200, 300, 400, 500) | fxt::drop<2>();
    std::cout << "   make_flat_tuple(100,200,300,400,500) | drop<2>(): ";
    print_flat_tuple(ft9_rvalue_result);
    std::cout << "\n\n";

    // Example 29: Complex chaining with mixed operations on fxt::tuple
    std::cout << "29. Complex chaining - extracting middle section of fxt::tuple:\n";
    auto t8 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9);
    std::cout << "   Original fxt::tuple: ";
    print_tuple(t8);
    std::cout << "\n";

    auto t8_middle = t8 | fxt::drop<3>() | fxt::drop_last<3>();
    std::cout << "   Middle section (drop<3> | drop_last<3>): ";
    print_tuple(t8_middle);
    std::cout << "\n\n";

    // Example 30: Complex chaining with mixed operations on fxt::flat_tuple
    std::cout << "30. Complex chaining - extracting middle section of fxt::flat_tuple:\n";
    auto ft10 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0);
    std::cout << "   Original flat_tuple: ";
    print_flat_tuple(ft10);
    std::cout << "\n";

    auto ft10_middle = ft10 | fxt::drop<3>() | fxt::drop_last<3>();
    std::cout << "   Middle section (drop<3> | drop_last<3>): ";
    print_flat_tuple(ft10_middle);
    std::cout << "\n\n";

    std::cout << "=== Demo Complete ===\n";

    return 0;
}

