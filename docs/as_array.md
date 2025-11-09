# fxt::as_array

## Overview

`fxt::as_array` is a utility function that converts tuple-like objects (either `fxt::tuple` or `fxt::flat_tuple`) into `std::array`. This is useful when you need to convert a heterogeneous collection of values into a homogeneous array.

## Features

- Convert `fxt::tuple` or `fxt::flat_tuple` to `std::array`
- Automatic type conversion for all tuple elements
- Support for both lvalue and rvalue tuples
- Curried version for use with pipe operator
- Compile-time size checking

## Function Signatures

### Direct Version
```cpp
template<typename T, typename... Ts>
constexpr std::array<T, sizeof...(Ts)> as_array(const tuple<Ts...>& t);

template<typename T, typename... Ts>
constexpr std::array<T, sizeof...(Ts)> as_array(tuple<Ts...>&& t);

template<typename T, typename... Ts>
constexpr std::array<T, sizeof...(Ts)> as_array(const flat_tuple<Ts...>& t);

template<typename T, typename... Ts>
constexpr std::array<T, sizeof...(Ts)> as_array(flat_tuple<Ts...>&& t);
```

### Curried Version
```cpp
template<typename T>
constexpr auto as_array();
```

## Parameters

- `T`: The target type for array elements (must be constructible from all tuple elements)
- `t`: The tuple to convert

## Return Value

`std::array<T, N>` where `N` is the number of elements in the input tuple.

## Usage Examples

### Basic Conversion

```cpp
// Convert tuple with mixed types to double array
auto tpl = fxt::make_tuple(1, 2.5, 3);
auto arr = fxt::as_array<double>(tpl);  // std::array<double, 3>{1.0, 2.5, 3.0}
```

### Using flat_tuple

```cpp
// Convert flat_tuple to int array (truncation occurs)
auto ft = fxt::make_flat_tuple(1.7, 2.3, 3.9);
auto arr = fxt::as_array<int>(ft);  // std::array<int, 3>{1, 2, 3}
```

### Curried Version with Pipe Operator

```cpp
// Use with pipe operator for functional composition
auto arr = fxt::make_tuple(10, 20, 30) 
    | fxt::as_array<double>();  // std::array<double, 3>{10.0, 20.0, 30.0}
```

### Chaining Operations

```cpp
// Chain with other tuple operations
auto result = fxt::make_tuple(1, 2, 3, 4, 5)
    | fxt::take<3>()
    | fxt::as_array<double>();  // std::array<double, 3>{1.0, 2.0, 3.0}
```

### Type Conversions

```cpp
// Converting to larger types
auto tpl = fxt::make_tuple(1, 2, 3);
auto arr = fxt::as_array<long long>(tpl);  // std::array<long long, 3>

// Converting with truncation
auto tpl2 = fxt::make_tuple(1.7, 2.3, 3.9);
auto arr2 = fxt::as_array<int>(tpl2);  // std::array<int, 3>{1, 2, 3}
```

## Requirements

- All tuple elements must be constructible to type `T`
- The tuple must have a known size at compile time
- Works with both `const` and non-`const` tuples
- Supports move semantics for rvalue tuples

## Notes

- The function is `constexpr`, allowing compile-time evaluation when possible
- Type conversion follows standard C++ conversion rules
- For heterogeneous tuples, ensure the target type `T` can represent all source types appropriately
- The resulting array has the same number of elements as the input tuple

---

# fxt::mas_array

## Overview

`fxt::mas_array` is the monadic version of `fxt::as_array`. It works with monadic containers (like `std::optional` or `fxt::expected`) that contain tuples, converting them to arrays. It has two modes of operation depending on whether the tuple elements themselves are monadic.

## Features

- Convert tuples inside monadic containers to arrays
- Support for both non-monadic and monadic tuple elements
- Works with `std::optional`, `fxt::expected`, and other monads
- Automatic type transformation for array elements
- Preserves monadic structure when elements are monadic

## Function Signature

### Curried Version (Only)
```cpp
template<typename T>
constexpr auto mas_array();
```

## Parameters

- `T`: The target type for array elements (or value type when elements are monadic)

## Return Value

Returns a lambda that transforms a monad containing a tuple:
- If tuple elements are **non-monadic**: `Monad<std::array<T, N>>`
- If tuple elements are **monadic**: `Monad<std::array<Monad<T>, N>>`

## Usage Examples

### Case 1: Non-Monadic Tuple Elements

```cpp
// Optional containing tuple of regular values
std::optional<fxt::tuple<int, double, int>> opt = fxt::make_tuple(1, 2.5, 3);
auto result = opt | fxt::mas_array<double>();
// result: std::optional<std::array<double, 3>>

if (result) {
    // *result is std::array<double, 3>{1.0, 2.5, 3.0}
}
```

