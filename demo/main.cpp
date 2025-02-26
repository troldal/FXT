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
        | fxt::get<2>()
        | fxt::value_or(0);


    auto f = [](int i, double d, std::string s) { std::cout << i << " " << d << " " << s << std::endl; };
    auto f2 = [f](auto && PH1, auto && PH2, auto && PH3) { f(std::forward<decltype(PH3)>(PH3), std::forward<decltype(PH2)>(PH2), std::forward<decltype(PH1)>(PH1)); };
    // f(42, 3.14, "Hello");

    auto f3 = fxt::curry(f2);
    f3("Hello")(3.14)(42);

    std::cout << result << std::endl;

    fxt::Failure failure = std::make_exception_ptr(std::runtime_error("Error"));
    auto exc = failure.exception();

    std::cout << failure << std::endl;

    try {
        std::rethrow_exception(exc);
    }
    catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    std::string msg = failure;
        std::cout << msg << std::endl;

    return 0;
}