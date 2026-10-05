#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Little-endian base 2^32 limbs without leading zeros; zero is an empty vector.
struct BigUInt {
    std::vector<std::uint32_t> limbs;
};

BigUInt from_decimal(const std::string &s);
std::string to_decimal(const BigUInt &x);

// Returns -1, 0 or 1.
int compare(const BigUInt &a, const BigUInt &b);

BigUInt add(const BigUInt &a, const BigUInt &b);
// Requires a >= b, otherwise throws std::invalid_argument.
BigUInt sub(const BigUInt &a, const BigUInt &b);
BigUInt mul(const BigUInt &a, const BigUInt &b);
BigUInt sqr(const BigUInt &a);
// Throws std::invalid_argument if m is zero.
BigUInt mod(const BigUInt &a, const BigUInt &m);

struct PowmodStep {
    std::size_t bit_index;
    int bit;
    BigUInt after_square;
    BigUInt after_multiply;
};

// Left-to-right square-and-multiply. If trace is not null, one step per bit of e
// is appended to it, from the most significant bit down. Throws if m is zero.
BigUInt powmod(const BigUInt &a, const BigUInt &e, const BigUInt &m,
               std::vector<PowmodStep> *trace = nullptr);
