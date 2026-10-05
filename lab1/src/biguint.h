#pragma once

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
