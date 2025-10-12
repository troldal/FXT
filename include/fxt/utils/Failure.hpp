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
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <ostream>
#include <compare>
#include <functional>

/**
 * @namespace fxt
 * @brief Main namespace for the FXT library
 */
namespace fxt
{
    /**
     * @class Failure
     * @brief Represents a failure state with an optional error message and exception
     *
     * The Failure class encapsulates error information, providing both a string message
     * and an optional exception pointer. It can be constructed from either a message
     * or an exception, and provides implicit conversions to both string and exception_ptr.
     *
     * @note This class is fully comparable and hashable in C++23
     */
    class Failure
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
        Failure(std::string message) // NOLINT
            : m_message(std::move(message)),
              m_exception(nullptr) {}

        /**
         * @brief Constructs a Failure from an exception pointer
         * @param exception The exception pointer to store
         */
        Failure(std::exception_ptr exception)    // NOLINT
            : m_message(exception_message(exception)),
              m_exception(exception)
        {}

        Failure(const Failure&) = default;
        Failure(Failure&&) noexcept = default;

        Failure& operator=(const Failure&) = default;
        Failure& operator=(Failure&&) noexcept = default;

        ~Failure() = default;

        Failure& operator=(std::exception_ptr exception)    // NOLINT
        {
            m_message = exception_message(exception);
            m_exception = exception;
            return *this;
        }

        Failure& operator=(std::string message)
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
        [[nodiscard]] static Failure from_message(std::string message)
        {
            return {std::move(message)};
        }

        /**
         * @brief Factory method to create a Failure from an exception pointer
         * @param exception The exception pointer
         * @return A new Failure object
         */
        [[nodiscard]] static Failure from_exception(std::exception_ptr exception)
        {
            return {exception};
        }

        /**
         * @brief Factory method to create a Failure from the current exception
         * @return A new Failure object containing the current exception
         * @note Should be called from within a catch block
         */
        [[nodiscard]] static Failure from_current_exception()
        {
            return {std::current_exception()};
        }

        /**
         * @brief Implicit conversion to string
         * @return The error message
         */
        operator std::string() const { return m_message; } // NOLINT

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
         * @return A const reference to the error message
         */
        [[nodiscard]] const std::string& message() const noexcept { return m_message; }

        /**
         * @brief Gets the error message as a string_view (zero-copy access)
         * @return A string_view of the error message
         */
        [[nodiscard]] std::string_view message_view() const noexcept { return m_message; }

        /**
         * @brief Gets the error message as a C-string (exception-like interface)
         * @return A pointer to the error message C-string
         */
        [[nodiscard]] const char* what() const noexcept { return m_message.c_str(); }

        /**
         * @brief Gets the stored exception
         * @return The exception pointer
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept { return m_exception; }

        /**
         * @brief Three-way comparison operator for Failure objects
         * @param other The Failure object to compare with
         * @return The ordering relationship
         * @note Compares based on the error message only
         */
        [[nodiscard]] auto operator<=>(const Failure& other) const noexcept
        {
            return m_message <=> other.m_message;
        }

        /**
         * @brief Equality comparison operator
         * @param other The Failure object to compare with
         * @return true if both Failures have the same message, false otherwise
         */
        [[nodiscard]] bool operator==(const Failure& other) const noexcept
        {
            return m_message == other.m_message;
        }

        /**
         * @brief Stream output operator
         * @param os The output stream
         * @param failure The Failure object to output
         * @return The output stream
         */
        friend std::ostream& operator<<(std::ostream& os, const Failure& failure)
        {
            os << failure.message();
            return os;
        }

        /**
         * @brief Hash support for use in unordered containers
         */
        friend struct std::hash<fxt::Failure>;

    private:
        std::string        m_message {};      ///< The error message
        std::exception_ptr m_exception {};    ///< The stored exception pointer
    };

}    // namespace fxt

/**
 * @brief Hash specialization for fxt::Failure
 * @note Combines hashes of both message and exception pointer for better distribution
 */
template<>
struct std::hash<fxt::Failure>
{
    [[nodiscard]] std::size_t operator()(const fxt::Failure& failure) const noexcept
    {
        std::size_t h1 = std::hash<std::string>{}(failure.message());

        // Hash the exception pointer using its address
        std::size_t h2 = 0;
        auto exc = failure.exception();
        if (exc) {
            // Hash the pointer by converting to size_t
            h2 = std::hash<std::size_t>{}(reinterpret_cast<std::size_t>(&exc));
        }

        // Combine hashes using a common technique (boost::hash_combine style)
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
