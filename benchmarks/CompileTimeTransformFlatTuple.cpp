#include "../include/fxt.hpp"

int main() {
    // Test transformation patterns for fxt::flat_tuple
    auto t1 = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    // Multiple transformations
    auto t2 = fxt::transform_tuple([](auto x) { return x * 2; }, t1);
    auto t3 = fxt::transform_tuple([](auto x) { return x + 1; }, t2);
    auto t4 = fxt::transform_tuple([](auto x) { return x * x; }, t3);
    auto t5 = fxt::transform_tuple([](auto x) { return x - 5; }, t4);

    // Chain transformations
    auto t6 = t1
        | fxt::transform_tuple([](auto x) { return x * 3; })
        | fxt::transform_tuple([](auto x) { return x + 10; })
        | fxt::transform_tuple([](auto x) { return x / 2; });

    // Complex lambda
    auto t7 = fxt::transform_tuple([](auto x) {
        return x * 2 + (x % 2 == 0 ? 1 : -1);
    }, t1);

    return fxt::get<0>(t7);
}

