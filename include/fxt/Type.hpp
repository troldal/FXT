//
// Created by kenne on 27/02/2025.
//

#pragma once

namespace fxt
{
    // Primary template - behaves like the original Type class
    template<typename TType, typename... TArgs>
    struct Type
    {
        using type = TType;
        Type() = default;
        Type(TType&&) : Type() {}
    };

    // Partial specialization - adds value storage and access (like TypeValue)
    template<typename TType, typename TValue>
    struct Type<TType, TValue>
    {
        using type = TType;
        using value_type = TValue;

        explicit Type(const TValue& value) : m_value(value) {}
        explicit Type(TValue&& value) : m_value(std::move(value)) {}
        TValue value() const { return m_value; }

    private:
        TValue m_value;
    };

}    // namespace fxt