```cpp
// Expected containing tuple
fxt::expected<fxt::tuple<int, int, int>, std::string> exp = fxt::make_tuple(10, 20, 30);
auto result = exp | fxt::mas_array<int>();
// result: fxt::expected<std::array<int, 3>, std::string>
```

### Case 2: Monadic Tuple Elements

```cpp
// Optional containing tuple of optionals
using OptInt = std::optional<int>;
std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt = 
    fxt::make_tuple(OptInt{1}, OptInt{2}, OptInt{3});

auto result = opt | fxt::mas_array<double>();
// result: std::optional<std::array<std::optional<double>, 3>>

if (result) {
    for (const auto& elem : *result) {
        if (elem) {
            std::cout << *elem << " ";  // Each element is an optional<double>
        }
    }
}
```

```cpp
// Expected containing tuple of expected values
using ExpInt = fxt::expected<int, std::string>;
fxt::expected<fxt::tuple<ExpInt, ExpInt, ExpInt>, std::string> exp = 
    fxt::make_tuple(ExpInt{100}, ExpInt{200}, ExpInt{300});

auto result = exp | fxt::mas_array<double>();
// result: fxt::expected<std::array<fxt::expected<double, std::string>, 3>, std::string>
```

### Error Handling

```cpp
// Empty optional
std::optional<fxt::tuple<int, int, int>> opt;
auto result = opt | fxt::mas_array<double>();
// result.has_value() == false

// Expected with error
fxt::expected<fxt::tuple<int, int>, std::string> exp = std::unexpected("error");
auto result = exp | fxt::mas_array<double>();
// result.has_value() == false
// result.error() == "error"
```

### Monadic Elements with Failures

```cpp
// Some optional elements are nullopt
using OptInt = std::optional<int>;
std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt = 
    fxt::make_tuple(OptInt{1}, OptInt{}, OptInt{3});  // Middle element is nullopt

auto result = opt | fxt::mas_array<double>();
// result.has_value() == true
// (*result)[0].has_value() == true
// (*result)[1].has_value() == false (nullopt)
// (*result)[2].has_value() == true
```

### Chaining Operations

```cpp
// Calculate sum of array elements
auto sum = std::optional{fxt::make_tuple(1, 2, 3, 4, 5)}
    | fxt::mas_array<double>()
    | fxt::transform([](const auto& arr) {
        double sum = 0;
        for (const auto& val : arr) sum += val;
        return sum;
    });
// sum: std::optional<double>{15.0}
```

### Using with flat_tuple

```cpp
std::optional<fxt::flat_tuple<double, double, double>> opt = 
    fxt::make_flat_tuple(1.1, 2.2, 3.3);

auto result = opt | fxt::mas_array<int>();
// result: std::optional<std::array<int, 3>>
// Values are truncated: {1, 2, 3}
```

## How It Decides Between Modes

The function uses compile-time detection to determine if tuple elements have a `.transform()` method:

1. **Non-monadic elements** (e.g., `int`, `double`, `std::string`):
   - Converts directly to `std::array<T, N>`
   - Simple type conversion

2. **Monadic elements** (e.g., `std::optional<int>`, `fxt::expected<double, E>`):
   - Converts to `std::array<Monad<T>, N>`
   - Each monad is transformed to have value type `T`
   - Preserves the monadic structure and error states

## Requirements

- Only available as a curried version (for pipe operator usage)
- The outer container must have a `.transform()` method
- The tuple must be `tuple_like` (`fxt::tuple` or `fxt::flat_tuple`)
- For monadic elements: all elements must be monadic (consistent)
- Type `T` must be constructible from all tuple element types

## Notes

- Always preserves the outer monadic structure
- When tuple elements are monadic, each is transformed independently
- Empty optionals or expected errors are propagated correctly
- Failures in individual monadic elements are preserved in the result array
- The function is `constexpr` where possible

## Comparison: as_array vs mas_array

| Feature | `as_array` | `mas_array` |
|---------|------------|-------------|
| Works with | Plain tuples | Tuples in monads |
| Input | `tuple<Ts...>` | `Monad<tuple<Ts...>>` |
| Output | `std::array<T, N>` | `Monad<std::array<T, N>>` |
| Monadic elements | No special handling | Transforms each monad |
| Curried only | No (has both) | Yes |

## See Also

- `fxt::as_array` - Non-monadic tuple to array conversion
- `fxt::tuple` - Standard tuple wrapper
- `fxt::flat_tuple` - Memory-efficient tuple alternative
- `fxt::transform` - Transform values in monadic containers
- `fxt::mapply` - Apply functions to tuples in monadic contexts
- `fxt::take` - Extract first N elements from tuple
- `fxt::drop` - Skip first N elements from tuple

