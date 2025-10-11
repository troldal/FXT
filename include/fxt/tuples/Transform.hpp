//
// Created by kenne on 11-10-2025.
//

#pragma once

#include "Tuple.hpp"
#include "FlatTuple.hpp"
#include "TupleSize.hpp"
#include "../concepts/IsTuple.hpp"
#include "../monads/Expected.hpp"
#include "../monads/Optional.hpp"
#include "../utils/Overload.hpp"
#include <utility>
#include <type_traits>

namespace fxt
{
    /**
     * @brief Transform each element of a tuple by applying a callable (direct call)
     *
     * Applies the given callable to each element in the input tuple and returns a new tuple
     * of the same type (fxt::tuple or fxt::flat_tuple) containing the transformed results.
     * The callable is invoked with each element, and the return values form the new tuple.
     *
     * @tparam F The callable type (deduced)
     * @tparam Tuple The tuple type (deduced)
     * @param f The callable to apply to each element
     * @param tpl The input tuple (lvalue or rvalue reference)
     * @return A new tuple of the same type containing the transformed elements
     *
     * @section Usage
     * @code
     * // Direct call with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto result = fxt::transform_tuple([](auto x) { return x * 2; }, t);
     * // result is fxt::tuple<int, int, int>{2, 4, 6}
     *
     * // Direct call with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = fxt::transform_tuple([](auto x) { return x + 1.0; }, ft);
     * // result2 is fxt::flat_tuple<double, double, double>{2.0, 3.0, 4.0}
     *
     * // With different return types
     * auto t2 = fxt::make_tuple(1, 2, 3);
     * auto result3 = fxt::transform_tuple([](auto x) { return std::to_string(x); }, t2);
     * // result3 is fxt::tuple<std::string, std::string, std::string>
     * @endcode
     */
    template<typename F, typename Tuple>
        requires tuple_like<std::remove_cvref_t<Tuple>>
    constexpr auto transform_tuple(F&& f, Tuple&& tpl)
    {
        constexpr std::size_t tupleSize = fxt::tuple_size_v<std::remove_reference_t<Tuple>>;

        return [&]<std::size_t... Indices>(std::index_sequence<Indices...>) {
            if constexpr (impl::is_fxt_tuple_v<std::remove_cvref_t<Tuple>>) {
                return fxt::make_tuple(std::invoke(std::forward<F>(f), fxt::get<Indices>(std::forward<Tuple>(tpl)))...);
            } else if constexpr (impl::is_flat_tuple_v<std::remove_cvref_t<Tuple>>) {
                return fxt::make_flat_tuple(std::invoke(std::forward<F>(f), fxt::get<Indices>(std::forward<Tuple>(tpl)))...);
            }
        }(std::make_index_sequence<tupleSize>{});
    }

    /**
     * @brief Transform each element of a tuple by applying a callable (curried version for pipeline)
     *
     * Returns a lambda that applies the given callable to each element of a tuple passed to it.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving the input type.
     *
     * @tparam F The callable type (deduced)
     * @param f The callable to apply to each element
     * @return A lambda that takes a tuple and returns a new tuple with transformed elements
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::tuple
     * auto t = fxt::make_tuple(1, 2, 3);
     * auto result = t | fxt::transform_tuple([](auto x) { return x * 2; });
     * // result is fxt::tuple<int, int, int>{2, 4, 6}
     *
     * // Pipe operator with fxt::flat_tuple
     * auto ft = fxt::make_flat_tuple(1.0, 2.0, 3.0);
     * auto result2 = ft | fxt::transform_tuple([](auto x) { return x + 1.0; });
     * // result2 is fxt::flat_tuple<double, double, double>{2.0, 3.0, 4.0}
     *
     * // Chaining with other operations
     * auto t2 = fxt::make_tuple(1, 2, 3, 4);
     * auto result3 = t2
     *     | fxt::transform_tuple([](auto x) { return x * 2; })
     *     | fxt::take<2>();
     * // result3 is fxt::tuple<int, int>{2, 4}
     * @endcode
     */
    template<typename F>
    constexpr auto transform_tuple(F&& f)
    {
        return [f = std::forward<F>(f)]<typename Tuple>(Tuple&& tpl) {
            return fxt::transform_tuple(f, std::forward<Tuple>(tpl));
        };
    }

