// //
// // Created by kenne on 25/02/2025.
// //
//
// #include <fxt.hpp>
// #include <iostream>
//
// #include <string_view>
// #include <tuple>
// #include <type_traits>
// #include <stdexcept>
// #include <utility>
//
// namespace detail {
//     // Helper to get the common type of a function applied to each tuple's tail
//     template <typename F, typename TupleOfTuples, std::size_t... I>
//     auto common_result_type_impl(std::index_sequence<I...>)
//         -> std::common_type_t<
//             std::invoke_result_t<F, decltype(std::declval<std::tuple_element_t<I, TupleOfTuples>>().second)>...
//         >;
//
//     template <typename F, typename TupleOfTuples>
//     using common_result_type = decltype(
//         common_result_type_impl<F, TupleOfTuples>(
//             std::make_index_sequence<std::tuple_size_v<TupleOfTuples>>{}
//         )
//     );
//
//     // Extract the tuple tail (all elements except the first)
//     template <typename Tuple, std::size_t... I>
//     auto tuple_tail_impl(const Tuple& t, std::index_sequence<I...>) {
//         return std::tuple<std::tuple_element_t<I + 1, Tuple>...>(std::get<I + 1>(t)...);
//     }
//
//     template <typename Tuple>
//     auto tuple_tail(const Tuple& t) {
//         return tuple_tail_impl(t, std::make_index_sequence<std::tuple_size_v<Tuple> - 1>{});
//     }
//
//     // Helper for visiting a tuple at compile time by index
//     template <typename Key, typename F, typename TupleOfTuples, std::size_t I = 0>
//     auto visit_tuple_impl(const Key& key, F&& f, const TupleOfTuples& tuples)
//         -> std::enable_if_t<I == std::tuple_size_v<TupleOfTuples>,
//                            common_result_type<F, TupleOfTuples>> {
//         // Base case: key not found
//         throw std::out_of_range("Key not found in tuple of tuples");
//     }
//
//     template <typename Key, typename F, typename TupleOfTuples, std::size_t I = 0>
//     auto visit_tuple_impl(const Key& key, F&& f, const TupleOfTuples& tuples)
//         -> std::enable_if_t<I < std::tuple_size_v<TupleOfTuples>,
//                            common_result_type<F, TupleOfTuples>> {
//         // Get the current inner tuple
//         const auto& inner_tuple = std::get<I>(tuples);
//
//         // Check if the key matches
//         if (std::get<0>(inner_tuple) == key) {
//             // Apply the function to the tail of the tuple
//             return std::apply(std::forward<F>(f), detail::tuple_tail(inner_tuple));
//         }
//
//         // Recursive case: try the next tuple
//         return visit_tuple_impl<Key, F, TupleOfTuples, I + 1>(key, std::forward<F>(f), tuples);
//     }
// }
//
// // Main visitor function
// template <typename Key, typename F, typename TupleOfTuples>
// auto visit(const Key& key, F&& f, const TupleOfTuples& tuples)
//     -> decltype(detail::common_result_type<F, TupleOfTuples>{}) {
//     return detail::visit_tuple_impl(key, std::forward<F>(f), tuples);
// }
//
// int main()
// {
//
//     using namespace std::literals;
//
//     auto initPipeline = []() -> std::expected<std::tuple<>, std::string> { return std::tuple<> {}; };
//
//     auto result = initPipeline()
//         | fxt::append(42)
//         | fxt::append(3.14)
//         //| fxt::transform([](auto&& t) { return std::get<0>(t) * std::get<1>(t); })
//         | fxt::apply([](int i, auto d) { return i * d; })
//         | fxt::get<2>()
//         | fxt::value_or(0);
//
//
//     auto f = [](int i, double d, std::string s) { std::cout << i << " " << d << " " << s << std::endl; };
//     auto f2 = [f](auto && PH1, auto && PH2, auto && PH3) { f(std::forward<decltype(PH3)>(PH3), std::forward<decltype(PH2)>(PH2), std::forward<decltype(PH1)>(PH1)); };
//     // f(42, 3.14, "Hello");
//
//     auto f3 = fxt::curry(f2);
//     f3("Hello")(3.14)(42);
//
//     std::cout << result << std::endl;
//
//     fxt::Failure failure = std::make_exception_ptr(std::runtime_error("Error"));
//     auto exc = failure.exception();
//
//     std::cout << failure << std::endl;
//
//     try {
//         std::rethrow_exception(exc);
//     }
//     catch (const std::exception& e) {
//         std::cout << e.what() << std::endl;
//     }
//
//     std::string msg = failure;
//         std::cout << msg << std::endl;
//
//
//     auto tuples = std::make_tuple(
//         std::make_tuple(std::string_view("int"), 42),
//         std::make_tuple(std::string_view("double"), 3.14),
//         std::make_tuple(std::string_view("string"), std::string("hello")),
//         std::make_tuple(std::string_view("complex"), 1, 2, 3)
//     );
//
//     // A visitor that can handle any arguments and returns a string
//     auto visitor = [](const auto&... args) -> std::string {
//         std::string result = "Visited values: ";
//         ((result += std::to_string(args) + " "), ...);
//         return result;
//     };
//
//     // Special visitor for the "string" key
//     auto string_visitor = [](const std::string& str) -> std::string {
//         return "String value: " + str;
//     };
//
//     // Use our visit function
//     try {
//         std::cout << visit("int", visitor, tuples) << std::endl;
//         std::cout << visit("double", visitor, tuples) << std::endl;
//         std::cout << visit("string", string_visitor, tuples) << std::endl;
//         std::cout << visit("complex", visitor, tuples) << std::endl;
//
//         // This will throw an exception as "unknown" is not a key
//         std::cout << visit("unknown", visitor, tuples) << std::endl;
//     }
//     catch (const std::exception& e) {
//         std::cerr << "Exception: " << e.what() << std::endl;
//     }
//
//     return 0;
// }

