/*
    8 8888888888 `8.`8888.      ,8' 8888888 8888888888
    8 8888        `8.`8888.    ,8'        8 8888
    8 8888         `8.`8888.  ,8'         8 8888
    8 8888          `8.`8888.,8'          8 8888
    8 888888888888   `8.`88888'           8 8888
    8 8888           .88.`8888.           8 8888
    8 8888          .8'`8.`8888.          8 8888
    8 8888         .8'  `8.`8888.         8 8888
    8 8888        .8'    `8.`8888.        8 8888
    8 8888       .8'      `8.`8888.       8 8888

         FXT - Functional Extensions for C++23

    ==================================================

    MIT License

    Copyright (c) 2025 Kenneth Troldal Balslev

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.

*/

/**
 * @file Lazy.hpp
 * @brief Lazy evaluation utility for deferred computation with thread-safe memoization
 *
 * This file provides the `fxt::lazy` class template that enables lazy evaluation
 * of callables with thread-safe memoization, similar to lazy evaluation in
 * functional programming languages like Haskell. The value is computed only once
 * on first access, and the result is cached for all subsequent accesses.
 *
 * ## Overview
 *
 * The `lazy` type wraps a callable (lambda, function object, function pointer) and
 * defers its execution until the value is actually needed. Once evaluated, the
 * result is memoized and shared across all copies of the lazy object.
 *
 * ## Key Features
 *
 * - **Lazy Evaluation**: Computation is deferred until first access
 * - **Memoization**: Result is computed exactly once and cached
 * - **Thread Safety**: Safe to access from multiple threads simultaneously
 * - **Exception Safety**: Exceptions are captured and rethrown consistently
 * - **Value Semantics**: Behaves like the underlying value via implicit conversion
 * - **Shared State**: Copied lazy objects share computation via `std::shared_ptr`
 * - **Copyable/Movable**: Unlike `std::once_flag`, lazy objects can be copied and moved
 *
 * ## Implementation Notes
 *
 * The implementation uses `std::shared_ptr` to store the callable and synchronization
 * state, enabling copy and move semantics while ensuring shared memoization. This is
 * necessary because `std::once_flag` itself is neither copyable nor movable.
 *
 * ## Example Usage
 *
 * @code
 * // Create a lazy value (not computed yet)
 * fxt::lazy expensive = []{ return compute_pi(1000000); };
 *
 * // First access triggers computation
 * double pi = expensive;  // Computed now
 * double pi2 = expensive; // Returns cached value (no recomputation)
 *
 * // Copies share the same computation
 * auto copy = expensive;
 * double pi3 = copy;      // Returns the same cached value
 * @endcode
 *
 * ## Credits
 *
 * The reference implementation is based on Ivan Čukić's presentation on
 * functional programming in C++. The implementation has been extended to
 * support copying, moving, exception handling, and enhanced thread safety.
 *
 * @author Kenneth Troldal Balslev
 * @date 2025-10-15
 * @see Ivan Čukić's reference implementation (preserved in comments at end of file)
 */

#pragma once

#include <concepts>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <variant>

namespace fxt
{

    namespace impl
    {
        /**
         * @brief Concept to constrain callables for lazy evaluation
         *
         * A callable satisfies LazyInvocable if it can be invoked with no arguments
         * and returns a non-void type. This ensures the lazy wrapper can store and
         * return a meaningful value.
         *
         * @tparam Fn The callable type to check
         */
        template<typename Fn>
        concept LazyInvocable = std::invocable<Fn> && (!std::is_void_v<std::invoke_result_t<Fn>>);

        /**
         * @brief Trait to construct monadic result type from monad template
         *
         * This trait takes a monad template (like fxt::optional or fxt::expected) without
         * template arguments and produces the fully instantiated monad type with the
         * appropriate value and error types.
         */
        template<template<typename...> typename Monad, typename T>
        struct make_monad_result;

        // Specialization for fxt::expected - uses fxt::failure as error type
        template<typename T>
        struct make_monad_result<expected, T>
        {
            using type = expected<T, failure>;
        };

        // Specialization for std::optional - single template parameter
        template<typename T>
        struct make_monad_result<std::optional, T>
        {
            using type = std::optional<T>;
        };

        template<template<typename...> typename Monad, typename T>
        using make_monad_result_t = typename make_monad_result<Monad, T>::type;
    }

