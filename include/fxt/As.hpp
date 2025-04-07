//
// Created by kenne on 12/02/2025.
//

#pragma once

#include "impl/utils/TupleAppend.hpp"
#include <string>
#include "Overload.hpp"

namespace fxt
{
    // Underlying function template that wraps a std::string in an object of type T.
    template<typename T>
    T as_func(auto&& s)
    {
        return T(s);
    }

    // Function object template that wraps the as_func function template.
    // This structure lets us specify the desired type inline using as<T>.
    template<typename T>
    struct AsWrapper
    {
        auto operator()() const
        {
            return [](const auto& s) { return s_overloads(s); };
        }

    private:

        static constexpr auto s_overloads = fxt::overload{
            [](const auto& s) { return as_func<T>(s); },
            []<typename TValue, typename TError>(const fxt::expected<TValue, TError>& s) { return s.transform([](const auto& v) { return as_func<T>(v); }); }
        };
    };

    // A variable template so you can write as<MyType> in the chain.
    template<typename T>
    inline constexpr AsWrapper<T> as = {};


}    // namespace fxt