// #include <initializer_list>
// #include <iostream>
// #include <stdexcept>
// #include <string>
// #include <string_view>
// #include <tuple>
// #include <utility>
//
// // Helper: Given a tuple, return a new tuple containing all elements except the first.
// // Note: This works for tuples with at least one element.
// template <typename Tuple>
// auto tuple_tail(const Tuple& tup) {
//     return std::apply([](const auto&, const auto&... rest) {
//         return std::make_tuple(rest...);
//     }, tup);
// }
//
// // Helper alias template to compute the result type from calling f with the tail of a tuple.
// template <typename Func, typename Tuple>
// using visit_result_t = decltype(std::declval<Func>()(tuple_tail(std::declval<Tuple>())));
//
// // The visit function iterates over the tuple of tuples. For each inner tuple whose first element
// // (the key) matches the given key, it invokes the provided function with the tail of that tuple.
// // The return type is the type returned by f when invoked with the tail. All invocations are expected
// // to yield a type that is common (convertible) to each other.
// template <typename Key, typename Func, typename TupleOfTuples>
// auto visit(const Key& key, Func&& f, const TupleOfTuples& tupleOfTuples)
//     -> visit_result_t<Func, std::tuple_element_t<0, TupleOfTuples>>
// {
//     using FirstTuple = std::tuple_element_t<0, TupleOfTuples>;
//     using ResultType = visit_result_t<Func, FirstTuple>;
//
//     bool found = false;
//     ResultType result{}; // default-constructed result; you might want to change how default values are handled
//
//     std::apply(
//         [&](auto&&... innerTuple) {
//             // Use an initializer list and fold over the pack.
//             (void)std::initializer_list<int>{
//                 ( (std::get<0>(innerTuple) == key
//                     ? (result = f(tuple_tail(innerTuple)), found = true, 0)
//                     : 0), 0)...
//             };
//         },
//         tupleOfTuples
//     );
//
//     if (!found) {
//         throw std::runtime_error("Key not found in tuple of tuples.");
//     }
//     return result;
// }
//
// // Example usage:
// int main() {
//     // Our tuple of tuples. Each inner tuple has a key (std::string_view) and additional values.
//     // In this example the tail contains an int and a double.
//     auto tupleOfTuples = std::make_tuple(
//         std::make_tuple(std::string_view("apple"), 42, 3.14),
//         std::make_tuple(std::string_view("banana"), 7, 2.71),
//         std::make_tuple(std::string_view("cherry"), 11, 1.61)
//     );
//
//     // Define a function object that accepts the tail of a tuple (an int and a double) and returns a string.
//     auto fun = [](const auto& tail) -> std::string {
//         // Unpack the tail tuple.
//         int a;
//         double b;
//         std::tie(a, b) = tail;
//         return "Processed values: " + std::to_string(a) + " and " + std::to_string(b);
//     };
//
//     try {
//         // Visit the tuple with key "banana".
//         std::string result = visit(std::string_view("banana"), fun, tupleOfTuples);
//         std::cout << result << "\n";
//     } catch (const std::runtime_error& err) {
//         std::cerr << "Error: " << err.what() << "\n";
//     }
//
//     return 0;
// }


