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


#pragma once

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>
#include <compare>
#include <functional>

/**
 * @namespace fxt
 * @brief Main namespace for the FXT library
 */
namespace fxt
{
    // Forward declaration
    class failure;

    namespace detail
    {

        /**
         * @brief Base class for type-erased context storage
         */
        class context_holder_base
        {
        public:
            virtual ~context_holder_base() = default;
            [[nodiscard]] virtual std::unique_ptr<context_holder_base> clone() const = 0;
            [[nodiscard]] virtual const std::type_info& type() const noexcept = 0;
            [[nodiscard]] virtual void* get() noexcept = 0;
            [[nodiscard]] virtual const void* get() const noexcept = 0;
        };

        /**
         * @brief Concrete implementation of context holder for a specific type
         * @tparam T The context type to store
         */
        template<typename T>
        class context_holder : public context_holder_base
        {
        public:
            explicit context_holder(T value) : m_value(std::move(value)) {}

            [[nodiscard]] std::unique_ptr<context_holder_base> clone() const override
            {
                return std::make_unique<context_holder<T>>(m_value);
            }

            [[nodiscard]] const std::type_info& type() const noexcept override
            {
                return typeid(T);
            }

            [[nodiscard]] void* get() noexcept override
            {
                return &m_value;
            }

            [[nodiscard]] const void* get() const noexcept override
            {
                return &m_value;
            }

        private:
            T m_value;
        };
    }    // namespace detail

    /**
     * @class failure
     * @brief Represents a failure state with an error message, exception, and optional context data
     *
     * The Failure class encapsulates error information as either a string message or an exception pointer.
     * It also supports attaching arbitrary context data using type-erased storage, which can be retrieved
     * later by type.
     *
     * @note This class is fully comparable and hashable in C++23
     */
    class failure
    {
        /**
         * @brief Extracts the error message from an exception pointer
         * @param exception The exception pointer to extract the message from
         * @return The error message as a string
         * @note This function never throws, it catches all exceptions internally
         */
        static std::string exception_message(std::exception_ptr exception) noexcept    // NOLINT
        {
            try {
                if (exception) std::rethrow_exception(exception);
                return {};
            }
            catch (const std::exception& e) {
                return e.what();
            }
            catch (...) {
                return "Unknown exception";
            }
        }

    public:
        /**
         * @brief Constructs a Failure from a message string
         * @param message The error message
         */
        failure(std::string message) // NOLINT
            : m_message(std::move(message)),
              m_exception(nullptr) {}

        /**
         * @brief Constructs a Failure from a C-string
         * @param message The error message
         */
        failure(const char* message) // NOLINT
            : m_message(message),
              m_exception(nullptr) {}

        /**
         * @brief Constructs a Failure from an exception pointer
         * @param exception The exception pointer to store
         */
        failure(std::exception_ptr exception)    // NOLINT
            : m_message(exception_message(exception)),
              m_exception(exception)
        {}

        failure(const failure& other)
            : m_message(other.m_message),
              m_exception(other.m_exception),
              m_context(other.m_context ? other.m_context->clone() : nullptr) {}

        failure(failure&&) noexcept = default;

        failure& operator=(const failure& other)
        {
            if (this != &other) {
                m_message = other.m_message;
                m_exception = other.m_exception;
                m_context = other.m_context ? other.m_context->clone() : nullptr;
            }
            return *this;
        }

        failure& operator=(failure&&) noexcept = default;

        ~failure() = default;

        failure& operator=(std::exception_ptr exception)    // NOLINT
        {
            m_message = exception_message(exception);
            m_exception = exception;
            return *this;
        }

        failure& operator=(std::string message)
        {
            m_message = std::move(message);
            m_exception = {};
            return *this;
        }

        /**
         * @brief Factory method to create a Failure from a message
         * @param message The error message
         * @return A new Failure object
         */
        [[nodiscard]] static failure from_message(std::string message)
        {
            return {std::move(message)};
        }

        /**
         * @brief Factory method to create a Failure from an exception pointer
         * @param exception The exception pointer
         * @return A new Failure object
         */
        [[nodiscard]] static failure from_exception(std::exception_ptr exception)
        {
            return {exception};
        }

        /**
         * @brief Factory method to create a Failure from the current exception
         * @return A new Failure object containing the current exception
         * @note Should be called from within a catch block
         */
        [[nodiscard]] static failure from_current_exception()
        {
            return {std::current_exception()};
        }


        /**
         * @brief Implicit conversion to string
         * @return The error message
         */
        operator std::string() const { return message(); } // NOLINT

        /**
         * @brief Implicit conversion to exception_ptr
         * @return The stored exception pointer
         */
        operator std::exception_ptr() const { return m_exception; } // NOLINT

        /**
         * @brief Checks if the Failure contains an exception
         * @return true if an exception is stored, false otherwise
         */
        [[nodiscard]] bool has_exception() const noexcept { return m_exception != nullptr; }