    /**
     * @brief Lazy evaluation wrapper with thread-safe memoization
     *
     * The `lazy` class template wraps a callable and defers its execution until
     * the value is accessed. The computation is performed exactly once in a
     * thread-safe manner using `std::call_once`, and the result is cached.
     *
     * Key features:
     * - **Lazy evaluation**: Computation is deferred until first access
     * - **Memoization**: Result is computed once and cached
     * - **Thread-safe**: Safe to access from multiple threads simultaneously
     * - **Exception-safe**: Exceptions are captured and rethrown on access
     * - **Value semantics**: Behaves like the computed value (implicit conversion)
     * - **Shared computation**: Copied lazy objects share the same computation
     *
     * @tparam Fn A callable type that takes no arguments and returns a non-void type
     *
     * @note The callable is invoked using `std::invoke`, so it can be a function
     *       pointer, function object, lambda, or member function pointer.
     *
     * Example:
     * @code
     * fxt::lazy expensive = []{ return compute_pi(1000000); };
     * // Not computed yet
     *
     * double pi = expensive;  // Computed now, cached
     * double pi2 = expensive; // Returns cached value
     *
     * auto copy = expensive;  // Shares computation with original
     * @endcode
     */
    template<impl::LazyInvocable Fn>
    class lazy
    {
    public:
        /// The type of value returned by the callable
        using value_type = std::invoke_result_t<Fn>;

    private:
        /**
         * @brief Internal state shared between lazy instances
         *
         * This structure holds the callable, synchronization primitives, and
         * the computed result. It is stored in a shared_ptr to enable copying
         * and moving of lazy objects while sharing computation state.
         */
        struct state
        {
            /**
             * @brief Constructs the state with a callable
             * @param fn The callable to store
             */
            explicit state(Fn fn) : function(std::move(fn)) {}

            Fn                                                                  function; ///< The callable to invoke
            mutable std::once_flag                                              once;     ///< Ensures single evaluation
            mutable std::optional<std::variant<value_type, std::exception_ptr>> result;   ///< Cached result or exception
        };

        std::shared_ptr<state> m_state; ///< Shared state for computation

    public:
        /**
         * @brief Constructs a lazy wrapper from a callable
         *
         * Creates a new lazy object that will invoke the provided callable on
         * first access. The callable is not invoked during construction.
         *
         * @param fn The callable to wrap (function, lambda, function object, etc.)
         *
         * @note This constructor is implicit to allow seamless creation:
         *       `fxt::lazy val = []{return 42;};`
         */
        lazy(Fn fn) : m_state(std::make_shared<state>(std::move(fn))) {}    // NOLINT

        /**
         * @brief Copy constructor (explicit)
         *
         * Creates a new lazy object that shares the computation state with the
         * original. Both objects will return the same cached result.
         *
         * @param other The lazy object to copy from
         *
         * @note Explicit to make copying uncommon operations visible. Use
         *       regular assignment for implicit sharing when needed.
         */
        // TODO: ERGONOMICS/DOCS — the copy and move constructors are `explicit`, which makes
        //       copy-initialization ill-formed: the file's own example `auto copy = expensive;`
        //       does not compile (must be `lazy copy{expensive};`), and returning a lazy from
        //       a function by value is awkward. Explicit copy/move constructors are highly
        //       unusual; drop `explicit` or fix the documentation.
        explicit lazy(const lazy& other)       = default;    // NOLINT

        /**
         * @brief Copy assignment operator
         * @param other The lazy object to copy from
         * @return Reference to this object
         */
        lazy& operator=(const lazy& other)     = default;

        /**
         * @brief Move constructor (explicit)
         *
         * Transfers ownership of the computation state. The moved-from object
         * will throw `std::runtime_error` if accessed.
         *
         * @param other The lazy object to move from
         *
         * @note Explicit to make move operations visible in code.
         */
        explicit lazy(lazy&& other) noexcept   = default;    // NOLINT

        /**
         * @brief Move assignment operator
         * @param other The lazy object to move from
         * @return Reference to this object
         */
        lazy& operator=(lazy&& other) noexcept = default;

        /**
         * @brief Implicit conversion to the computed value
         *
         * Evaluates the callable if not already done, and returns a const reference
         * to the cached result. This operator allows lazy objects to be used
         * transparently as their underlying value type.
         *
         * @return Const reference to the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         *
         * @note This is thread-safe: multiple threads can call this simultaneously,
         *       but the callable will be invoked exactly once.
         */
        operator const value_type&() const    // NOLINT
        {
            evaluate();
            if (auto* val = std::get_if<value_type>(&*m_state->result)) return *val;
            std::rethrow_exception(std::get<std::exception_ptr>(*m_state->result));
        }

