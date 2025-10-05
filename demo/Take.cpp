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
    std::cout << "=== fxt::take and fxt::take_last Demo ===\n\n";

    // Example 1: Basic take - extracting first N elements
    std::cout << "1. Basic take - extracting first N elements:\n";
    auto tuple1 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple1);
    std::cout << "\n";

    auto take2 = fxt::take<2>(tuple1);
    std::cout << "   After take<2>: ";
    print_tuple(take2);
    std::cout << "\n\n";

    // Example 2: Basic take_last - extracting last N elements
    std::cout << "2. Basic take_last - extracting last N elements:\n";
    auto tuple2 = std::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Original: ";
    print_tuple(tuple2);
    std::cout << "\n";

    auto take_last2 = fxt::take_last<2>(tuple2);
    std::cout << "   After take_last<2>: ";
    print_tuple(take_last2);
    std::cout << "\n\n";

    // Example 3: Take with mixed types
    std::cout << "3. Take with mixed types:\n";
    auto mixed = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed);
    std::cout << "\n";

    auto mixed_take = fxt::take<3>(mixed);
    std::cout << "   After take<3>: ";
    print_tuple(mixed_take);
    std::cout << "\n\n";

    // Example 4: Take_last with mixed types
    std::cout << "4. Take_last with mixed types:\n";
    auto mixed2 = std::make_tuple(42, 3.14, std::string("hello"), 'x', true);
    std::cout << "   Original: ";
    print_tuple(mixed2);
    std::cout << "\n";

    auto mixed_take_last = fxt::take_last<3>(mixed2);
    std::cout << "   After take_last<3>: ";
    print_tuple(mixed_take_last);
    std::cout << "\n\n";

    // Example 5: Take zero elements (empty tuple)
    std::cout << "5. Take zero elements (empty tuple):\n";
    auto tuple3 = std::make_tuple(10, 20, 30);
    std::cout << "   Original: ";
    print_tuple(tuple3);
    std::cout << "\n";

    auto no_take = fxt::take<0>(tuple3);
    std::cout << "   After take<0>: ";
    print_tuple(no_take);
    std::cout << " (empty tuple)\n\n";

    // Example 6: Take all elements (identity operation)
    std::cout << "6. Take all elements (identity operation):\n";
    auto tuple4 = std::make_tuple(1, 2, 3);
    std::cout << "   Original: ";
    print_tuple(tuple4);
    std::cout << "\n";

    auto all_taken = fxt::take<3>(tuple4);
    std::cout << "   After take<3>: ";
    print_tuple(all_taken);
    std::cout << "\n\n";

    // Example 7: Extracting middle elements using take and take_last
    std::cout << "7. Extracting middle elements:\n";
    auto tuple5 = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "   Original: ";
    print_tuple(tuple5);
    std::cout << "\n";

    auto first_take = fxt::take<7>(tuple5);
    std::cout << "   After take<7>: ";
    print_tuple(first_take);
    std::cout << "\n";

    auto middle = fxt::take_last<5>(first_take);
    std::cout << "   Then take_last<5>: ";
    print_tuple(middle);
    std::cout << " (elements 3-7)\n\n";

    // Example 8: Alternative middle extraction
    std::cout << "8. Alternative middle extraction:\n";
    auto tuple6 = std::make_tuple(1, 2, 3, 4, 5, 6, 7);
    std::cout << "   Original: ";
    print_tuple(tuple6);
    std::cout << "\n";

    auto last_take = fxt::take_last<5>(tuple6);
    std::cout << "   After take_last<5>: ";
    print_tuple(last_take);
    std::cout << "\n";

    auto middle2 = fxt::take<3>(last_take);
    std::cout << "   Then take<3>: ";
    print_tuple(middle2);
    std::cout << " (elements 3-5)\n\n";

    // Example 9: Working with strings
    std::cout << "9. Working with strings:\n";
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

    auto first_two = fxt::take<2>(strings);
    std::cout << "   First two: ";
    print_tuple(first_two);
    std::cout << "\n";

    auto last_two = fxt::take_last<2>(strings);
    std::cout << "   Last two: ";
    print_tuple(last_two);
    std::cout << "\n\n";

    // Example 10: Using with rvalues (temporary tuples)
    std::cout << "10. Using with temporary (rvalue) tuples:\n";
    auto result1 = fxt::take<3>(std::make_tuple(100, 200, 300, 400, 500));
    std::cout << "   take<3> from temporary: ";
    print_tuple(result1);
    std::cout << "\n";

    auto result2 = fxt::take_last<3>(std::make_tuple(100, 200, 300, 400, 500));
    std::cout << "   take_last<3> from temporary: ";
    print_tuple(result2);
    std::cout << "\n\n";

    // Example 11: Partitioning a tuple
    std::cout << "11. Partitioning a tuple:\n";
    auto data = std::make_tuple(10, 20, 30, 40, 50, 60, 70);
    std::cout << "   Original: ";
    print_tuple(data);
    std::cout << "\n";

    auto first_part = fxt::take<4>(data);
    auto last_part = fxt::take_last<3>(data);
    std::cout << "   First part (take<4>): ";
    print_tuple(first_part);
    std::cout << "\n";
    std::cout << "   Last part (take_last<3>): ";
    print_tuple(last_part);
    std::cout << "\n\n";

    // Example 12: Practical use case - extracting data fields
    std::cout << "12. Practical use case - extracting fields:\n";
    // Simulating a data record: [id, name, age, email, salary, department, hire_date]
    auto record = std::make_tuple(
        12345,
        std::string("John Doe"),
        35,
        std::string("john@example.com"),
        75000.0,
        std::string("Engineering"),
        std::string("2020-01-15")
    );
    std::cout << "   Full record: ";
    print_tuple(record);
    std::cout << "\n";

    // Extract only basic info (first 3 fields)
    auto basic_info = fxt::take<3>(record);
    std::cout << "   Basic info (take<3>): ";
    print_tuple(basic_info);
    std::cout << "\n";

    // Extract employment info (last 3 fields)
    auto employment_info = fxt::take_last<3>(record);
    std::cout << "   Employment info (take_last<3>): ";
    print_tuple(employment_info);
    std::cout << "\n\n";

    // Example 13: Single element extraction
    std::cout << "13. Single element extraction:\n";
    auto tuple7 = std::make_tuple(100, 200, 300, 400);
    std::cout << "   Original: ";
    print_tuple(tuple7);
    std::cout << "\n";

    auto first = fxt::take<1>(tuple7);
    std::cout << "   First element (take<1>): ";
    print_tuple(first);
    std::cout << "\n";

    auto last = fxt::take_last<1>(tuple7);
    std::cout << "   Last element (take_last<1>): ";
    print_tuple(last);
    std::cout << "\n\n";

    // Example 14: Large tuple manipulation
    std::cout << "14. Working with larger tuples:\n";
    auto large = std::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    std::cout << "   Original (15 elements): ";
    print_tuple(large);
    std::cout << "\n";

    auto first_half = fxt::take<8>(large);
    std::cout << "   First half (take<8>): ";
    print_tuple(first_half);
    std::cout << "\n";

    auto last_half = fxt::take_last<8>(large);
    std::cout << "   Last half (take_last<8>): ";
    print_tuple(last_half);
    std::cout << "\n";

    return 0;
}

