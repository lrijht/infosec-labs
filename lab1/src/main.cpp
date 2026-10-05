#include "biguint.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void print_usage() {
    std::cerr << "usage:\n"
                 "  bdp add A B [M]\n"
                 "  bdp mul A B [M]\n"
                 "  bdp sqr A [M]\n"
                 "  bdp mod A M\n"
                 "  bdp powmod A E M [--trace]\n";
}

void print_trace(const std::vector<PowmodStep> &trace) {
    std::cout << "bit\tvalue\tafter square\tafter multiply\n";
    for (const PowmodStep &step : trace) {
        std::cout << step.bit_index << '\t' << step.bit << '\t' << to_decimal(step.after_square)
                  << '\t' << (step.bit ? to_decimal(step.after_multiply) : "-") << '\n';
    }
}

int run(const std::vector<std::string> &args) {
    const std::string &cmd = args[0];
    const std::size_t n = args.size();

    if ((cmd == "add" || cmd == "mul") && (n == 3 || n == 4)) {
        BigUInt a = from_decimal(args[1]);
        BigUInt b = from_decimal(args[2]);
        BigUInt r = cmd == "add" ? add(a, b) : mul(a, b);
        if (n == 4) {
            r = mod(r, from_decimal(args[3]));
        }
        std::cout << to_decimal(r) << '\n';
        return 0;
    }
    if (cmd == "sqr" && (n == 2 || n == 3)) {
        BigUInt r = sqr(from_decimal(args[1]));
        if (n == 3) {
            r = mod(r, from_decimal(args[2]));
        }
        std::cout << to_decimal(r) << '\n';
        return 0;
    }
    if (cmd == "mod" && n == 3) {
        std::cout << to_decimal(mod(from_decimal(args[1]), from_decimal(args[2]))) << '\n';
        return 0;
    }
    if (cmd == "powmod" && (n == 4 || (n == 5 && args[4] == "--trace"))) {
        std::vector<PowmodStep> trace;
        BigUInt r = powmod(from_decimal(args[1]), from_decimal(args[2]), from_decimal(args[3]),
                           n == 5 ? &trace : nullptr);
        if (n == 5) {
            print_trace(trace);
        }
        std::cout << to_decimal(r) << '\n';
        return 0;
    }
    print_usage();
    return 2;
}

} // namespace

int main(int argc, char **argv) {
    std::vector<std::string> args(argv + 1, argv + argc);
    if (args.empty()) {
        print_usage();
        return 2;
    }
    try {
        return run(args);
    } catch (const std::invalid_argument &e) {
        std::cerr << "error: " << e.what() << '\n';
        return 1;
    }
}
