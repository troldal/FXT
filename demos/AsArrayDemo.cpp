/*
    AsArrayDemo - Demonstration of fxt::as_array functionality
*/

#include <fxt.hpp>
#include <iostream>
#include <iomanip>

int main()
{
    std::cout << "=== FXT as_array Demo ===" << std::endl << std::endl;

    // Example 1: Convert fxt::tuple with mixed types to double array
    std::cout << "Example 1: fxt::tuple to std::array<double>" << std::endl;
    auto tpl1 = fxt::make_tuple(1, 2.5, 3);
    auto arr1 = fxt::as_array<double>(tpl1);

    std::cout << "Original tuple: ";
    fxt::apply([](auto... args) {
        ((std::cout << args << " "), ...);
    }, tpl1);
    std::cout << std::endl;

    std::cout << "As array: ";
    for (const auto& val : arr1) {
        std::cout << val << " ";
    }
    std::cout << std::endl << std::endl;

    // Example 2: Convert fxt::flat_tuple to int array
    std::cout << "Example 2: fxt::flat_tuple to std::array<int>" << std::endl;
    auto tpl2 = fxt::make_flat_tuple(1.1, 2.9, 3.5, 4.2);
    auto arr2 = fxt::as_array<int>(tpl2);

    std::cout << "Original flat_tuple: ";
    fxt::apply([](auto... args) {
        ((std::cout << args << " "), ...);
    }, tpl2);
    std::cout << std::endl;

    std::cout << "As array: ";
    for (const auto& val : arr2) {
        std::cout << val << " ";
    }
    std::cout << std::endl << std::endl;

    // Example 3: Curried version with pipe operator
    std::cout << "Example 3: Curried version with pipe operator" << std::endl;
    auto tpl3 = fxt::make_tuple(10, 20, 30, 40);
    auto arr3 = tpl3 | fxt::as_array<double>();

    std::cout << "Original tuple: ";
    fxt::apply([](auto... args) {
        ((std::cout << args << " "), ...);
    }, tpl3);
    std::cout << std::endl;

    std::cout << "Piped to array: ";
    for (const auto& val : arr3) {
        std::cout << val << " ";
    }
    std::cout << std::endl << std::endl;

    // Example 4: Chain with other operations
    std::cout << "Example 4: Chained operations" << std::endl;
    auto result = fxt::make_flat_tuple(1, 2, 3, 4, 5)
        | fxt::as_array<double>();

    std::cout << "Result array: ";
    for (const auto& val : result) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
    std::cout << "Array size: " << result.size() << std::endl;

    return 0;
}