        /**
         * @brief Explicit access to the computed value
         *
         * Provides an explicit way to access the value without relying on
         * implicit conversion. Semantically equivalent to the conversion operator.
         *
         * @return Const reference to the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         */
        [[nodiscard]] const value_type& value() const { return static_cast<const value_type&>(*this); }

        /**
         * @brief Force evaluation and return a copy of the value
         *
         * Evaluates the callable if needed and returns a copy (not a reference)
         * of the result. Useful when you need to move the value out or explicitly
         * force evaluation.
         *
         * @return Copy of the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         *
         * @note The function call operator provides explicit evaluation semantics
         */
        [[nodiscard]] value_type operator()() const { return static_cast<const value_type&>(*this); }

    private:
        /**
         * @brief Ensures the callable has been evaluated
         *
         * Uses `std::call_once` to guarantee thread-safe single evaluation.
         * If the callable throws an exception, it is captured and stored for
         * rethrowing on access.
         *
         * @throws std::runtime_error if the lazy object has been moved from
         */
        void evaluate() const
        {
            if (!m_state) {
                throw std::runtime_error("lazy object has been moved from");
            }
            std::call_once(m_state->once, [this]() {
                try {
                    m_state->result.emplace(std::invoke(m_state->function));
                }
                catch (...) {
                    m_state->result.emplace(std::current_exception());
                }
            });
        }
    };

    /**
     * @brief Deduction guide for lazy class template
     *
     * Allows class template argument deduction (CTAD) to work with lazy:
     * `fxt::lazy val = []{return 42;};` deduces to `lazy<lambda_type>`
     *
     * @tparam Fn The callable type (deduced from constructor argument)
     */
    template<impl::LazyInvocable Fn>
    lazy(Fn) -> lazy<Fn>;



    // ===== Reference implementation by Ivan Čukić. Is not copyable or movable

    // template<typename Fn>
    // class lazy
    // {
    // public:
    //     using value_type = std::invoke_result_t<Fn>;
    //
    //     operator const value_type&() const {
    //         std::call_once(m_once, [&]() { m_data = m_function(); });
    //         return m_data.value();
    //     }
    //
    // private:
    //     Fn m_function;
    //     mutable std::once_flag m_once;
    //     mutable std::optional<value_type> m_data;
    //
    // };
    //
    // template<typename Fn>
    // lazy(Fn) -> lazy<Fn>;


    // ===== Implementation for static storage of callable. This enables independently created
    // ===== fxt::lazy objects to point to the same state data, but requires that the callable
    // ===== does not contain state.

    // template<typename Fn>
    // concept StatelessLazyInvocable = LazyInvocable<Fn> && std::is_empty_v<Fn>;

    // template<StatelessLazyInvocable Fn>
    // class lazy
    // {
    // public:
    //     using value_type = std::invoke_result_t<Fn>;
    //
    // private:
    //     struct state
    //     {
    //         Fn function;
    //         mutable std::once_flag once;
    //         mutable std::optional<std::variant<value_type, std::exception_ptr>> result;
    //     };
    //
    //     // Static state shared by all instances of lazy<Fn>
    //     static state& get_state() {
    //         static state s{Fn{}};  // Default-construct the callable
    //         return s;
    //     }
    //
    // public:
    //     // Store function instance (needed if Fn captures state)
    //     lazy(Fn fn) : m_fn(std::move(fn)) {}
    //
    //     explicit lazy(const lazy&) = default;
    //     lazy& operator=(const lazy&) = default;
    //     explicit lazy(lazy&&) noexcept = default;
    //     lazy& operator=(lazy&&) noexcept = default;
    //
    //     operator const value_type&() const
    //     {
    //         auto& s = get_state();
    //         std::call_once(s.once, [this, &s]() {
    //             try {
    //                 s.result.emplace(std::invoke(m_fn));
    //             }
    //             catch (...) {
    //                 s.result.emplace(std::current_exception());
    //             }
    //         });
    //
    //         if (auto* val = std::get_if<value_type>(&*s.result))
    //             return *val;
    //         std::rethrow_exception(std::get<std::exception_ptr>(*s.result));
    //     }
    //
    //     [[nodiscard]] const value_type& value() const {
    //         return static_cast<const value_type&>(*this);
    //     }
    //
    // private:
    //     Fn m_fn;  // Store instance (may have captured state)
    // };


}  // namespace fxt
