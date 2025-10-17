//
// Created by AI Assistant
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

TEST_CASE("Failure - Construction from message", "[failure]")
{
    SECTION("construct from string literal")
    {
        fxt::failure failure("Error message");
        REQUIRE(failure.message() == "Error message");
        REQUIRE(failure.what() == std::string("Error message"));
        REQUIRE_FALSE(failure.has_exception());
    }

    SECTION("construct from std::string")
    {
        std::string msg = "Another error";
        fxt::failure failure(msg);
        REQUIRE(failure.message() == "Another error");
        REQUIRE_FALSE(failure.has_exception());
    }

    SECTION("construct from moved string")
    {
        std::string msg = "Moved error";
        fxt::failure failure(std::move(msg));
        REQUIRE(failure.message() == "Moved error");
        REQUIRE_FALSE(failure.has_exception());
    }
}

TEST_CASE("Failure - Construction from exception", "[failure]")
{
    SECTION("construct from runtime_error")
    {
        try {
            throw std::runtime_error("Runtime error occurred");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.message() == "Runtime error occurred");
            REQUIRE(failure.has_exception());
            REQUIRE(failure.exception() != nullptr);
        }
    }

    SECTION("construct from logic_error")
    {
        try {
            throw std::logic_error("Logic error occurred");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.message() == "Logic error occurred");
            REQUIRE(failure.has_exception());
        }
    }

    SECTION("construct from unknown exception")
    {
        try {
            throw 42;
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.message() == "Unknown exception");
            REQUIRE(failure.has_exception());
        }
    }

    SECTION("construct from nullptr exception")
    {
        fxt::failure failure(std::exception_ptr{});
        REQUIRE(failure.message().empty());
        REQUIRE_FALSE(failure.has_exception());
    }
}

TEST_CASE("Failure - Factory methods", "[failure]")
{
    SECTION("from_message")
    {
        auto failure = fxt::failure::from_message("Factory message");
        REQUIRE(failure.message() == "Factory message");
        REQUIRE_FALSE(failure.has_exception());
    }

    SECTION("from_exception")
    {
        try {
            throw std::runtime_error("Factory exception");
        }
        catch (...) {
            auto failure = fxt::failure::from_exception(std::current_exception());
            REQUIRE(failure.message() == "Factory exception");
            REQUIRE(failure.has_exception());
        }
    }

    SECTION("from_current_exception")
    {
        try {
            throw std::runtime_error("Current exception");
        }
        catch (...) {
            auto failure = fxt::failure::from_current_exception();
            REQUIRE(failure.message() == "Current exception");
            REQUIRE(failure.has_exception());
        }
    }
}

TEST_CASE("Failure - Copy and move semantics", "[failure]")
{
    SECTION("copy constructor")
    {
        fxt::failure f1("Original");
        fxt::failure f2(f1);
        REQUIRE(f2.message() == "Original");
        REQUIRE(f1.message() == "Original");
    }

    SECTION("move constructor")
    {
        fxt::failure f1("Original");
        fxt::failure f2(std::move(f1));
        REQUIRE(f2.message() == "Original");
    }

    SECTION("copy assignment")
    {
        fxt::failure f1("Original");
        fxt::failure f2("Different");
        f2 = f1;
        REQUIRE(f2.message() == "Original");
        REQUIRE(f1.message() == "Original");
    }

    SECTION("move assignment")
    {
        fxt::failure f1("Original");
        fxt::failure f2("Different");
        f2 = std::move(f1);
        REQUIRE(f2.message() == "Original");
    }
}

TEST_CASE("Failure - Assignment operators", "[failure]")
{
    SECTION("assign string")
    {
        fxt::failure failure("Initial");
        failure = std::string("New message");
        REQUIRE(failure.message() == "New message");
        REQUIRE_FALSE(failure.has_exception());
    }

    SECTION("assign exception_ptr")
    {
        fxt::failure failure("Initial");
        try {
            throw std::runtime_error("Assigned exception");
        }
        catch (...) {
            failure = std::current_exception();
            REQUIRE(failure.message() == "Assigned exception");
            REQUIRE(failure.has_exception());
        }
    }
}

TEST_CASE("Failure - Conversions", "[failure]")
{
    SECTION("implicit conversion to string")
    {
        fxt::failure failure("Test message");
        std::string str = failure;
        REQUIRE(str == "Test message");
    }

    SECTION("implicit conversion to exception_ptr")
    {
        try {
            throw std::runtime_error("Exception");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            std::exception_ptr exc = failure;
            REQUIRE(exc != nullptr);
        }
    }

    SECTION("explicit bool conversion - with message")
    {
        fxt::failure failure("Error");
        REQUIRE(static_cast<bool>(failure));
    }

    SECTION("explicit bool conversion - empty message")
    {
        fxt::failure failure("");
        REQUIRE_FALSE(static_cast<bool>(failure));
    }

    SECTION("explicit bool conversion - with exception")
    {
        try {
            throw std::runtime_error("Error");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(static_cast<bool>(failure));
        }
    }
}

