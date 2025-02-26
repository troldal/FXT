//
// Created by kenne on 25/02/2025.
//

#include <fxt.hpp>
#include <iostream>

int main()
{

    using namespace std::literals;

    auto initPipeline = []() -> std::expected<std::tuple<>, std::string> { return std::tuple<> {}; };

    auto result = initPipeline()
        | fxt::append(42)
        | fxt::append(3.14)
        //| fxt::transform([](auto&& t) { return std::get<0>(t) * std::get<1>(t); })
        | fxt::apply([](int i, auto d) { return i * d; })
        | fxt::select<2>();
        //| fxt::value_or(0);

    //std::cout << result << std::endl;

    return 0;
}