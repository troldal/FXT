#include "../include/fxt.hpp"

int main() {
    // Test combined operations for fxt::tuple
    auto t1 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

    // Complex pipeline
    //auto result1 = t1
    //    | fxt::transform_tuple([](auto x) { return x * 2; })
    //    | fxt::tuple_take<8>
    //    | fxt::transform_tuple([](auto x) { return x + 1; })
    //    | fxt::tuple_drop<2>
    //    | fxt::transform_tuple([](auto x) { return x * x; });

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
    auto complex = fxt::make_tuple(
        fxt::make_tuple(1, 2) | fxt::transform_tuple([](auto x) { return x * 2; }),
        fxt::make_tuple(3, 4) | fxt::transform_tuple([](auto x) { return x + 1; }),
        fxt::make_tuple(5, 6) | fxt::transform_tuple([](auto x) { return x - 1; })
    );

    return applied + fxt::get<0>(t1);
}

