/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/


#pragma once

namespace fxt
{
    /**
     * @brief A utility for creating an overload set from multiple callable objects.
     *
     * The overload struct inherits from all provided callable types and brings their
     * operator() into scope, creating a single callable object that dispatches to the
     * appropriate lambda based on parameter types. This is particularly useful for
     * creating visitors for std::variant or pattern matching scenarios.
     *
     * @tparam Ts Variadic template parameter pack of callable types (typically lambdas)
     *
     * @section Usage
     * @code
     * auto visitor = fxt::overload{
     *     [](int i) { return std::to_string(i); },
     *     [](double d) { return std::to_string(d); },
     *     [](const std::string& s) { return s; }
     * };
     *
     * std::variant<int, double, std::string> var = 42;
     * auto result = std::visit(visitor, var);  // Calls the int overload
     *
     * // Can also be used inline with std::visit
     * std::visit(fxt::overload{
     *     [](int i) { std::cout << "Integer: " << i << "\n"; },
     *     [](double d) { std::cout << "Double: " << d << "\n"; },
     *     [](auto&& arg) { std::cout << "Other: " << arg << "\n"; }
     * }, var);
     * @endcode
     *
     * @note This is a common C++17 pattern that enables elegant pattern matching
     *       and is often used with std::visit for std::variant types.
     */
    template<class... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;
    };

    /**
     * @brief Deduction guide for the overload template.
     *
     * This deduction guide allows class template argument deduction (CTAD) to work
     * with the overload struct, enabling the compiler to automatically deduce the
     * template arguments from the constructor arguments.
     *
     * @tparam Ts The types of the callable objects passed to the constructor
     */
    template<class... Ts>
    overload(Ts...) -> overload<Ts...>;
}    // namespace fxt