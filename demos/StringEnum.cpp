// Demo program showing how to use fxt::string_enum
// A compile-time string-based enum with type safety

#include <fxt.hpp>
#include <iostream>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

int main()
{
    print_separator("FXT String Enum Demo");

    // Define a string enum for HTTP methods
    using HttpMethod = fxt::string_enum<"GET", "POST", "PUT", "DELETE", "PATCH">;

    // Define a string enum for log levels
    using LogLevel = fxt::string_enum<"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL">;

    print_separator("1. Construction and Assignment");

    std::cout << "Creating string enums:\n\n";

    // Default constructor - initializes to first string
    HttpMethod method1;
    std::cout << "  HttpMethod method1;  // Defaults to first: \"" << method1.value() << "\"\n";

    // Construct from string literal
    HttpMethod method2 = "POST";
    std::cout << "  HttpMethod method2 = \"POST\";  // Value: \"" << method2.value() << "\"\n";

    // Construct from invalid string - throws exception
    std::cout << "\n  HttpMethod method3 = \"INVALID\";  // Throws std::invalid_argument\n";
    try {
        HttpMethod method3 = "INVALID";
        std::cout << "  ERROR: Should have thrown exception!\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught exception: " << e.what() << "\n";
    }

    // Assignment
    method1 = "DELETE";
    std::cout << "\n  method1 = \"DELETE\";  // Now: \"" << method1.value() << "\"\n";

    // Invalid assignment - throws exception
    std::cout << "\n  method1 = \"INVALID\";  // Throws std::invalid_argument\n";
    try {
        method1 = "INVALID";
        std::cout << "  ERROR: Should have thrown exception!\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught exception: " << e.what() << "\n";
        std::cout << "  method1 remains: \"" << method1.value() << "\"\n";
    }

    print_separator("2. Checking Current Value");

    LogLevel level = "WARNING";
    std::cout << "LogLevel level = \"WARNING\";\n\n";

    // Check specific values using is<>()
    std::cout << "  level.is<\"DEBUG\">(): " << (level.is<"DEBUG">() ? "true" : "false") << "\n";
    std::cout << "  level.is<\"WARNING\">(): " << (level.is<"WARNING">() ? "true" : "false") << "\n";
    std::cout << "  level.is<\"ERROR\">(): " << (level.is<"ERROR">() ? "true" : "false") << "\n";

    // Get the current value
    std::cout << "\n  level.value(): \"" << level.value() << "\"\n";

    // Get the index
    std::cout << "  level.index(): " << level.index() << "\n";

    print_separator("3. Compile-Time Index Lookup");

    std::cout << "Get the compile-time index of specific strings:\n\n";
    std::cout << "  LogLevel::IndexOf<\"DEBUG\">(): " << LogLevel::IndexOf<"DEBUG">() << "\n";
    std::cout << "  LogLevel::IndexOf<\"INFO\">(): " << LogLevel::IndexOf<"INFO">() << "\n";
    std::cout << "  LogLevel::IndexOf<\"WARNING\">(): " << LogLevel::IndexOf<"WARNING">() << "\n";
    std::cout << "  LogLevel::IndexOf<\"ERROR\">(): " << LogLevel::IndexOf<"ERROR">() << "\n";
    std::cout << "  LogLevel::IndexOf<\"CRITICAL\">(): " << LogLevel::IndexOf<"CRITICAL">() << "\n";

    print_separator("4. Switch Statement Support");

    HttpMethod method = "PUT";
    std::cout << "HttpMethod method = \"PUT\";\n\n";
    std::cout << "switch (method.index()) {\n";

    switch (method.index()) {
        case HttpMethod::IndexOf<"GET">():
            std::cout << "  case GET: Reading resource\n";
            break;
        case HttpMethod::IndexOf<"POST">():
            std::cout << "  case POST: Creating resource\n";
            break;
        case HttpMethod::IndexOf<"PUT">():
            std::cout << "  case PUT: Updating resource (full replace)\n";
            break;
        case HttpMethod::IndexOf<"DELETE">():
            std::cout << "  case DELETE: Deleting resource\n";
            break;
        case HttpMethod::IndexOf<"PATCH">():
            std::cout << "  case PATCH: Partial update\n";
            break;
        default:
            std::cout << "  default: Unknown method\n";
            break;
    }

    std::cout << "}\n";

    print_separator("5. Type-Safe Visitation");

    std::cout << "Using visit() with overloaded lambdas:\n\n";

    LogLevel log = "ERROR";
    std::cout << "LogLevel log = \"ERROR\";\n\n";

    // Visit with typed_string parameters
    log.visit(fxt::overload{
        [](LogLevel::Type<"DEBUG"> s) {
            std::cout << "  DEBUG handler: Low priority message\n";
        },
        [](LogLevel::Type<"INFO"> s) {
            std::cout << "  INFO handler: General information\n";
        },
        [](LogLevel::Type<"WARNING"> s) {
            std::cout << "  WARNING handler: Something needs attention\n";
        },
        [](LogLevel::Type<"ERROR"> s) {
            std::cout << "  ERROR handler: Something went wrong!\n";
        },
        [](LogLevel::Type<"CRITICAL"> s) {
            std::cout << "  CRITICAL handler: System failure imminent!\n";
        }
    });

    print_separator("6. Exception Safety");

    std::cout << "String enums always hold valid values - invalid strings throw exceptions:\n\n";

    std::cout << "  HttpMethod method = \"GET\";\n";
    HttpMethod safe_method = "GET";
    std::cout << "  method.value(): \"" << safe_method.value() << "\"\n";
    std::cout << "  method.is_valid(): " << (safe_method.is_valid() ? "true" : "false") << "\n";

    std::cout << "\n  try { method = \"INVALID\"; }\n";
    try {
        safe_method = "INVALID";
        std::cout << "  ERROR: Should have thrown exception!\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught: " << e.what() << "\n";
        std::cout << "  method.value(): \"" << safe_method.value() << "\" (unchanged)\n";
        std::cout << "  method.is_valid(): " << (safe_method.is_valid() ? "true" : "false") << "\n";
    }

    std::cout << "\nConstructing with invalid string:\n";
    std::cout << "  try { HttpMethod bad = \"TRACE\"; }\n";
    try {
        HttpMethod bad_method = "TRACE";
        std::cout << "  ERROR: Should have thrown exception!\n";
    } catch (const std::invalid_argument& e) {
        std::cout << "  Caught: " << e.what() << "\n";
        std::cout << "  Object was not constructed\n";
    }

    print_separator("7. Practical Example: HTTP Request Router");

    std::cout << "Simple HTTP router using string_enum:\n\n";

    auto handle_request = [](const std::string& method_str, const std::string& path) {
        std::cout << "  Request: " << method_str << " " << path << "\n";
        std::cout << "  Response: ";

        try {
            HttpMethod method = std::string_view(method_str);
            method.visit(fxt::overload{
                [&](HttpMethod::Type<"GET"> s) {
                    std::cout << "200 OK - Resource retrieved\n";
                },
                [&](HttpMethod::Type<"POST"> s) {
                    std::cout << "201 Created - Resource created\n";
                },
                [&](HttpMethod::Type<"PUT"> s) {
                    std::cout << "200 OK - Resource updated\n";
                },
                [&](HttpMethod::Type<"DELETE"> s) {
                    std::cout << "204 No Content - Resource deleted\n";
                },
                [&](HttpMethod::Type<"PATCH"> s) {
                    std::cout << "200 OK - Resource partially updated\n";
                }
            });
        } catch (const std::invalid_argument&) {
            std::cout << "405 Method Not Allowed\n";
        }
    };

    handle_request("GET", "/users/123");
    handle_request("POST", "/users");
    handle_request("DELETE", "/users/123");
    handle_request("TRACE", "/users/123");

    print_separator("8. Comparison and Equality");

    HttpMethod m1 = "GET";
    HttpMethod m2 = "GET";
    HttpMethod m3 = "POST";

    std::cout << "HttpMethod m1 = \"GET\", m2 = \"GET\", m3 = \"POST\";\n\n";
    std::cout << "  m1 == m2: " << (m1 == m2 ? "true" : "false") << "\n";
    std::cout << "  m1 == m3: " << (m1 == m3 ? "true" : "false") << "\n";
    std::cout << "  m1 != m3: " << (m1 != m3 ? "true" : "false") << "\n";

    print_separator("Demo Complete");

    std::cout << "Key takeaways:\n";
    std::cout << "  • string_enum provides type-safe string-based enums\n";
    std::cout << "  • Each string value becomes a distinct type\n";
    std::cout << "  • Supports switch statements via compile-time indices\n";
    std::cout << "  • Type-safe visitation with overloaded lambdas\n";
    std::cout << "  • Throws std::invalid_argument for invalid string assignments\n";
    std::cout << "  • Always holds a valid value - no invalid state possible\n";
    std::cout << "  • All string checking happens at compile time\n\n";

    return 0;
}

