#include <iostream>
#include <string>
#include <vector>
#include "fxt.hpp"

struct Person {
    std::string name;
    int age;

    void print() const {
        std::cout << name << " (" << age << " years old)\n";
    }
};

void process_value(const fxt::immutable<int>& value) {
    // Can read the value through implicit conversion
    int x = value;
    std::cout << "Processing: " << x << "\n";

    // Can also dereference
    std::cout << "Dereferenced: " << *value << "\n";
}

void process_person(const fxt::immutable<Person>& person) {
    // Arrow operator allows calling const methods
    person->print();

    // Can access through dereference
    std::cout << "Name: " << (*person).name << "\n";
}

fxt::immutable<std::vector<int>> create_data() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    // Return by value - will be moved
    return fxt::immutable<std::vector<int>>(std::move(data));
}

int main() {
    // Basic usage with int
    fxt::immutable<int> num(42);
    process_value(num);

    // Usage with struct
    Person p{"Alice", 30};
    fxt::immutable<Person> immut_person(p);
    process_person(immut_person);

    // Move semantics - extract value from rvalue
    auto data_wrapper = create_data();
    std::vector<int> extracted = std::move(data_wrapper);

    std::cout << "Extracted data size: " << extracted.size() << "\n";

    return 0;
}