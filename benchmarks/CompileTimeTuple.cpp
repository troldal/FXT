#include "../include/fxt.hpp"

int main() {
    // Start with empty flat_tuple
    auto t0 = fxt::make_flat_tuple();

    // Gradually append 20 elements
    auto t1 = fxt::tuple_append(t0, 1);
    auto t2 = fxt::tuple_append(t1, 2);
    auto t3 = fxt::tuple_append(t2, 3);
    auto t4 = fxt::tuple_append(t3, 4);
    auto t5 = fxt::tuple_append(t4, 5);
    auto t6 = fxt::tuple_append(t5, 6);
    auto t7 = fxt::tuple_append(t6, 7);
    auto t8 = fxt::tuple_append(t7, 8);
    auto t9 = fxt::tuple_append(t8, 9);
    auto t10 = fxt::tuple_append(t9, 10);
    auto t11 = fxt::tuple_append(t10, 11);
    auto t12 = fxt::tuple_append(t11, 12);
    auto t13 = fxt::tuple_append(t12, 13);
    auto t14 = fxt::tuple_append(t13, 14);
    auto t15 = fxt::tuple_append(t14, 15);
    auto t16 = fxt::tuple_append(t15, 16);
    auto t17 = fxt::tuple_append(t16, 17);
    auto t18 = fxt::tuple_append(t17, 18);
    auto t19 = fxt::tuple_append(t18, 19);
    auto t20 = fxt::tuple_append(t19, 20);

    // Use the final tuple to prevent optimization
    return fxt::get<0>(t20);
}