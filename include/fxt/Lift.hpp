//
// Created by kenne on 15/02/2025.
//

#pragma once

#define FWD(...) std::forward<decltype(__VA_ARGS__)>(__VA_ARGS__)

#define LIFT(X) [](auto&&... args) noexcept(noexcept(X(FWD(args)...))) -> decltype(X(FWD(args)...)) { return X(FWD(args)...); }