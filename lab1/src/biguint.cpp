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

void shift_left_one(std::vector<std::uint32_t> &limbs) {
    std::uint32_t shifted_out = 0;
    for (std::uint32_t &limb : limbs) {
        std::uint32_t top_bit = limb >> 31;
        limb = (limb << 1) | shifted_out;
        shifted_out = top_bit;
    }
    if (shifted_out != 0) {
        limbs.push_back(shifted_out);
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

BigUInt mul(const BigUInt &a, const BigUInt &b) {
    BigUInt r;
    r.limbs.assign(a.limbs.size() + b.limbs.size(), 0);
    for (std::size_t i = 0; i < a.limbs.size(); ++i) {
        std::uint64_t carry = 0;
        for (std::size_t j = 0; j < b.limbs.size(); ++j) {
            std::uint64_t cur =
                static_cast<std::uint64_t>(a.limbs[i]) * b.limbs[j] + r.limbs[i + j] + carry;
            r.limbs[i + j] = static_cast<std::uint32_t>(cur);
            carry = cur >> 32;
        }
        r.limbs[i + b.limbs.size()] = static_cast<std::uint32_t>(carry);
    }
    trim(r.limbs);
    return r;
}

BigUInt sqr(const BigUInt &a) {
    const std::size_t n = a.limbs.size();
    BigUInt r;
    r.limbs.assign(2 * n, 0);

    for (std::size_t i = 0; i < n; ++i) {
        std::uint64_t carry = 0;
        for (std::size_t j = i + 1; j < n; ++j) {
            std::uint64_t cur =
                static_cast<std::uint64_t>(a.limbs[i]) * a.limbs[j] + r.limbs[i + j] + carry;
            r.limbs[i + j] = static_cast<std::uint32_t>(cur);
            carry = cur >> 32;
        }
        r.limbs[i + n] = static_cast<std::uint32_t>(carry);
    }

    // The cross sum is below a^2 / 2, so doubling it cannot carry out of 2n limbs.
    shift_left_one(r.limbs);

    std::uint64_t carry = 0;
    for (std::size_t i = 0; i < n; ++i) {
        std::uint64_t cur =
            static_cast<std::uint64_t>(a.limbs[i]) * a.limbs[i] + r.limbs[2 * i] + carry;
        r.limbs[2 * i] = static_cast<std::uint32_t>(cur);
        cur = (cur >> 32) + r.limbs[2 * i + 1];
        r.limbs[2 * i + 1] = static_cast<std::uint32_t>(cur);
        carry = cur >> 32;
    }

    trim(r.limbs);
    return r;
}

BigUInt mod(const BigUInt &a, const BigUInt &m) {
    if (m.limbs.empty()) {
        throw std::invalid_argument("mod: modulus is zero");
    }
    BigUInt r;
    for (std::size_t i = a.limbs.size(); i-- > 0;) {
        for (int bit = 31; bit >= 0; --bit) {
            shift_left_one(r.limbs);
            if ((a.limbs[i] >> bit) & 1) {
                if (r.limbs.empty()) {
                    r.limbs.push_back(1);
                } else {
                    r.limbs[0] |= 1;
                }
            }
            if (compare(r, m) >= 0) {
                r = sub(r, m);
            }
        }
    }
    return r;
}

BigUInt powmod(const BigUInt &a, const BigUInt &e, const BigUInt &m,
               std::vector<PowmodStep> *trace) {
    const BigUInt base = mod(a, m);
    // 1 mod m rather than 1, so that m = 1 gives 0 even when e = 0.
    BigUInt r = mod(BigUInt{{1}}, m);

    std::size_t bit_count = 0;
    if (!e.limbs.empty()) {
        bit_count = (e.limbs.size() - 1) * 32;
        for (std::uint32_t top = e.limbs.back(); top != 0; top >>= 1) {
            ++bit_count;
        }
    }

    for (std::size_t k = bit_count; k-- > 0;) {
        int bit = static_cast<int>((e.limbs[k / 32] >> (k % 32)) & 1);
        BigUInt squared = mod(sqr(r), m);
        r = bit ? mod(mul(squared, base), m) : squared;
        if (trace) {
            trace->push_back({k, bit, squared, r});
        }
    }
    return r;
}
