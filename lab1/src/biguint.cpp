#include "biguint.h"

#include <stdexcept>

namespace {

constexpr std::uint32_t chunk_base = 1000000000;
constexpr int chunk_digits = 9;

void trim(std::vector<std::uint32_t> &limbs) {
    while (!limbs.empty() && limbs.back() == 0) {
        limbs.pop_back();
    }
}

} // namespace

BigUInt from_decimal(const std::string &s) {
    if (s.empty()) {
        throw std::invalid_argument("empty number");
    }
    BigUInt x;
    for (char c : s) {
        if (c < '0' || c > '9') {
            throw std::invalid_argument("invalid digit in number: " + s);
        }
        std::uint64_t carry = static_cast<std::uint64_t>(c - '0');
        for (std::uint32_t &limb : x.limbs) {
            std::uint64_t cur = static_cast<std::uint64_t>(limb) * 10 + carry;
            limb = static_cast<std::uint32_t>(cur);
            carry = cur >> 32;
        }
        if (carry != 0) {
            x.limbs.push_back(static_cast<std::uint32_t>(carry));
        }
    }
    return x;
}

std::string to_decimal(const BigUInt &x) {
    if (x.limbs.empty()) {
        return "0";
    }
    std::vector<std::uint32_t> n = x.limbs;
    std::vector<std::uint32_t> chunks;
    while (!n.empty()) {
        std::uint64_t rem = 0;
        for (std::size_t i = n.size(); i-- > 0;) {
            std::uint64_t cur = (rem << 32) | n[i];
            n[i] = static_cast<std::uint32_t>(cur / chunk_base);
            rem = cur % chunk_base;
        }
        trim(n);
        chunks.push_back(static_cast<std::uint32_t>(rem));
    }
    std::string s = std::to_string(chunks.back());
    for (std::size_t i = chunks.size() - 1; i-- > 0;) {
        std::string part = std::to_string(chunks[i]);
        s += std::string(chunk_digits - part.size(), '0') + part;
    }
    return s;
}

int compare(const BigUInt &a, const BigUInt &b) {
    if (a.limbs.size() != b.limbs.size()) {
        return a.limbs.size() < b.limbs.size() ? -1 : 1;
    }
    for (std::size_t i = a.limbs.size(); i-- > 0;) {
        if (a.limbs[i] != b.limbs[i]) {
            return a.limbs[i] < b.limbs[i] ? -1 : 1;
        }
    }
    return 0;
}

BigUInt add(const BigUInt &a, const BigUInt &b) {
    const BigUInt &longer = a.limbs.size() >= b.limbs.size() ? a : b;
    const BigUInt &shorter = a.limbs.size() >= b.limbs.size() ? b : a;
    BigUInt r;
    r.limbs.reserve(longer.limbs.size() + 1);
    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < longer.limbs.size(); ++i) {
        std::uint64_t cur = longer.limbs[i] + carry;
        if (i < shorter.limbs.size()) {
            cur += shorter.limbs[i];
        }
        r.limbs.push_back(static_cast<std::uint32_t>(cur));
        carry = cur >> 32;
    }
    if (carry != 0) {
        r.limbs.push_back(static_cast<std::uint32_t>(carry));
    }
    return r;
}

BigUInt sub(const BigUInt &a, const BigUInt &b) {
    if (compare(a, b) < 0) {
        throw std::invalid_argument("sub: minuend is less than subtrahend");
    }
    BigUInt r;
    r.limbs.reserve(a.limbs.size());
    std::uint64_t borrow = 0;
    for (std::size_t i = 0; i < a.limbs.size(); ++i) {
        std::uint64_t subtrahend = borrow;
        if (i < b.limbs.size()) {
            subtrahend += b.limbs[i];
        }
        std::uint64_t cur = a.limbs[i];
        if (cur < subtrahend) {
            cur += std::uint64_t{1} << 32;
            borrow = 1;
        } else {
            borrow = 0;
        }
        r.limbs.push_back(static_cast<std::uint32_t>(cur - subtrahend));
    }
    trim(r.limbs);
    return r;
}
