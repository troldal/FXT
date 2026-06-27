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
 * - **Resource Release**: The callable and any captured state are freed after evaluation
 *
 * ## Implementation Notes
 *
 * The implementation uses `std::shared_ptr` to store the callable and synchronization
 * state, enabling copy and move semantics while ensuring shared memoization. This is
 * necessary because `std::once_flag` itself is neither copyable nor movable.
 *
 * The shared state uses `std::variant<std::monostate, value_type, std::exception_ptr>`
 * to track evaluation status: monostate = pending, value_type = success, exception_ptr
 * = failure. The callable is stored in an `std::optional` and cleared after evaluation
 * to release any captured resources.
 *
 * ## Example Usage
 *
 * @code
 * // Create a lazy value (not computed yet)
 * fxt::lazy expensive = []{ return compute_pi(1000000); };
 *
 * // First access triggers computation
 * double pi = expensive;   // Computed now, callable freed
 * double pi2 = expensive;  // Returns cached value (no recomputation)
 *
 * // Copies share the same computation
 * auto copy = expensive;
 * double pi3 = copy;       // Returns the same cached value
 * @endcode
 *
 * ## Credits
 *
 * The reference implementation is based on Ivan Čukić's presentation on
 * functional programming in C++. The implementation has been extended to
 * support copying, moving, exception handling, enhanced thread safety,
 * and callable release after evaluation.
 *
 * @author Kenneth Troldal Balslev
 * @date 2025-10-15
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
         * and returns a copy-constructible object type. Specifically:
         * - Reference and void return types are rejected (cannot be stored in the variant)
         * - Move-only return types are rejected: lazy's contract is "compute once, give
         *   everyone a copy"; a non-copyable value cannot be shared across instances
         * - std::exception_ptr and std::monostate are rejected because they are the
         *   other two alternatives in the result variant; duplicating them would make
         *   variant access ambiguous
         *
         * @tparam Fn The callable type to check
         */
        template<typename Fn>
        concept LazyInvocable = std::invocable<Fn> &&
                                std::is_object_v<std::invoke_result_t<Fn>> &&
                                std::copy_constructible<std::decay_t<std::invoke_result_t<Fn>>> &&
                                !std::is_same_v<std::decay_t<std::invoke_result_t<Fn>>, std::exception_ptr> &&
                                !std::is_same_v<std::decay_t<std::invoke_result_t<Fn>>, std::monostate>;
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
     * - **Resource release**: The callable is freed after first evaluation
     *
     * @tparam Fn A callable type that takes no arguments and returns an object type
     *
     * @note The callable is invoked using `std::invoke`, so it can be a function
     *       pointer, function object, lambda, or member function pointer.
     *
     * @note All accessors return by value. `value_type` is required to be
     *       copy-constructible — move-only return types are rejected by the concept.
     *
     * Example:
     * @code
     * fxt::lazy expensive = []{ return compute_pi(1000000); };
     * // Not computed yet; callable and any captures are alive
     *
     * double pi = expensive;  // Computed now; callable freed
     * double pi2 = expensive; // Returns cached value
     *
     * auto copy = expensive;  // Shares computation with original
     * @endcode
     */
    template<impl::LazyInvocable Fn>
    class lazy
    {
    public:
        /// The type of value returned by the callable (cv-qualifiers stripped for storage)
        using value_type = std::decay_t<std::invoke_result_t<Fn>>;

    private:
        /**
         * @brief Internal state shared between lazy instances
         *
         * Holds the callable (until evaluated), the once_flag, and the result.
         * The result variant encodes three states:
         *   - monostate: not yet evaluated
         *   - value_type: successfully evaluated and cached
         *   - exception_ptr: evaluation threw; exception cached for rethrowing
         *
         * Stored in a shared_ptr to enable copying and moving of lazy objects
         * while sharing computation state.
         */
        struct state
        {
            explicit state(Fn fn) : function(std::move(fn)) {}

            std::optional<Fn>                                             function; ///< Callable; reset after evaluation
            std::once_flag                                                once;     ///< Ensures single evaluation
            std::variant<std::monostate, value_type, std::exception_ptr> result;   ///< Pending / value / exception
        };

        std::shared_ptr<state> m_state;

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
         * @brief Copy constructor
         *
         * Creates a new lazy object that shares the computation state with the
         * original. Both objects will return the same cached result — sharing
         * the memoized computation is the intended behaviour, analogous to
         * copying a std::shared_ptr.
         *
         * @param other The lazy object to copy from
         */
        lazy(const lazy& other)            = default;

        /**
         * @brief Copy assignment operator
         * @param other The lazy object to copy from
         * @return Reference to this object
         */
        lazy& operator=(const lazy& other ) = default;

        /**
         * @brief Move constructor
         *
         * Transfers ownership of the computation state. The moved-from object
         * will throw `std::runtime_error` if accessed.
         *
         * @param other The lazy object to move from
         */
        lazy(lazy&& other) noexcept        = default;

        /**
         * @brief Move assignment operator
         * @param other The lazy object to move from
         * @return Reference to this object
         */
        lazy& operator=(lazy&& other) noexcept = default;

        /**
         * @brief Implicit conversion to a copy of the computed value
         *
         * Evaluates the callable if not already done and returns a copy of the
         * cached result. Safe to use with temporaries — `const auto& r = lazy{...}`
         * binds to the returned copy (lifetime extension applies).
         *
         * @return Copy of the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         *
         * @note Thread-safe: multiple threads may call this simultaneously;
         *       the callable is invoked exactly once.
         */
        operator value_type() const { return get_result(); }    // NOLINT

        /**
         * @brief Explicit access to a copy of the computed value
         *
         * @return Copy of the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         */
        [[nodiscard]] value_type value() const { return get_result(); }

        /**
         * @brief Evaluate and return a copy of the value via call syntax
         *
         * Syntactic alternative to implicit conversion; useful when the conversion
         * would be ambiguous or when explicit evaluation intent is preferred.
         *
         * @return Copy of the computed value
         * @throws Any exception thrown by the callable
         * @throws std::runtime_error if the lazy object has been moved from
         */
        [[nodiscard]] value_type operator()() const { return get_result(); }

    private:
        /**
         * @brief Shared implementation for all accessors
         *
         * Evaluates if needed and returns a const reference to the cached result,
         * rethrowing any stored exception. The public accessors copy from this
         * reference when value_type is copy-constructible.
         */
        [[nodiscard]] const value_type& get_result() const
        {
            evaluate();
            if (auto* val = std::get_if<value_type>(&m_state->result)) return *val;
            std::rethrow_exception(std::get<std::exception_ptr>(m_state->result));
        }

        /**
         * @brief Ensures the callable has been evaluated exactly once
         *
         * Uses `std::call_once` to guarantee thread-safe single evaluation.
         * Captures any exception thrown by the callable (or by move-constructing
         * the result into the cache) and stores it for rethrowing on access.
         * The callable is released after evaluation to free captured resources.
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
                    m_state->result = std::invoke(*m_state->function);
                }
                catch (...) {
                    m_state->result = std::current_exception();
                }
                m_state->function.reset();
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

}  // namespace fxt
