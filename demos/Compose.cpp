// Demo program showing how to use fxt::compose

#include <fxt.hpp>
#include <functional>
#include <iostream>
#include <string>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

int main() {
    print_separator("fxt::compose(f, g) — conventional right-to-left order");

    auto negate = [](int x) { return -x; };
    auto square = [](int x) { return x * x; };

    auto square_then_negate = fxt::compose(negate, square);
    std::cout << "fxt::compose(negate, square)(3) == negate(square(3)) == " << square_then_negate(3) << "\n";
    std::cout << "fxt::compose(negate, square)(-4) == negate(square(-4)) == " << square_then_negate(-4) << "\n";

    print_separator("Argument order matters");

    auto square_first  = fxt::compose(negate, square);   // negate(square(x))
    auto negate_first  = fxt::compose(square, negate);   // square(negate(x))
    std::cout << "fxt::compose(negate, square)(3) = " << square_first(3) << "  (negate applied last)\n";
    std::cout << "fxt::compose(square, negate)(3) = " << negate_first(3) << "  (square applied last)\n";

    print_separator("Composing three or more functions");

    auto to_string = [](int x) { return std::to_string(x); };
    auto pipeline = fxt::compose(to_string, negate, square);
    std::cout << "fxt::compose(to_string, negate, square)(3):\n";
    std::cout << "  = to_string(negate(square(3)))\n";
    std::cout << "  = to_string(negate(9))\n";
    std::cout << "  = to_string(-9)\n";
    std::cout << "  = \"" << pipeline(3) << "\"\n";

    print_separator("The innermost function may take multiple arguments");

    auto add = [](int a, int b) { return a + b; };
    auto add_then_negate = fxt::compose(negate, add);
    std::cout << "fxt::compose(negate, add)(3, 4) == negate(add(3, 4)) == " << add_then_negate(3, 4) << "\n";

    print_separator("Single-function composition is the function itself");

    auto same_as_negate = fxt::compose(negate);
    std::cout << "fxt::compose(negate)(5) == " << same_as_negate(5) << "\n";

    print_separator("Works with function pointers and std::function too");

    int (*triple)(int) = [](int x) { return x * 3; };
    auto with_fp = fxt::compose(negate, triple);
    std::cout << "fxt::compose(negate, <function pointer triple>)(2) == " << with_fp(2) << "\n";

    std::function<int(int)> square_fn = square;
    auto with_std_function = fxt::compose(negate, square_fn);
    std::cout << "fxt::compose(negate, <std::function square>)(4) == " << with_std_function(4) << "\n";

    print_separator("Reusable across many calls");

    auto h = fxt::compose(negate, square);
    std::cout << "h(2) = " << h(2) << ", h(3) = " << h(3) << ", h(4) = " << h(4) << "\n";

    print_separator("Mixed types across the chain");

    auto length = [](const std::string& s) { return s.size(); };
    auto describe_length = [](std::size_t n) { return "length=" + std::to_string(n); };
    auto describe = fxt::compose(describe_length, length);
    std::cout << "fxt::compose(describe_length, length)(\"hello\") == \"" << describe(std::string("hello")) << "\"\n";

    print_separator("Note: no separate monadic compose");

    std::cout << "fxt::compose does not special-case fxt::optional/fxt::expected returns.\n";
    std::cout << "Composing two functions that each return a monad produces a NESTED monad,\n";
    std::cout << "not a short-circuiting chain. For that, chain fxt::and_then instead:\n";
    std::cout << "  x | fxt::and_then(g) | fxt::and_then(f)\n";

    print_separator("Demo Complete");
    return 0;
}