#include <tuple>
#include <iostream>
#include <fxt.hpp>


// auto someFunc(fxt::Type<int>) {
//     std::cout << "int" << std::endl;
// }
//
// auto someFunc(fxt::Type<std::string>) {
//     std::cout << "string" << std::endl;
// }
//
// // Example usage:
// int main() {
//
//     using namespace std::literals;
//
//     // Our tuple of tuples. Each inner tuple has a key (std::string_view) and additional values.
//     // In this example the tail contains an int and a double.
//     auto tupleOfTuples = std::make_tuple(
//         std::make_tuple("apple"sv, 42, 3.14),
//         std::make_tuple("banana"sv, 7, 2.71),
//         std::make_tuple("cherry"sv, 11, "1.61"),
//         std::make_tuple("banana"sv, 8, 3.71)
//     );
//
//     // Define a function object that accepts the tail of a tuple (an int and a double) and returns a string.
//     // auto fun = [](const auto& tail) -> std::string {
//     //     int a;
//     //     double b;
//     //     std::tie(a, b) = tail;
//     //     return "Processed values: " + std::to_string(a) + " and " + std::to_string(b);
//     // };
//
//     auto fun2 = [](int a, auto b) {
//         //int a;
//         //double b;
//         //std::tie(a, b) = tail;
//         if constexpr (std::is_same_v<decltype(b), double>)
//             return "Processed values: " + std::to_string(a) + " and " + std::to_string(b);
//         else
//             return "Processed values: " + std::to_string(a) + " and " + b;
//     };
//
//     try {
//         // Visit the tuple with key "banana".
//         std::string result = fxt::visit_element("banana", fun2, tupleOfTuples).value_or("No element found");
//         std::cout << result << "\n";
//     } catch (const std::runtime_error& err) {
//         std::cerr << "Error: " << err.what() << "\n";
//     }
//
//     // Using like the original Type class
//     fxt::Type<int> typeOnly;
//     using IntType = typename fxt::Type<int>::type; // IntType is int
//     IntType valueOnly = 42; // valueOnly is 42
//
//     // Using like the original TypeValue class
//     fxt::Type<std::string, double> typeWithValue(3.14);
//     using StringType = typename fxt::Type<std::string, double>::type; // StringType is std::string
//     double value = typeWithValue.value(); // value is 3.14
//
//     std::cout << "Value: " << value << std::endl;
//
//     someFunc(42);
//     someFunc("Hello"s);
//
//     auto inp = fxt::expected<std::string, int>("Input");
//     auto out = inp | fxt::transform([](std::string) { return "Hello, World"; });
//     std::cout << *out << std::endl;
//
//     return 0;
// }

#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    fxt::expected<int, std::string> v1 = fxt::unexpected(std::string("Error"));
    fxt::expected<int, std::string> v2 = fxt::unexpected(std::string("Error"));

    auto res = v1 || v2 | fxt::value_or(42);

    return 0;
}