//
// Created by kenne on 01-11-2025.
//

#pragma once

// TODO: COMPLETENESS — missing #include <ranges> (std::ranges::range) and <utility>
//       (std::forward); this header only compiles via transitive includes. Also missing
//       the FXT license banner that every other header carries.

namespace fxt {
    template<typename TFunction>
    struct for_each_adaptor {
        TFunction function;

        // TODO: SAFETY — returning `TRange&&` hands back an rvalue reference to the argument.
        //       When the adaptor is invoked directly with a temporary
        //       (`auto&& r = fxt::for_each(f)(std::vector{1,2,3});`) the temporary dies at the
        //       end of the full expression and r dangles. Return a decayed value for rvalue
        //       inputs (or TRange like std::forward does in the operator| below, which
        //       happens to copy because its return type is deduced as a value).
        //       Also prefer std::invoke(function, elem) so member pointers work, matching
        //       the rest of the library.
        template<std::ranges::range TRange>
        constexpr auto operator()(TRange&& range) const -> TRange&& {
            for (auto&& elem : range) {
                function(elem);
            }
            return std::forward<TRange>(range);
        }
    };

    inline constexpr auto for_each = []<typename TFunction>(TFunction&& f) {
        return for_each_adaptor<std::decay_t<TFunction>>{ std::forward<TFunction>(f) };
    };

    template<std::ranges::range TRange, typename TFunction>
    constexpr auto operator|(TRange&& range, for_each_adaptor<TFunction> adaptor) {
        return adaptor(std::forward<TRange>(range));
    }
}
