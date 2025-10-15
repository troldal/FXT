

#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    const fxt::lazy val = [] {
        // throw std::runtime_error("Error");
        return 42;
    };

    const int a = val;

    std::cout << a << std::endl;

    return 0;
}