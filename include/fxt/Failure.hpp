//
// Created by kenne on 26/02/2025.
//

#pragma once

#include <exception>
#include <optional>
#include <string>
#include <utility>
#include <ostream>

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
        Failure(Failure&&) = default;

        Failure& operator=(const Failure&) = default;
        Failure& operator=(Failure&&) = default;

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
         * @brief Gets the stored exception
         * @return The exception pointer
         */
        [[nodiscard]] std::exception_ptr exception() const noexcept { return m_exception; }

        friend std::ostream& operator<<(std::ostream& os, const Failure& failure)
        {
            os << failure.message();
            return os;
        }

    private:
        std::string        m_message {};      ///< The error message
        std::exception_ptr m_exception {};    ///< The stored exception pointer
    };

}    // namespace fxt