// Demo program showing how to use fxt::type_enum
// A type-based enum with compile-time type safety

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <vector>

void print_separator(const std::string& title) {
    std::cout << "\n========== " << title << " ==========\n\n";
}

// Define some example types for our type enum
struct Circle {
    double radius;
    [[nodiscard]] double area() const { return 3.14159 * radius * radius; }
    friend std::ostream& operator<<(std::ostream& os, const Circle& c) {
        return os << "Circle(radius=" << c.radius << ")";
    }
};

struct Rectangle {
    double width;
    double height;
    [[nodiscard]] double area() const { return width * height; }
    friend std::ostream& operator<<(std::ostream& os, const Rectangle& r) {
        return os << "Rectangle(width=" << r.width << ", height=" << r.height << ")";
    }
};

struct Triangle {
    double base;
    double height;
    [[nodiscard]] double area() const { return 0.5 * base * height; }
    friend std::ostream& operator<<(std::ostream& os, const Triangle& t) {
        return os << "Triangle(base=" << t.base << ", height=" << t.height << ")";
    }
};

// Another example: different message types
struct InfoMessage {
    std::string content;
    friend std::ostream& operator<<(std::ostream& os, const InfoMessage& m) {
        return os << "[INFO] " << m.content;
    }
};

struct WarningMessage {
    std::string content;
    int severity;
    friend std::ostream& operator<<(std::ostream& os, const WarningMessage& m) {
        return os << "[WARNING:" << m.severity << "] " << m.content;
    }
};

struct ErrorMessage {
    std::string content;
    int error_code;
    friend std::ostream& operator<<(std::ostream& os, const ErrorMessage& m) {
        return os << "[ERROR:" << m.error_code << "] " << m.content;
    }
};

