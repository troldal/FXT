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

int main() {
    std::cout << "=== fxt::drop and fxt::drop_last Demo ===\n\n";

    // Example 1: Basic drop - removing elements from the beginning
    std::cout << "1. Basic drop - removing first N elements:\n";
    auto tuple1 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple1);
    std::cout << "\n";

    auto drop1 = fxt::drop<2>(tuple1);
    std::cout << "   After drop<2>: ";
    print_tuple(drop1);
    std::cout << "\n\n";

    // Example 2: Basic drop_last - removing elements from the end
    std::cout << "2. Basic drop_last - removing last N elements:\n";
    auto tuple2 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple2);
    std::cout << "\n";

    auto drop_last2 = fxt::drop_last<2>(tuple2);
    std::cout << "   After drop_last<2>: ";
    print_tuple(drop_last2);
    std::cout << "\n\n";

    // Example 3: Drop with mixed types
    std::cout << "3. Drop with mixed types:\n";
    auto mixed = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed);
    std::cout << "\n";

    auto mixed_drop = fxt::drop<2>(mixed);
    std::cout << "   After drop<2>: ";
    print_tuple(mixed_drop);
    std::cout << "\n\n";

    // Example 4: Drop_last with mixed types
    std::cout << "4. Drop_last with mixed types:\n";
    auto mixed2 = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed2);
    std::cout << "\n";

    auto mixed_drop_last = fxt::drop_last<2>(mixed2);
    std::cout << "   After drop_last<2>: ";
    print_tuple(mixed_drop_last);
    std::cout << "\n\n";

    // Example 5: Drop zero elements
    std::cout << "5. Drop zero elements (identity operation):\n";
    auto tuple3 = std::make_tuple(10, 20, 30);
    std::cout << "   Original: ";
    print_tuple(tuple3);
    std::cout << "\n";

    auto no_drop = fxt::drop<0>(tuple3);
    std::cout << "   After drop<0>: ";
    print_tuple(no_drop);
    std::cout << "\n\n";

    // Example 6: Drop all elements
    std::cout << "6. Drop all elements (empty tuple):\n";
    auto tuple4 = std::make_tuple(1, 2, 3);
    std::cout << "   Original: ";
    print_tuple(tuple4);
    std::cout << "\n";

    auto all_dropped = fxt::drop<3>(tuple4);
    std::cout << "   After drop<3>: ";
    print_tuple(all_dropped);
    std::cout << " (empty tuple)\n\n";

    // Example 7: Combining drop and drop_last
    std::cout << "7. Combining drop and drop_last:\n";
    auto tuple5 = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original: ";
    print_tuple(tuple5);
    std::cout << "\n";

    auto first_drop = fxt::drop<2>(tuple5);
    std::cout << "   After drop<2>: ";
    print_tuple(first_drop);
    std::cout << "\n";

    auto both_dropped = fxt::drop_last<3>(first_drop);
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

    auto dropped_strings = fxt::drop<1>(strings);
    std::cout << "   After drop<1>: ";
    print_tuple(dropped_strings);
    std::cout << "\n";

    auto final_strings = fxt::drop_last<1>(dropped_strings);
    std::cout << "   Then drop_last<1>: ";
    print_tuple(final_strings);
    std::cout << "\n\n";

    // Example 9: Using with rvalues
    std::cout << "9. Using with temporary (rvalue) tuples:\n";
    auto result = fxt::drop<2>(std::make_tuple(100, 200, 300, 400, 500));
    std::cout << "   drop<2> from temporary: ";
    print_tuple(result);
    std::cout << "\n";

    auto result2 = fxt::drop_last<2>(std::make_tuple(100, 200, 300, 400, 500));
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
    auto middle = fxt::drop_last<2>(fxt::drop<2>(data));
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
    auto without_header = fxt::drop<3>(message);  // Skip header, version, type
    auto payload = fxt::drop_last<1>(without_header);  // Skip checksum
    std::cout << "   Extracted payload (data only): ";
    print_tuple(payload);
    std::cout << "\n\n";

    // Example 12: Large tuple manipulation
    std::cout << "12. Working with larger tuples:\n";
    auto large = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    std::cout << "   Original (15 elements): ";
    print_tuple(large);
    std::cout << "\n";

    auto trimmed = fxt::drop<5>(large);
    std::cout << "   After drop<5>: ";
    print_tuple(trimmed);
    std::cout << "\n";

    auto final = fxt::drop_last<5>(trimmed);
    std::cout << "   Then drop_last<5>: ";
    print_tuple(final);
    std::cout << "\n";

    return 0;
}

