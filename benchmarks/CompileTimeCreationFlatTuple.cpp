#include "../include/fxt.hpp"

int main() {
    // Test various creation patterns for fxt::flat_tuple
    auto t1 = fxt::make_flat_tuple(1, 2, 3, 4, 5);
    auto t2 = fxt::make_flat_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    auto t3 = fxt::make_flat_tuple("hello", 42, 3.14, true, 'x');
    auto t4 = fxt::make_flat_tuple(t1, t2, t3);
    auto t5 = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    auto t6 = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    auto t7 = fxt::make_flat_tuple(t5, t6);

    // Nested tuples
    auto t8 = fxt::make_flat_tuple(
        fxt::make_flat_tuple(1, 2),
        fxt::make_flat_tuple(3, 4),
        fxt::make_flat_tuple(5, 6)
    );

    // Use to prevent optimization
    return fxt::get<0>(t1) + fxt::get<0>(t5);
}

