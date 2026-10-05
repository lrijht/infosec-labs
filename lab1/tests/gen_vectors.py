import random
from pathlib import Path

SEED = 20261005
EDGE_VALUES = [0, 1, 2**32 - 1, 2**32, 10**50 - 1]


def random_number(rng, digits):
    if digits == 1:
        return rng.randrange(10)
    return rng.randrange(10 ** (digits - 1), 10**digits)


def line(op, a, b, m, expected):
    args = ["-" if x is None else str(x) for x in (a, b, m)]
    return " ".join([op, *args, str(expected)])


def main():
    rng = random.Random(SEED)
    values = EDGE_VALUES + [random_number(rng, digits) for digits in range(1, 51)]
    lines = []

    for a in values:
        lines.append(line("dec", a, None, None, a))
    lines.append(line("dec", "007", None, None, 7))
    lines.append(line("dec", "000", None, None, 0))
    for bad in ["12a", "-5", "+3", "1.0"]:
        lines.append(line("dec", bad, None, None, "error"))

    for a in values:
        for b in values:
            lines.append(line("cmp", a, b, None, (a > b) - (a < b)))

    for a in values:
        for b in values:
            lines.append(line("add", a, b, None, a + b))
    lines.append(line("add", 2**96 - 1, 1, None, 2**96))

    for a in values:
        for b in values:
            lines.append(line("sub", a, b, None, a - b if a >= b else "error"))
    lines.append(line("sub", 2**96, 1, None, 2**96 - 1))

    # All limbs equal to 2^32-1 maximise every carry in mul and sqr.
    all_ones = [2 ** (32 * k) - 1 for k in range(1, 7)]
    for a in values:
        for b in values:
            lines.append(line("mul", a, b, None, a * b))
    for a in all_ones:
        for b in all_ones:
            lines.append(line("mul", a, b, None, a * b))
    for a in values + all_ones:
        lines.append(line("sqr", a, None, None, a * a))

    for a in values:
        for m in values:
            lines.append(line("mod", a, None, m, a % m if m != 0 else "error"))
    # powmod reduces products of two residues, so dividends reach 100 digits.
    for _ in range(300):
        m = random_number(rng, rng.randint(1, 50))
        if m == 0:
            continue
        a = random_number(rng, rng.randint(1, 100))
        lines.append(line("mod", a, None, m, a % m))
        lines.append(line("mod", a * m, None, m, 0))

    special = [0, 1, 2, 2**32 - 1, 2**32, 10**50 - 1]
    for a in special:
        for e in special:
            for m in special:
                lines.append(line("powmod", a, e, m, pow(a, e, m) if m != 0 else "error"))
    for _ in range(300):
        m = random_number(rng, rng.randint(1, 50))
        if m == 0:
            continue
        a = random_number(rng, rng.randint(1, 50))
        e = random_number(rng, rng.randint(1, 50))
        lines.append(line("powmod", a, e, m, pow(a, e, m)))
        lines.append(line("powmod", a % m, e, m, pow(a % m, e, m)))
        lines.append(line("powmod", m, e, m, pow(m, e, m)))

    out = Path(__file__).with_name("vectors.txt")
    out.write_text("\n".join(lines) + "\n", newline="\n")


if __name__ == "__main__":
    main()
