#include <scheme.h>
#include <iostream>
#include <object.h>

int main() {
    Scheme scheme;
    std::string expression;
    std::cout << "Scheme 1.0.0\n";
    while (std::cin) {
        std::cout << ">>> ";
        std::getline(std::cin, expression);
        if (expression.empty()) {
            continue;
        }
        try {
            std::cout << scheme.Evaluate(expression);
        } catch (const std::runtime_error& ex) {
            std::cout << ex.what();
        }
        std::cout << '\n';
    }
}
