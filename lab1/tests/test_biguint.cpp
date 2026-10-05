#include "biguint.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace {

std::string run(const std::string &op, const std::string &a, const std::string &b) {
    if (op == "dec") {
        return to_decimal(from_decimal(a));
    }
    if (op == "cmp") {
        return std::to_string(compare(from_decimal(a), from_decimal(b)));
    }
    throw std::runtime_error("unknown op: " + op);
}

} // namespace

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "usage: test_biguint VECTORS_FILE\n";
        return 2;
    }
    std::ifstream in(argv[1]);
    if (!in) {
        std::cerr << "cannot open " << argv[1] << '\n';
        return 2;
    }
    std::string line;
    int line_no = 0;
    while (std::getline(in, line)) {
        ++line_no;
        std::istringstream fields(line);
        std::string op, a, b, m, expected;
        if (!(fields >> op >> a >> b >> m >> expected)) {
            std::cerr << "line " << line_no << ": malformed: " << line << '\n';
            return 2;
        }
        // Expected failures are written as "error" in the vectors file.
        std::string actual;
        try {
            actual = run(op, a, b);
        } catch (const std::invalid_argument &) {
            actual = "error";
        }
        if (actual != expected) {
            std::cerr << "line " << line_no << ": " << line << "\n  got " << actual << '\n';
            return 1;
        }
    }
    std::cout << line_no << " cases passed\n";
    return 0;
}
