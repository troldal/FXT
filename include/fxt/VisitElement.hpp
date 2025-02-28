//
// Created by kenne on 27/02/2025.
//

#pragma once

#include <initializer_list>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <optional>

namespace fxt
{
    namespace impl
    {
        // Helper meta-function to compute the common type returned by the visitor
        // for all embedded tuples. For each inner tuple type T, we compute
        // decltype(f(tuple_tail(std::declval<T>()))), and then take the std::common_type_t
        // between all these types.
        template<typename Func, typename TupleOfTuples>
        struct visit_return_type;

        template<typename Func, typename... Tuples>
        struct visit_return_type<Func, std::tuple<Tuples...>>
        {
            using type = std::common_type_t<decltype(std::apply(std::declval<Func>(), fxt::drop<1>(std::declval<Tuples>())))...>;
        };

        template<typename Func, typename TupleOfTuples>
        using visit_result_t = typename visit_return_type<Func, TupleOfTuples>::type;
    }    // namespace impl
    // The visit function iterates over the tuple of tuples. For each inner tuple whose first element
    // (the key) matches the given key, it invokes the provided function with the tail of that tuple.
    // The return type is the common type of f invoked on the tails.
    template<typename Key, typename Func, typename TupleOfTuples>
    auto visit_element(const Key& key, Func&& f, const TupleOfTuples& tupleOfTuples) -> std::optional<impl::visit_result_t<Func, TupleOfTuples>>
    {
        using ResultType = impl::visit_result_t<Func, TupleOfTuples>;
        // bool       found = false;
        std::optional<ResultType> result {};    // default-constructed result; adjust if default construction doesn't make sense

        std::apply(
            [&](auto&&... innerTuple) {
                // Use an initializer list and a fold expression to iterate over the tuples.
                (void)std::initializer_list<int> {
                    ((std::get<0>(innerTuple) == key ? (result = std::apply(f, fxt::drop<1>(innerTuple)), 0) : 0), 0)...
                };
            },
            tupleOfTuples);

        // if (!found) {
        //     throw std::runtime_error("Key not found in tuple of tuples.");
        // }
        return result;
    }

}    // namespace fxt