#include <iostream>
#include <string>
#include <cstdio>
#include "exprtk.hpp"
#define SIZE 1000

int main() {    
    // 1. Variable Declaration
    double x;

    // 2. Setup ExprTk
    exprtk::symbol_table<double> symbol_table;
    symbol_table.add_variable("x", x);
    symbol_table.add_constants();

    exprtk::expression<double> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<double> parser;

    // 3. Input
    std::string function_string;
    std::getline(std::cin, function_string);

    // 4. Traslation and Compilation
    if (!parser.compile(function_string, expression)) {
        std::cout << "\nInvalid formula:\n";
        for (std::size_t i = 0; i < parser.error_count(); ++i) {
            exprtk::parser_error::type error = parser.get_error(i);
            std::cout << "Error: " << error.diagnostic << " at position " << error.token.position << "\n";
        }
        return 1;
    }

    std::cout << "\nFunction compiled successfully!\n";

    // 7. Input of domain
    double domain_start, domain_end;
    std::cout << "Enter domain start: ";
    std::cin >> domain_start;
    std::cout << "Enter domain end: ";
    std::cin >> domain_end;

    // 6. Generation (Generate SIZE values)
    FILE* output_file = std::fopen("output.txt", "w");
    for (int i = 0; i < SIZE; ++i) {
        x = domain_start + i * (domain_end - domain_start) / (SIZE - 1);
        double y = expression.value();
        std::fprintf(output_file, "%lf\n", y);
    }

    /*
    <val_x1>
    <val_x2> 
    ...
    */

    return 0;
}