//
// Created by kenne on 01-11-2025.
//

#pragma once

namespace fxt {
    template<typename TFunction>
    struct for_each_adaptor {
        TFunction function;

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
