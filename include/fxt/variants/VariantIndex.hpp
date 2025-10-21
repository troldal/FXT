//
// Created by kenne on 21-10-2025.
//

#pragma once

#include "../concepts/IsVariant.hpp"
namespace fxt
{
    /**
     * @brief Get the zero-based index of the currently held alternative in a variant
     *
     * This function provides two overloads:
     * 1. A direct call that takes a variant and returns its current index
     * 2. A nullary overload that returns a lambda, enabling use with the pipe operator
     *
     * The index corresponds to the position of the currently active alternative
     * within the variant's template parameter list.
     *
     * @tparam TVariant The variant type (deduced)
     * @param variant The variant to query
     * @return The zero-based index of the currently held alternative
     *
     * @code
     * // Direct usage
     * fxt::variant<int, double, std::string> v{42};
     * std::size_t idx = fxt::index(v);  // Returns 0
     *
     * v = 3.14;
     * idx = fxt::index(v);  // Returns 1
     *
     * // With pipe operator
     * auto idx2 = v | fxt::index();
     *
     * // In a pipeline
     * auto result = some_variant
     *             | fxt::transform_variant(some_operation)
     *             | fxt::index();
     * @endcode
     */
    template<typename TVariant>
        requires fxt::variant_like<std::remove_cvref_t<TVariant>>
    constexpr std::size_t index(TVariant&& variant)
    {
        return std::forward<TVariant>(variant).index();
    }

    /**
     * @brief Returns a lambda that gets the index of a variant (for use with pipe operator)
     *
     * This overload returns a lambda that can be used with the pipe operator,
     * enabling functional composition with variants.
     *
     * @return A lambda that takes a variant and returns its current index
     *
     * @code
     * fxt::variant<int, double, std::string> v{3.14};
     * auto idx = v | fxt::index();  // Returns 1
     * @endcode
     */
    inline constexpr auto index()
    {
        return []<typename TVariant>(TVariant&& variant)
            requires fxt::variant_like<std::remove_cvref_t<TVariant>>
        {
            return std::forward<TVariant>(variant).index();
        };
    }

    /**
     * @brief Get the index of a variant contained within a monad (optional or expected)
     *
     * This function extracts the variant index from within a monad container.
     * It uses the monad's transform operation to apply the index() function.
     *
     * @tparam TMonad The monad type containing a variant (deduced)
     * @param monad The monad containing a variant
     * @return A monad containing the variant's index
     *
     * @code
     * fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
     * auto idx_opt = fxt::mindex(opt);  // Returns fxt::optional<std::size_t> containing 1
     *
     * fxt::expected<fxt::variant<int, double>, std::string> exp{fxt::variant<int, double>{42}};
     * auto idx_exp = fxt::mindex(exp);  // Returns fxt::expected<std::size_t, std::string> containing 0
     * @endcode
     */
    template<typename TMonad>
        requires requires(TMonad&& monad) {
            { std::forward<TMonad>(monad).transform(index()) };
        }
    constexpr auto mindex(TMonad&& monad)
    {
        return std::forward<TMonad>(monad).transform(index());
    }

    /**
     * @brief Returns a lambda that gets the index of a variant in a monad (for use with pipe operator)
     *
     * This overload returns a lambda that can be used with the pipe operator,
     * enabling functional composition with monadic variants.
     *
     * @return A lambda that takes a monad containing a variant and returns a monad containing its index
     *
     * @code
     * fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
     * auto idx_opt = opt | fxt::mindex();  // Returns fxt::optional<std::size_t> containing 1
     * @endcode
     */
    inline constexpr auto mindex()
    {
        return []<typename TMonad>(TMonad&& monad)
            requires requires {
                { std::forward<TMonad>(monad).transform(index()) };
            }
        {
            return std::forward<TMonad>(monad).transform(index());
        };
    }
}