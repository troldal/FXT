# FXT - Functional Extensions for C++23

A modern, header-only C++ library that brings functional programming paradigms to C++23, featuring monadic types, tuple utilities, and composable operations with elegant pipeline syntax.

## 📋 Table of Contents

- [Features](#-features)
- [Requirements](#-requirements)
- [Installation](#-installation)
- [Quick Start](#-quick-start)
- [Core Features](#-core-features)
  - [Monads](#monads)
  - [Tuples](#tuples)
  - [Utilities](#utilities)
- [Examples](#-examples)
- [Building](#-building)
- [Testing](#-testing)
- [License](#-license)

## ✨ Features

### Monadic Types
- **`fxt::expected<T, E>`** - Result type for operations that may fail with typed errors (internally represented by either std::expected or TartanLlama's tl::expected)
- **`fxt::optional<T>`** - Type-safe nullable values with monadic operations (internally represented by either std::optional or TartanLlama's tl::optional)
- Full suite of monadic operations: `transform`, `and_then`, `or_else`, `match`, etc.
- Pipeline syntax with the `|` operator for chaining operations

### Tuple Utilities
- **`fxt::tuple<Ts...>`** - Current an alias for `std::tuple`, but other implementations are possible
- **`fxt::flat_tuple<Ts...>`** - A flat tuple implementation, for improved compilation times
- Rich set of operations: `transform_tuple`, `select`, `take`, `drop`, `append`, `tuple_cat`
- Monadic tuple operations: `mtransform_tuple`, `mtuple_cat`

### Functional Utilities
- **Currying** - Convert multi-argument functions into sequences of single-argument functions
- **Type utilities** - Type introspection and manipulation
- **Overload patterns** - Simplified visitor pattern for `std::variant`-like types

## 🔧 Requirements

- **C++23** compatible compiler
  - GCC 13+
  - Clang 17+
  - MSVC 19.30+ (Visual Studio 2022)
- **CMake** 3.20 or higher

## 📦 Installation

FXT is a header-only library. Simply include the headers in your project:

```cpp
#include <fxt.hpp>
```

### Using CMake

```cmake
include(FetchContent)

FetchContent_Declare(
    fxt
    GIT_REPOSITORY https://github.com/troldal/FXT.git
    GIT_TAG main
)

FetchContent_MakeAvailable(fxt)

target_link_libraries(your_target PRIVATE fxt::fxt)
```

## 🚀 Quick Start

```cpp
#include <fxt.hpp>
#include <iostream>
#include <string>

int main() {
    // Working with expected
    auto divide = [](int a, int b) -> fxt::expected<int, std::string> {
        if (b == 0) return fxt::unexpected{"Division by zero"};
        return a / b;
    };

    auto result = divide(10, 2)
        | fxt::transform([](int x) { return x * 2; })
        | fxt::transform([](int x) { return std::to_string(x); });

    result | fxt::match(
        [](const std::string& s) { std::cout << "Result: " << s << "\n"; },
        [](const std::string& err) { std::cout << "Error: " << err << "\n"; }
    );

    // Working with tuples
    auto t = fxt::make_tuple(1, 2, 3, 4, 5)
        | fxt::transform_tuple([](auto x) { return x * 2; })
        | fxt::take<3>()
        | fxt::append(10);

    std::cout << "First element: " << fxt::get<0>(t) << "\n"; // 2
}
```

## 🎯 Core Features

### Monads

#### fxt::expected<T, E>

A type that represents either a successful value of type `T` or an error of type `E`.

```cpp
#include <fxt.hpp>
#include <string>

struct Error { std::string message; };

// Function that may fail
auto parse_int(const std::string& s) -> fxt::expected<int, Error> {
    try {
        return std::stoi(s);
    } catch (...) {
        return fxt::unexpected{Error{"Invalid number"}};
    }
}

// Chaining operations
auto result = parse_int("42")
    | fxt::transform([](int x) { return x * 2; })
    | fxt::and_then([](int x) -> fxt::expected<double, Error> {
        return static_cast<double>(x) / 10.0;
    });

// Pattern matching
result | fxt::match(
    [](double value) { std::cout << "Success: " << value << "\n"; },
    [](const Error& err) { std::cout << "Error: " << err.message << "\n"; }
);

// Error handling
auto safe_result = result
    | fxt::or_else([](const Error&) -> fxt::expected<double, Error> {
        return 0.0; // Fallback value
    });

// Extract value with default
double value = result | fxt::value_or(0.0);
```

#### fxt::optional<T>

Type-safe nullable values with monadic operations.

```cpp
#include <fxt.hpp>

auto find_user(int id) -> fxt::optional<std::string> {
    if (id == 1) return "Alice";
    return fxt::nullopt;
}

auto result = find_user(1)
    | fxt::transform([](const std::string& name) {
        return "Hello, " + name;
    })
    | fxt::and_then([](const std::string& greeting) -> fxt::optional<int> {
        return greeting.length();
    });

// Using match
result | fxt::match(
    [](int len) { std::cout << "Length: " << len << "\n"; },
    []() { std::cout << "Not found\n"; }
);

// Value or alternative
std::string name = find_user(42) | fxt::value_or("Unknown");
```

### Monadic Operations

All monadic types support the following operations:

#### transform
Applies a function to the contained value (if present/successful).

```cpp
auto result = fxt::optional{42}
    | fxt::transform([](int x) { return x * 2; })
    | fxt::transform([](int x) { return std::to_string(x); });
// result is optional<std::string>{"84"}
```

#### and_then
Chains operations that return monads (flatMap).

```cpp
auto result = fxt::expected<int, Error>{10}
    | fxt::and_then([](int x) -> fxt::expected<std::string, Error> {
        return std::to_string(x);
    });
```

#### or_else
Provides fallback for error/empty cases.

```cpp
auto result = fxt::expected<int, Error>{fxt::unexpected{Error{"fail"}}}
    | fxt::or_else([](const Error&) -> fxt::expected<int, Error> {
        return 42; // Default value
    });
```

#### match
Pattern matching on success/error or some/none cases.

```cpp
fxt::expected<int, std::string>{42} | fxt::match(
    [](int value) { std::cout << "Value: " << value; },
    [](const std::string& err) { std::cout << "Error: " << err; }
);
```

### Tuples

#### Basic Tuple Operations

```cpp
#include <fxt.hpp>

// Create tuples
auto t1 = fxt::make_tuple(1, 2.5, "hello");
auto t2 = fxt::make_flat_tuple(3, 4, 5); // Cache-friendly flat layout

// Access elements
int first = fxt::get<0>(t1);
double second = fxt::get<1>(t1);

// Get tuple size
constexpr size_t size = fxt::tuple_size_v<decltype(t1)>; // 3
```

#### transform_tuple

Apply a function to each element of a tuple.

```cpp
auto t = fxt::make_tuple(1, 2, 3);
auto doubled = fxt::transform_tuple([](auto x) { return x * 2; }, t);
// Result: tuple<int, int, int>{2, 4, 6}

// Pipeline syntax
auto result = t
    | fxt::transform_tuple([](auto x) { return x * 2; })
    | fxt::transform_tuple([](auto x) { return x + 1; });
// Result: tuple<int, int, int>{3, 5, 7}
```

#### select, take, drop

Extract or remove elements from tuples.

```cpp
auto t = fxt::make_tuple(1, 2, 3, 4, 5);

// Select specific indices
auto selected = fxt::select<0, 2, 4>(t);
// Result: tuple<int, int, int>{1, 3, 5}

// Take first N elements
auto first_three = fxt::take<3>(t);
// Result: tuple<int, int, int>{1, 2, 3}

// Drop first N elements
auto last_two = fxt::drop<3>(t);
// Result: tuple<int, int>{4, 5}

// Pipeline style
auto result = t
    | fxt::take<4>()
    | fxt::drop<1>();
// Result: tuple<int, int, int>{2, 3, 4}
```

#### append

Add elements to tuples.

```cpp
auto t = fxt::make_tuple(1, 2);
auto extended = fxt::append(3.14, t);
// Result: tuple<int, int, double>{1, 2, 3.14}

// Pipeline syntax
auto result = t
    | fxt::append(3)
    | fxt::append(4.5);
// Result: tuple<int, int, int, double>{1, 2, 3, 4.5}
```

#### tuple_cat

Concatenate tuples.

```cpp
auto t1 = fxt::make_tuple(1, 2);
auto t2 = fxt::make_tuple(3.0, 4.0);

// Direct call
auto result = fxt::tuple_cat(t1, t2);
// Result: tuple<int, int, double, double>{1, 2, 3.0, 4.0}

// Pipeline syntax
auto result2 = t1 | fxt::tuple_cat(t2);
```

#### apply

Apply a function to tuple elements as arguments.

```cpp
auto add = [](int a, int b, int c) { return a + b + c; };
auto t = fxt::make_tuple(1, 2, 3);

int result = fxt::apply(add, t);
// Result: 6

// Pipeline syntax
auto result2 = t | fxt::apply(add);
```

### Monadic Tuple Operations

Work with tuples inside monads (expected or optional).

#### mtransform_tuple

Transform tuple elements inside a monad.

```cpp
auto exp = fxt::expected<fxt::tuple<int, int, int>, Error>{
    fxt::make_tuple(1, 2, 3)
};

auto result = exp | fxt::mtransform_tuple([](auto x) { return x * 2; });
// Result: expected<tuple<int, int, int>, Error> containing {2, 4, 6}

// Error propagation
auto err = fxt::expected<fxt::tuple<int, int>, Error>{
    fxt::unexpected{Error{"fail"}}
};
auto result2 = err | fxt::mtransform_tuple([](auto x) { return x * 2; });
// Result still contains the error
```

#### mtuple_cat

Concatenate tuples inside monads.

```cpp
auto exp1 = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
auto exp2 = fxt::expected<fxt::tuple<double, double>, Error>{fxt::make_tuple(3.0, 4.0)};

// Concatenate two monadic tuples
auto result = exp1 | fxt::mtuple_cat(exp2);
// Result: expected<tuple<int, int, double, double>, Error> containing {1, 2, 3.0, 4.0}

// Concatenate monadic tuple with plain tuple
auto plain = fxt::make_tuple(5, 6);
auto result2 = exp1 | fxt::mtuple_cat(plain);
// Result: expected<tuple<int, int, int, int>, Error> containing {1, 2, 5, 6}
```

### Utilities

#### Curry

Convert multi-argument functions into curried form.

```cpp
auto add = [](int a, int b, int c) { return a + b + c; };
auto curried = fxt::curry(add);

auto result1 = curried(1)(2)(3); // 6
auto add_one = curried(1);
auto result2 = add_one(2)(3); // 6
```

#### Type Introspection

```cpp
#include <fxt.hpp>

// Get the type name as a string_view
constexpr auto name = fxt::type_name<int>();
// name == "int"

constexpr auto complex = fxt::type_name<std::vector<std::string>>();
// complex == "std::vector<std::string>"
```

#### Tap (Side Effects)

Perform side effects without changing the value.

```cpp
auto result = fxt::expected<int, Error>{42}
    | fxt::tap([](int x) { std::cout << "Debug: " << x << "\n"; })
    | fxt::transform([](int x) { return x * 2; })
    | fxt::tap([](int x) { std::cout << "After transform: " << x << "\n"; });
// Prints debug info while maintaining the pipeline
```

## 📚 Examples

### Example 1: Error Handling Pipeline

```cpp
#include <fxt.hpp>
#include <iostream>
#include <string>

struct ParseError { std::string message; };

auto parse_config(const std::string& content) 
    -> fxt::expected<fxt::tuple<std::string, int>, ParseError> 
{
    // Simulate parsing
    return fxt::make_tuple("config", 42);
}

auto validate_config(const fxt::tuple<std::string, int>& cfg)
    -> fxt::expected<fxt::tuple<std::string, int>, ParseError>
{
    if (fxt::get<1>(cfg) < 0) {
        return fxt::unexpected{ParseError{"Invalid value"}};
    }
    return cfg;
}

int main() {
    auto result = parse_config("name=config;value=42")
        | fxt::and_then(validate_config)
        | fxt::transform([](const auto& cfg) {
            return fxt::get<1>(cfg) * 2;
        })
        | fxt::or_else([](const ParseError& err) 
            -> fxt::expected<int, ParseError> {
            std::cerr << "Error: " << err.message << "\n";
            return 0; // Default
        });

    std::cout << "Result: " << result.value() << "\n";
}
```

### Example 2: Complex Tuple Transformations

```cpp
#include <fxt.hpp>
#include <iostream>

int main() {
    // Create a tuple and perform complex transformations
    auto data = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    
    auto result = data
        | fxt::transform_tuple([](auto x) { return x * x; })  // Square
        | fxt::take<5>()                                      // First 5
        | fxt::drop<1>()                                      // Drop first
        | fxt::append(100)                                    // Add element
        | fxt::transform_tuple([](auto x) { return x / 2; }); // Halve

    // Apply aggregation
    auto sum = result | fxt::apply([](auto... xs) {
        return (xs + ...);
    });

    std::cout << "Sum: " << sum << "\n";
}
```

### Example 3: Monadic Tuple Operations

```cpp
#include <fxt.hpp>
#include <iostream>

struct Error { std::string msg; };

auto process_data(fxt::tuple<int, int, int> data)
    -> fxt::expected<fxt::tuple<double, double, double>, Error>
{
    return fxt::make_tuple(
        static_cast<double>(fxt::get<0>(data)),
        static_cast<double>(fxt::get<1>(data)),
        static_cast<double>(fxt::get<2>(data))
    );
}

int main() {
    auto input = fxt::expected<fxt::tuple<int, int, int>, Error>{
        fxt::make_tuple(1, 2, 3)
    };

    auto result = input
        | fxt::mtransform_tuple([](auto x) { return x * 2; })
        | fxt::and_then(process_data)
        | fxt::mtransform_tuple([](auto x) { return x / 10.0; })
        | fxt::mtuple_cat(fxt::make_tuple(100.0, 200.0));

    result | fxt::match(
        [](const auto& tuple) {
            std::cout << "Success: " << fxt::get<0>(tuple) << ", "
                      << fxt::get<1>(tuple) << ", "
                      << fxt::get<2>(tuple) << ", "
                      << fxt::get<3>(tuple) << ", "
                      << fxt::get<4>(tuple) << "\n";
        },
        [](const Error& err) {
            std::cout << "Error: " << err.msg << "\n";
        }
    );
}
```

### Example 4: Optional Chaining

```cpp
#include <fxt.hpp>
#include <iostream>
#include <map>

struct User {
    std::string name;
    int age;
};

std::map<int, User> users = {
    {1, {"Alice", 30}},
    {2, {"Bob", 25}}
};

auto find_user(int id) -> fxt::optional<User> {
    auto it = users.find(id);
    if (it != users.end()) return it->second;
    return fxt::nullopt;
}

auto get_user_name(int id) -> fxt::optional<std::string> {
    return find_user(id)
        | fxt::transform([](const User& u) { return u.name; });
}

int main() {
    auto result = get_user_name(1)
        | fxt::transform([](const std::string& name) {
            return "Hello, " + name + "!";
        })
        | fxt::value_or("Hello, stranger!");

    std::cout << result << "\n";
}
```

## 🔨 Building

```bash
# Clone the repository
git clone https://github.com/yourusername/FXT.git
cd FXT

# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build .

# Run tests
ctest
```

## 🧪 Testing

FXT uses Catch2 for unit testing. Run tests with:

```bash
cmake --build . --target fxt_tests
./fxt_tests
```

## 📄 License
```

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
```

## 🤝 Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## 📖 Documentation

For more detailed documentation, see the inline documentation in the header files.

## 🔗 Links

- [GitHub Repository](https://github.com/yourusername/FXT)
- [Issue Tracker](https://github.com/yourusername/FXT/issues)
- [Releases](https://github.com/yourusername/FXT/releases)

---

**FXT** - Bringing functional programming elegance to modern C++

