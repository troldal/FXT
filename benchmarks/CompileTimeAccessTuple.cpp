#include "../include/fxt.hpp"

int main() {
    // Test element access patterns for fxt::tuple
    auto t = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20);

    // Access all elements
    auto v0 = fxt::get<0>(t);
    auto v1 = fxt::get<1>(t);
    auto v2 = fxt::get<2>(t);
    auto v3 = fxt::get<3>(t);
    auto v4 = fxt::get<4>(t);
    auto v5 = fxt::get<5>(t);
    auto v6 = fxt::get<6>(t);
    auto v7 = fxt::get<7>(t);
    auto v8 = fxt::get<8>(t);
    auto v9 = fxt::get<9>(t);
    auto v10 = fxt::get<10>(t);
    auto v11 = fxt::get<11>(t);
    auto v12 = fxt::get<12>(t);
    auto v13 = fxt::get<13>(t);
    auto v14 = fxt::get<14>(t);
    auto v15 = fxt::get<15>(t);
    auto v16 = fxt::get<16>(t);
    auto v17 = fxt::get<17>(t);
    auto v18 = fxt::get<18>(t);
    auto v19 = fxt::get<19>(t);

    // Multiple access patterns
    auto sum1 = v0 + v5 + v10 + v15;
    auto sum2 = v1 + v6 + v11 + v16;
    auto sum3 = v2 + v7 + v12 + v17;

    return sum1 + sum2 + sum3;
}

