//
// Demo: fxt::transform_error
// This demonstrates how to use fxt::transform_error to modify error values in fxt::expected
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <cmath>

// Helper function to simulate a division operation that can fail
auto safe_divide(double numerator, double denominator) -> fxt::expected<double, std::string>
{
    if (denominator == 0.0)
    {
        return fxt::unexpected("Division by zero");
    }
    return numerator / denominator;
}

// Helper function to simulate parsing a number from a string
auto parse_int(const std::string& str) -> fxt::expected<int, int>
{
    if (str.empty())
    {
        return fxt::unexpected(1);  // Error code 1: empty string
    }
    if (!std::isdigit(str[0]) && str[0] != '-')
    {
        return fxt::unexpected(2);  // Error code 2: invalid format
    }
    try
    {
        return std::stoi(str);
    }
    catch (...)
    {
        return fxt::unexpected(3);  // Error code 3: out of range
    }
}

// Helper function to simulate a file operation
auto read_config(const std::string& filename) -> fxt::expected<std::string, int>
{
    if (filename.empty())
    {
        return fxt::unexpected(404);  // File not found
    }
    if (filename == "readonly.txt")
    {
        return fxt::unexpected(403);  // Permission denied
    }
    return "config_data";
}

int main()
{
    std::cout << "=== fxt::transform_error Demo ===\n\n";

    // Example 1: Transform string error to a more detailed message
    std::cout << "1. Transform error message with additional context:\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::transform_error([](const std::string& err) {
                        return "[MATH_ERROR] " + err + " - Cannot compute result";
                    });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 2: Transform error code to error message
    std::cout << "\n2. Transform error code to human-readable message:\n";
    {
        auto result = parse_int("abc")
                    | fxt::transform_error([](int code) {
                        switch (code)
                        {
                            case 1: return "Error: Input string is empty";
                            case 2: return "Error: Invalid number format";
                            case 3: return "Error: Number out of range";
                            default: return "Error: Unknown error";
                        }
                    });

        if (result.has_value())
        {
            std::cout << "   Parsed value: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   " << result.error() << "\n";
        }
    }

    // Example 3: Transform HTTP error codes to descriptive messages
    std::cout << "\n3. Transform HTTP error codes:\n";
    {
        auto result = read_config("")
                    | fxt::transform_error([](int http_code) {
                        return "HTTP " + std::to_string(http_code) + ": " +
                               (http_code == 404 ? "Not Found" :
                                http_code == 403 ? "Forbidden" : "Unknown Error");
                    });

        if (result.has_value())
        {
            std::cout << "   Config: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   " << result.error() << "\n";
        }
    }

    // Example 4: Chaining multiple transform_error operations
    std::cout << "\n4. Chain multiple error transformations:\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::transform_error([](const std::string& err) {
                        return "[ERROR] " + err;
                    })
                    | fxt::transform_error([](const std::string& err) {
                        return err + " [Line: 123]";
                    })
                    | fxt::transform_error([](const std::string& err) {
                        return err + " [File: calculation.cpp]";
                    });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   " << result.error() << "\n";
        }
    }

    // Example 5: Transform error with captured context
    std::cout << "\n5. Transform error with lambda capture:\n";
    {
        std::string operation = "divide";
        std::string module = "calculator";

        auto result = safe_divide(10.0, 0.0)
                    | fxt::transform_error([operation, module](const std::string& err) {
                        return "[" + module + "::" + operation + "] " + err;
                    });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   " << result.error() << "\n";
        }
    }

    // Example 6: Transform error type (int to struct)
    std::cout << "\n6. Transform error to a structured error type:\n";
    {
        struct ErrorInfo
        {
            int code;
            std::string category;
            std::string message;
        };

        auto result = read_config("")
                    | fxt::transform_error([](int code) {
                        ErrorInfo info;
                        info.code = code;
                        if (code == 404)
                        {
                            info.category = "FILE";
                            info.message = "File not found";
                        }
                        else if (code == 403)
                        {
                            info.category = "ACCESS";
                            info.message = "Permission denied";
                        }
                        else
                        {
                            info.category = "UNKNOWN";
                            info.message = "Unknown error";
                        }
                        return info;
                    });

        if (result.has_value())
        {
            std::cout << "   Config: " << result.value() << "\n";
        }
        else
        {
            const auto& err = result.error();
            std::cout << "   Error [" << err.category << ":" << err.code << "] " << err.message << "\n";
        }
    }

    // Example 7: Success case - transform_error does nothing
    std::cout << "\n7. Success case (transform_error has no effect):\n";
    {
        auto result = safe_divide(10.0, 2.0)
                    | fxt::transform_error([](const std::string& err) {
                        return "This won't be called: " + err;
                    });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << " (error transformation not applied)\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 8: Pipeline with transform_error and or_else
    std::cout << "\n8. Combine transform_error with or_else:\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::transform_error([](const std::string& err) {
                        return "[CAUGHT] " + err;
                    })
                    | fxt::or_else([](const std::string& err) {
                        std::cout << "   Handling error: " << err << "\n";
                        return fxt::expected<double, std::string>{0.0};  // Provide default value
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 9: Transform error with side effects (logging)
    std::cout << "\n9. Transform error with logging side effect:\n";
    {
        auto result = parse_int("")
                    | fxt::transform_error([](int code) {
                        std::cout << "   [LOG] Error code " << code << " occurred\n";
                        return "Error code: " + std::to_string(code);
                    });

        if (result.has_value())
        {
            std::cout << "   Parsed value: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Final error: " << result.error() << "\n";
        }
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

