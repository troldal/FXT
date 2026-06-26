#include "../include/fxt.hpp"

int main() {
    // Test select/take/drop patterns for fxt::tuple
    auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);

    // Select various elements
    auto s1 = fxt::select<0, 2, 4, 6, 8>(t);
    auto s2 = fxt::select<1, 3, 5, 7, 9>(t);
    auto s3 = fxt::select<0, 1, 2>(t);
    auto s4 = fxt::select<10, 11, 12, 13, 14>(t);

    // Take operations
    auto tk1 = fxt::tuple_take<5>(t);
    auto tk2 = fxt::tuple_take<10>(t);
    auto tk3 = fxt::tuple_take<3>(t);

    // Drop operations
    auto dr1 = fxt::tuple_drop<5>(t);
    auto dr2 = fxt::tuple_drop<10>(t);
    auto dr3 = fxt::tuple_drop<3>(t);

    // Combined operations
    auto combined = fxt::tuple_take<10>(t) | fxt::tuple_transform([](auto x) { return x * 2; });

    return fxt::get<0>(s1) + fxt::get<0>(tk1) + fxt::get<0>(dr1);
}