    // ========================================================================
    // fxt::mtransform_tuple - Monadic transform for tuples
    // ========================================================================

    /**
     * @brief Transform each element of a tuple inside a monad by applying a callable (direct call)
     *
     * Applies the given callable to each element of a tuple contained within an fxt::expected
     * or fxt::optional object. Returns a monad of the same type containing the transformed tuple.
     * Works with both fxt::tuple and fxt::flat_tuple, preserving both the monad type and tuple type.
     *
     * @tparam F The callable type (deduced)
     * @tparam Container The monad type containing a tuple (deduced)
     * @param f The callable to apply to each tuple element
     * @param container The monad containing the tuple
     * @return A monad of the same type containing the transformed tuple
     *
     * @section Usage
     * @code
     * // With fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result = fxt::mtransform_tuple([](auto x) { return x * 2; }, exp);
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
     *
     * // With fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result2 = fxt::mtransform_tuple([](auto x) { return x + 1.0; }, opt);
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // With different return types
     * auto exp2 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
     * auto result3 = fxt::mtransform_tuple([](auto x) { return std::to_string(x); }, exp2);
     * // result3 is fxt::expected<fxt::tuple<std::string, std::string>, Error>
     * @endcode
     */
    template<typename F, typename Container>
    constexpr auto mtransform_tuple(F&& f, Container&& container)
    {
        return std::forward<Container>(container).transform([f = std::forward<F>(f)](auto&& tpl) {
            return fxt::transform_tuple(f, std::forward<decltype(tpl)>(tpl));
        });
    }

    /**
     * @brief Transform each element of a tuple inside a monad by applying a callable (curried version)
     *
     * Returns a lambda that transforms each element of a tuple inside a monad.
     * This overload enables pipeline-style usage with the pipe operator.
     * Works with fxt::expected or fxt::optional containing either fxt::tuple or fxt::flat_tuple.
     *
     * @tparam F The callable type (deduced)
     * @param f The callable to apply to each tuple element
     * @return A lambda that takes a monad<tuple> and returns a monad<transformed_tuple>
     *
     * @section Usage
     * @code
     * // Pipe operator with fxt::expected containing fxt::tuple
     * auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result = exp | fxt::mtransform_tuple([](auto x) { return x * 2; });
     * // result is fxt::expected<fxt::tuple<int, int, int>, Error> containing {2, 4, 6}
     *
     * // Pipe operator with fxt::optional containing fxt::flat_tuple
     * auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
     * auto result2 = opt | fxt::mtransform_tuple([](auto x) { return x + 1.0; });
     * // result2 is fxt::optional<fxt::flat_tuple<double, double>> containing {2.0, 3.0}
     *
     * // Chaining with other monadic operations
     * auto exp2 = fxt::expected<fxt::tuple<int, int, int>, Error>{fxt::make_tuple(1, 2, 3)};
     * auto result3 = exp2
     *     | fxt::mtransform_tuple([](auto x) { return x * 2; })
     *     | fxt::mselect<0, 2>();
     * // result3 is fxt::expected<fxt::tuple<int, int>, Error> containing {2, 6}
     *
     * // Error propagation
     * auto exp_err = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{}}};
     * auto result4 = exp_err | fxt::mtransform_tuple([](auto x) { return x * 2; });
     * // result4 is fxt::expected<fxt::tuple<int, int>, Error> containing the error
     *
     * // None propagation
     * auto opt_none = fxt::optional<fxt::tuple<int, int>>{};
     * auto result5 = opt_none | fxt::mtransform_tuple([](auto x) { return x * 2; });
     * // result5 is fxt::optional<fxt::tuple<int, int>> containing nullopt
     * @endcode
     */
    template<typename F>
    constexpr auto mtransform_tuple(F&& f)
    {
        return [f = std::forward<F>(f)]<typename Container>(Container&& container) {
            return fxt::mtransform_tuple(f, std::forward<Container>(container));
        };
    }

} // namespace fxt
