#include <fxt/tuples/Tuple.hpp>
#include <fxt/tuples/Transform.hpp>
#include <fxt/tuples/FlatTuple.hpp>
#include <fxt/monads/Expected.hpp>
#include <fxt/monads/Optional.hpp>
#include <iostream>
#include <string>

int main()
{
    // Test transform_tuple with fxt::tuple
    auto t = fxt::make_tuple(1, 2, 3);
    auto result1 = fxt::transform_tuple([](auto x) { return x * 2; }, t);
    std::cout << "transform_tuple direct: " << fxt::get<0>(result1) << ", "
              << fxt::get<1>(result1) << ", " << fxt::get<2>(result1) << "\n";

    // Test transform_tuple with pipe operator
    auto result2 = t | fxt::transform_tuple([](auto x) { return x * 3; });
    std::cout << "transform_tuple piped: " << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", " << fxt::get<2>(result2) << "\n";

    // Test transform_tuple with fxt::flat_tuple
    auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
    auto result3 = ft | fxt::transform_tuple([](auto x) { return x + 1.0; });
    std::cout << "transform_tuple flat_tuple: " << fxt::get<0>(result3) << ", "
              << fxt::get<1>(result3) << ", " << fxt::get<2>(result3) << "\n";

    // Test mtransform_tuple with fxt::expected containing fxt::tuple
    struct Error { int code; };
    auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
    auto result4 = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);
    if (result4.has_value()) {
        auto& val = result4.value();
        std::cout << "mtransform_tuple expected direct: " << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", " << fxt::get<2>(val) << "\n";
    }

    // Test mtransform_tuple with pipe operator
    auto result5 = exp | fxt::mtransform_tuple([](auto x) { return x * 3; });
    if (result5.has_value()) {
        auto& val = result5.value();
        std::cout << "mtransform_tuple expected piped: " << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", " << fxt::get<2>(val) << "\n";
    }

    // Test mtransform_tuple with fxt::optional containing fxt::flat_tuple
    auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
    auto result6 = opt | fxt::mtransform_tuple([](auto x) { return x + 5.0; });
    if (result6.has_value()) {
        auto& val = result6.value();
        std::cout << "mtransform_tuple optional flat_tuple: " << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << "\n";
    }

    // Test error propagation
    auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{42}}};
    auto result7 = exp_err | fxt::mtransform_tuple([](auto x) { return x * 2; });
    std::cout << "mtransform_tuple error propagation: "
              << (result7.has_value() ? "has value" : "has error") << "\n";

    // Test none propagation
    auto opt_none = fxt::optional<fxt::tuple<int, int>>{};
    auto result8 = opt_none | fxt::mtransform_tuple([](auto x) { return x * 2; });
    std::cout << "mtransform_tuple none propagation: "
              << (result8.has_value() ? "has value" : "nullopt") << "\n";

    return 0;
}

