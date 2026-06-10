//
// Created by kenne on 21-10-2025.
//

#pragma once

#include "../concepts/IsVariant.hpp"
#include <variant>

namespace fxt
{
    /**
     * @brief Check if a variant currently holds a specific alternative type
     *
     * This function is an alias for std::holds_alternative, providing a consistent
     * interface within the fxt namespace. It checks whether the variant currently
     * holds the specified type T.
     *
     * @tparam T The alternative type to check for
     * @tparam TVariant The variant type (deduced)
     * @param variant The variant to check
     * @return true if the variant currently holds the alternative T, false otherwise
     *
     * @code
     * fxt::variant<int, double, std::string> v{42};
     * bool has_int = fxt::holds_alternative<int>(v);        // Returns true
     * bool has_double = fxt::holds_alternative<double>(v);  // Returns false
     *
     * v = std::string{"hello"};
     * bool has_string = fxt::holds_alternative<std::string>(v);  // Returns true
     * @endcode
     */
    // TODO: COMPLETENESS — unlike fxt::index()/fxt::visit()/fxt::when(), there is no
    //       curried/pipe form (`v | fxt::holds_alternative<T>()`) and no monadic
    //       counterpart (mholds_alternative); every other variant operation provides both.
    //       Also missing the FXT license banner.
    template<typename T, typename TVariant>
        requires fxt::variant_like<std::remove_cvref_t<TVariant>>
    constexpr bool holds_alternative(TVariant&& variant)
    {
        return std::holds_alternative<T>(std::forward<TVariant>(variant));
    }
}
