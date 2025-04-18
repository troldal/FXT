//
// Created by kenne on 25/02/2025.
//

#pragma once



#include <type_traits>
#include "../../Expected.hpp"

namespace fxt::impl {

// template <typename T>
// struct is_specialization_of_std_expected : std::false_type {};
//
// template <typename T, typename E>
// struct is_specialization_of_std_expected<fxt::expected<T, E>> : std::true_type {};
//
// // Concept to check if a type is a specialization of std::expected
// template <typename T>
// concept IsExpected = is_specialization_of_std_expected<T>::value;

  template<typename T>
concept expected_like = requires {
    typename T::value_type;
    typename T::error_type;
    typename T::unexpected_type;
};

}