TEST_CASE("Failure - Accessors", "[failure]")
{
    SECTION("message()")
    {
        fxt::failure failure("Test message");
        REQUIRE(failure.message() == "Test message");
    }

    SECTION("message_view()")
    {
        fxt::failure failure("Test message");
        std::string_view view = failure.message_view();
        REQUIRE(view == "Test message");
    }

    SECTION("what()")
    {
        fxt::failure failure("Test message");
        REQUIRE(std::string(failure.what()) == "Test message");
    }

    SECTION("exception()")
    {
        try {
            throw std::runtime_error("Error");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.exception() != nullptr);
        }
    }

    SECTION("has_exception() - true")
    {
        try {
            throw std::runtime_error("Error");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.has_exception());
        }
    }

    SECTION("has_exception() - false")
    {
        fxt::failure failure("Message only");
        REQUIRE_FALSE(failure.has_exception());
    }
}

TEST_CASE("Failure - Comparison operators", "[failure]")
{
    SECTION("equality - same message")
    {
        fxt::failure f1("Error");
        fxt::failure f2("Error");
        REQUIRE(f1 == f2);
    }

    SECTION("equality - different message")
    {
        fxt::failure f1("Error1");
        fxt::failure f2("Error2");
        REQUIRE_FALSE(f1 == f2);
    }

    SECTION("inequality")
    {
        fxt::failure f1("Error1");
        fxt::failure f2("Error2");
        REQUIRE(f1 != f2);
    }

    SECTION("three-way comparison - less than")
    {
        fxt::failure f1("AAA");
        fxt::failure f2("BBB");
        REQUIRE(f1 < f2);
    }

    SECTION("three-way comparison - greater than")
    {
        fxt::failure f1("BBB");
        fxt::failure f2("AAA");
        REQUIRE(f1 > f2);
    }

    SECTION("three-way comparison - equal")
    {
        fxt::failure f1("AAA");
        fxt::failure f2("AAA");
        REQUIRE((f1 <=> f2) == std::strong_ordering::equal);
    }

    SECTION("less than or equal")
    {
        fxt::failure f1("AAA");
        fxt::failure f2("BBB");
        fxt::failure f3("AAA");
        REQUIRE(f1 <= f2);
        REQUIRE(f1 <= f3);
    }

    SECTION("greater than or equal")
    {
        fxt::failure f1("BBB");
        fxt::failure f2("AAA");
        fxt::failure f3("BBB");
        REQUIRE(f1 >= f2);
        REQUIRE(f1 >= f3);
    }
}

TEST_CASE("Failure - Stream output", "[failure]")
{
    SECTION("output to ostream")
    {
        fxt::failure failure("Stream test");
        std::ostringstream oss;
        oss << failure;
        REQUIRE(oss.str() == "Stream test");
    }

    SECTION("output with empty message")
    {
        fxt::failure failure("");
        std::ostringstream oss;
        oss << failure;
        REQUIRE(oss.str() == "");
    }
}

TEST_CASE("Failure - Hash support", "[failure]")
{
    SECTION("hash of same message")
    {
        fxt::failure f1("Error");
        fxt::failure f2("Error");
        std::hash<fxt::failure> hasher;
        REQUIRE(hasher(f1) == hasher(f2));
    }

    SECTION("hash of different messages")
    {
        fxt::failure f1("Error1");
        fxt::failure f2("Error2");
        std::hash<fxt::failure> hasher;
        // Different messages should (very likely) have different hashes
        REQUIRE(hasher(f1) != hasher(f2));
    }

    SECTION("use in unordered_set")
    {
        std::unordered_set<fxt::failure> set;
        set.insert(fxt::failure("Error1"));
        set.insert(fxt::failure("Error2"));
        set.insert(fxt::failure("Error1")); // Duplicate
        REQUIRE(set.size() == 2);
    }

    SECTION("use in unordered_map")
    {
        std::unordered_map<fxt::failure, int> map;
        map[fxt::failure("Error1")] = 1;
        map[fxt::failure("Error2")] = 2;
        map[fxt::failure("Error1")] = 10; // Update
        REQUIRE(map.size() == 2);
        REQUIRE(map[fxt::failure("Error1")] == 10);
        REQUIRE(map[fxt::failure("Error2")] == 2);
    }
}

TEST_CASE("Failure - Edge cases", "[failure]")
{
    SECTION("empty message")
    {
        fxt::failure failure("");
        REQUIRE(failure.message().empty());
        REQUIRE_FALSE(static_cast<bool>(failure));
    }

    SECTION("very long message")
    {
        std::string long_msg(10000, 'x');
        fxt::failure failure(long_msg);
        REQUIRE(failure.message().size() == 10000);
    }

    SECTION("message with special characters")
    {
        fxt::failure failure("Error: \n\t\r special chars");
        REQUIRE(failure.message() == "Error: \n\t\r special chars");
    }

    SECTION("message with unicode")
    {
        fxt::failure failure("Error: 你好 мир");
        REQUIRE(failure.message() == "Error: 你好 мир");
    }
}

TEST_CASE("Failure - Exception handling", "[failure]")
{
    SECTION("rethrow stored exception")
    {
        try {
            throw std::runtime_error("Original error");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());

            try {
                std::rethrow_exception(failure.exception());
                REQUIRE(false); // Should not reach here
            }
            catch (const std::runtime_error& e) {
                REQUIRE(std::string(e.what()) == "Original error");
            }
        }
    }

    SECTION("nested exceptions")
    {
        try {
            throw std::runtime_error("Outer error");
        }
        catch (...) {
            fxt::failure failure(std::current_exception());
            REQUIRE(failure.has_exception());
            REQUIRE(failure.message() == "Outer error");
        }
    }
}
