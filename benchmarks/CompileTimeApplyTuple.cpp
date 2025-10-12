#include "../include/fxt.hpp"

int main() {
    // Test apply patterns for fxt::tuple
    auto t1 = fxt::make_tuple(1, 2, 3);
    auto t2 = fxt::make_tuple(4, 5, 6, 7);
    auto t3 = fxt::make_tuple(8, 9, 10, 11, 12);

    // Apply with various arities
    auto r1 = fxt::apply([](auto a, auto b, auto c) {
        return a + b + c;
    }, t1);

    auto r2 = fxt::apply([](auto a, auto b, auto c, auto d) {
        return a * b + c * d;
    }, t2);

    auto r3 = fxt::apply([](auto a, auto b, auto c, auto d, auto e) {
        return a + b * c - d + e;
    }, t3);

    // Apply with more complex operations
    auto r4 = fxt::apply([](auto... args) {
        return (args + ...);
    }, t1);

    auto r5 = fxt::apply([](auto... args) {
        return (args * ...);
    }, t2);

    // Multiple applies
    auto r6 = fxt::apply([](auto a, auto b, auto c) {
        auto sum = a + b + c;
        auto prod = a * b * c;
        return fxt::make_tuple(sum, prod);
    }, t1);

    return r1 + r2 + r3;
}