        /**
         * @brief Checks if the Failure represents an actual error condition
         * @return true if there is either a message or an exception, false otherwise
         */
        [[nodiscard]] explicit operator bool() const noexcept { return !m_message.empty() || m_exception != nullptr; }

        /**
         * @brief Gets the error message
         * @return The error message as a string
         */
        [[nodiscard]] std::string message() const noexcept
        {
            return m_message;
        }

        /**
         * @brief Gets the error message as a string_view (zero-copy access)
         * @return A string_view of the error message
         * @note The returned view is only valid as long as this failure object is alive
         *       and not modified
         */
        [[nodiscard]] std::string_view message_view() const noexcept { return m_message; }

        /**
         * @brief Gets the error message as a C-string (exception-like interface)
         * @return A pointer to the error message C-string
         * @note The returned pointer is only valid as long as no modifications are made
         */
        [[nodiscard]] const char* what() const noexcept
        {
            return m_message.c_str();
        }

        /**
         * @brief Gets the stored exception
         * @return The exception pointer
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept { return m_exception; }


        /**
         * @brief Attaches context data to this failure
         * @tparam T The type of context data
         * @param data The context data to attach
         * @return Reference to this for chaining
         */
        template<typename T>
        failure& with_context(T data) &
        {
            m_context = std::make_unique<detail::context_holder<std::decay_t<T>>>(std::move(data));
            return *this;
        }

        /**
         * @brief Attaches context data to this failure (rvalue version for chaining)
         * @tparam T The type of context data
         * @param data The context data to attach
         * @return Rvalue reference to this for chaining
         */
        template<typename T>
        failure&& with_context(T data) &&
        {
            m_context = std::make_unique<detail::context_holder<std::decay_t<T>>>(std::move(data));
            return std::move(*this);
        }

        /**
         * @brief Checks if context data is attached
         * @return true if any context is attached, false otherwise
         */
        [[nodiscard]] bool has_context() const noexcept
        {
            return m_context != nullptr;
        }

        /**
         * @brief Checks if context of a specific type is attached
         * @tparam T The type to check against
         * @return true if context of type T is attached
         */
        template<typename T>
        [[nodiscard]] bool has_context() const noexcept
        {
            return m_context && m_context->type() == typeid(T);
        }

        /**
         * @brief Gets the attached context as a specific type
         * @tparam T The type to retrieve
         * @return Optional containing a copy of the context if it matches type T, nullopt otherwise
         */
        template<typename T>
        [[nodiscard]] std::optional<T> get_context() const noexcept
        {
            if (has_context<T>()) {
                return *static_cast<const T*>(m_context->get());
            }
            return std::nullopt;
        }

        /**
         * @brief Gets the type info of the attached context
         * @return The type_info of the context type, or typeid(void) if no context
         */
        [[nodiscard]] const std::type_info& context_type() const noexcept
        {
            return m_context ? m_context->type() : typeid(void);
        }

        /**
         * @brief Three-way comparison operator for Failure objects
         * @param other The Failure object to compare with
         * @return The ordering relationship
         * @note Compares based on the error message only
         */
        [[nodiscard]] auto operator<=>(const failure& other) const noexcept
        {
            return message() <=> other.message();
        }

        /**
         * @brief Equality comparison operator
         * @param other The Failure object to compare with
         * @return true if both Failures have the same message, false otherwise
         */
        [[nodiscard]] bool operator==(const failure& other) const noexcept
        {
            return message() == other.message();
        }

        /**
         * @brief Stream output operator
         * @param os The output stream
         * @param f The Failure object to output
         * @return The output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const failure& f)
        {
            os << f.message();
            return os;
        }

        /**
         * @brief Hash support for use in unordered containers
         */
        friend struct std::hash<fxt::failure>;

    private:
        std::string                                  m_message {};        ///< The error message
        std::exception_ptr                           m_exception {};      ///< The stored exception pointer
        std::unique_ptr<detail::context_holder_base> m_context {};        ///< Optional type-erased context storage
    };

}    // namespace fxt

/**
 * @brief Hash specialization for fxt::Failure
 * @note Combines hashes of both message and exception pointer for better distribution
 */
template<>
struct std::hash<fxt::failure>
{
    [[nodiscard]] std::size_t operator()(const fxt::failure& f) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(f.message());

        // TODO: BUG — `&exc` takes the address of the LOCAL copy of the exception_ptr,
        //       not of the exception object. The hash therefore depends on a stack address
        //       and is neither stable nor consistent with operator== (which compares
        //       message() only). Two equal failures can hash differently, breaking the
        //       unordered-container contract. Hash the message only (matching operator==)
        //       and drop the exception component.
        std::size_t h2 = 0;
        auto exc = f.exception();
        if (exc) {
            // Hash the pointer by converting to size_t
            h2 = std::hash<std::size_t>{}(reinterpret_cast<std::size_t>(&exc));
        }

        // Combine hashes using a common technique (boost::hash_combine style)
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
