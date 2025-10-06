//
// Created by kenne on 23/02/2025.
//

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