int main()
{
    print_separator("FXT Type Enum Demo");

    // Define type enums
    using Shape = fxt::type_enum<Circle, Rectangle, Triangle>;
    using Message = fxt::type_enum<InfoMessage, WarningMessage, ErrorMessage>;

    print_separator("1. Construction and Assignment");

    std::cout << "Creating type enums:\n\n";

    // Default constructor - initializes to first type
    Shape shape1;
    std::cout << "  Shape shape1;  // Defaults to first type (Circle)\n";
    std::cout << "  shape1.index(): " << shape1.index() << "\n";

    // Construct from a specific type
    Shape shape2 = Rectangle{5.0, 3.0};
    std::cout << "\n  Shape shape2 = Rectangle{5.0, 3.0};\n";
    std::cout << "  shape2: " << shape2.get<Rectangle>() << "\n";

    // Assignment
    shape1 = Circle{2.5};
    std::cout << "\n  shape1 = Circle{2.5};\n";
    std::cout << "  shape1: " << shape1.get<Circle>() << "\n";

    print_separator("2. Type Checking");

    Shape shape = Triangle{4.0, 3.0};
    std::cout << "Shape shape = Triangle{4.0, 3.0};\n\n";

    // Check which type is currently held
    std::cout << "  shape.is<Circle>(): " << (shape.is<Circle>() ? "true" : "false") << "\n";
    std::cout << "  shape.is<Rectangle>(): " << (shape.is<Rectangle>() ? "true" : "false") << "\n";
    std::cout << "  shape.is<Triangle>(): " << (shape.is<Triangle>() ? "true" : "false") << "\n";

    // Get the current index
    std::cout << "\n  shape.index(): " << shape.index() << "\n";

    print_separator("3. Compile-Time Index Lookup");

    std::cout << "Get the compile-time index of specific types:\n\n";
    std::cout << "  Shape::IndexOf<Circle>(): " << Shape::IndexOf<Circle>() << "\n";
    std::cout << "  Shape::IndexOf<Rectangle>(): " << Shape::IndexOf<Rectangle>() << "\n";
    std::cout << "  Shape::IndexOf<Triangle>(): " << Shape::IndexOf<Triangle>() << "\n";

    print_separator("4. Getting Values");

    Shape shape_rect = Rectangle{6.0, 4.0};
    std::cout << "Shape shape_rect = Rectangle{6.0, 4.0};\n\n";

    // Safe retrieval with get_if
    std::cout << "Using get_if() (safe, returns nullptr if wrong type):\n\n";
    if (auto* rect = shape_rect.get_if<Rectangle>()) {
        std::cout << "  Rectangle found: " << *rect << "\n";
        std::cout << "  Area: " << rect->area() << "\n";
    }

    if (auto* circ = shape_rect.get_if<Circle>()) {
        std::cout << "  Circle found\n";
    } else {
        std::cout << "  Circle not found (get_if returned nullptr)\n";
    }

    // Direct retrieval with get (throws if wrong type)
    std::cout << "\nUsing get() (throws if wrong type):\n\n";
    try {
        Rectangle& rect = shape_rect.get<Rectangle>();
        std::cout << "  Successfully got Rectangle: " << rect << "\n";
    } catch (const std::bad_variant_access& e) {
        std::cout << "  Exception: " << e.what() << "\n";
    }

    print_separator("5. Switch Statement Support");

    Shape shape_switch = Circle{3.0};
    std::cout << "Shape shape_switch = Circle{3.0};\n\n";
    std::cout << "switch (shape_switch.index()) {\n";

    switch (shape_switch.index()) {
        case Shape::IndexOf<Circle>():
            std::cout << "  case Circle: Computing circle area\n";
            std::cout << "    Area = " << shape_switch.get<Circle>().area() << "\n";
            break;
        case Shape::IndexOf<Rectangle>():
            std::cout << "  case Rectangle: Computing rectangle area\n";
            std::cout << "    Area = " << shape_switch.get<Rectangle>().area() << "\n";
            break;
        case Shape::IndexOf<Triangle>():
            std::cout << "  case Triangle: Computing triangle area\n";
            std::cout << "    Area = " << shape_switch.get<Triangle>().area() << "\n";
            break;
    }

    std::cout << "}\n";

    print_separator("6. Type-Safe Visitation with Generic Lambda");

    std::cout << "Using visit() with a generic lambda:\n\n";

    Shape shape_visit = Rectangle{8.0, 5.0};
    std::cout << "Shape shape_visit = Rectangle{8.0, 5.0};\n\n";

    shape_visit.visit([](auto& s) {
        using T = std::decay_t<decltype(s)>;
        std::cout << "  Visitor received: " << s << "\n";
        std::cout << "  Type: " << typeid(T).name() << "\n";
        std::cout << "  Area: " << s.area() << "\n";
    });

    print_separator("7. Type-Safe Visitation with Overloaded Lambdas");

    std::cout << "Using visit() with fxt::overloaded:\n\n";

    Message msg = WarningMessage{"Low memory", 3};
    std::cout << "Message msg = WarningMessage{\"Low memory\", 3};\n\n";

    msg.visit(fxt::overload{
        [](const InfoMessage& m) {
            std::cout << "  Info handler: " << m << "\n";
            std::cout << "  No action needed\n";
        },
        [](const WarningMessage& m) {
            std::cout << "  Warning handler: " << m << "\n";
            std::cout << "  Action: Log and monitor\n";
        },
        [](const ErrorMessage& m) {
            std::cout << "  Error handler: " << m << "\n";
            std::cout << "  Action: Alert administrator\n";
        }
    });

    print_separator("8. Modifying Values Through Visit");

    std::cout << "Modifying the held value using non-const visit:\n\n";

    Shape shape_modify = Circle{1.0};
    std::cout << "Shape shape_modify = Circle{1.0};\n";
    std::cout << "  Before: " << shape_modify.get<Circle>() << "\n";

    shape_modify.visit([](auto& s) {
        using T = std::decay_t<decltype(s)>;
        if constexpr (std::is_same_v<T, Circle>) {
            s.radius *= 2.0;
        } else if constexpr (std::is_same_v<T, Rectangle>) {
            s.width *= 2.0;
            s.height *= 2.0;
        } else if constexpr (std::is_same_v<T, Triangle>) {
            s.base *= 2.0;
            s.height *= 2.0;
        }
    });

    std::cout << "  After doubling: " << shape_modify.get<Circle>() << "\n";

    print_separator("9. Practical Example: Shape Calculator");

    std::cout << "Processing a collection of shapes:\n\n";

    std::vector<Shape> shapes;
    shapes.push_back(Circle{2.0});
    shapes.push_back(Rectangle{4.0, 3.0});
    shapes.push_back(Triangle{5.0, 4.0});
    shapes.push_back(Circle{1.5});

    double total_area = 0.0;
    int circle_count = 0;
    int rect_count = 0;
    int triangle_count = 0;

    for (const auto& shape : shapes) {
        // Calculate area using visit
        double area = shape.visit([](const auto& s) { return s.area(); });
        total_area += area;

        // Count shape types
        shape.visit(fxt::overload{
            [&](const Circle& c) {
                ++circle_count;
                std::cout << "  " << c << " -> area = " << area << "\n";
            },
            [&](const Rectangle& r) {
                ++rect_count;
                std::cout << "  " << r << " -> area = " << area << "\n";
            },
            [&](const Triangle& t) {
                ++triangle_count;
                std::cout << "  " << t << " -> area = " << area << "\n";
            }
        });
    }

    std::cout << "\nSummary:\n";
    std::cout << "  Total shapes: " << shapes.size() << "\n";
    std::cout << "  Circles: " << circle_count << "\n";
    std::cout << "  Rectangles: " << rect_count << "\n";
    std::cout << "  Triangles: " << triangle_count << "\n";
    std::cout << "  Total area: " << total_area << "\n";

    print_separator("10. Accessing Underlying Variant");

    Shape shape_variant = Triangle{3.0, 4.0};
    std::cout << "Shape shape_variant = Triangle{3.0, 4.0};\n\n";

    // Access the underlying std::variant
    std::variant<Circle, Rectangle, Triangle>& var = shape_variant.variant();
    std::cout << "  Accessed underlying variant\n";
    std::cout << "  Variant index: " << var.index() << "\n";

    // Can use std::visit directly on the variant
    std::visit([](const auto& s) {
        std::cout << "  std::visit on variant: " << s << "\n";
    }, var);

    print_separator("11. Practical Example: Message Router");

    std::cout << "Simple message routing system:\n\n";

    auto route_message = [](const Message& msg) {
        std::cout << "  Routing: ";

        msg.visit(fxt::overload{
            [](const InfoMessage& m) {
                std::cout << m << " -> Console\n";
            },
            [](const WarningMessage& m) {
                std::cout << m << " -> Log file";
                if (m.severity >= 5) {
                    std::cout << " + Email";
                }
                std::cout << "\n";
            },
            [](const ErrorMessage& m) {
                std::cout << m << " -> Log file + Email + Alert\n";
            }
        });
    };

    route_message(InfoMessage{"System started"});
    route_message(WarningMessage{"Disk usage at 80%", 3});
    route_message(WarningMessage{"Disk usage at 95%", 7});
    route_message(ErrorMessage{"Database connection failed", 500});

    print_separator("Demo Complete");

    std::cout << "Key takeaways:\n";
    std::cout << "  • type_enum uses C++ types as enum values\n";
    std::cout << "  • Wraps std::variant with a cleaner interface\n";
    std::cout << "  • Supports switch statements via compile-time indices\n";
    std::cout << "  • Type-safe visitation with generic or overloaded lambdas\n";
    std::cout << "  • Safe value access with get_if() or direct with get()\n";
    std::cout << "  • Can modify values through non-const visit\n";
    std::cout << "  • Perfect for type-safe state machines and polymorphism\n\n";

    return 0;
}

