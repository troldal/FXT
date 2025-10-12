#include "../include/fxt.hpp"

int main() {
    // Test combined operations for fxt::flat_tuple
    auto t1 = fxt::make_flat_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    // Complex pipeline
    auto result1 = t1
        | fxt::transform_tuple([](auto x) { return x * 2; })
        | fxt::take<8>
        | fxt::transform_tuple([](auto x) { return x + 1; })
        | fxt::drop<2>
        | fxt::transform_tuple([](auto x) { return x * x; });

    // Append and transform
    auto t2 = fxt::tuple_append(t1, 11);
    auto t3 = fxt::tuple_append(t2, 12);
    auto t4 = fxt::transform_tuple([](auto x) { return x * 3; }, t3);

    // Select and apply
    auto selected = fxt::select<0, 2, 4, 6, 8>(t1);
    auto applied = fxt::apply([](auto... args) {
        return (args + ...);
    }, selected);

    // Multiple nested operations
    auto complex = fxt::make_flat_tuple(
        fxt::make_flat_tuple(1, 2) | fxt::transform_tuple([](auto x) { return x * 2; }),
        fxt::make_flat_tuple(3, 4) | fxt::transform_tuple([](auto x) { return x + 1; }),
        fxt::make_flat_tuple(5, 6) | fxt::transform_tuple([](auto x) { return x - 1; })
    );

    return applied + fxt::get<0>(result1);
}
#include "../include/fxt.hpp"

int main() {
    // Test various creation patterns for fxt::tuple
    auto t1 = fxt::make_tuple(1, 2, 3, 4, 5);
    auto t2 = fxt::make_tuple(1.0, 2.0, 3.0, 4.0, 5.0);
    auto t3 = fxt::make_tuple("hello", 42, 3.14, true, 'x');
    auto t4 = fxt::make_tuple(t1, t2, t3);
    auto t5 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    auto t6 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    auto t7 = fxt::make_tuple(t5, t6);

    // Nested tuples
    auto t8 = fxt::make_tuple(
        fxt::make_tuple(1, 2),
        fxt::make_tuple(3, 4),
        fxt::make_tuple(5, 6)
    );

    // Use to prevent optimization
    return fxt::get<0>(t1) + fxt::get<0>(t5);
}

