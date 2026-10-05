# Lab 1: fast modular exponentiation

Modular exponentiation for RSA and ElGamal on unsigned integers (inputs up to
50 decimal digits) with its own long arithmetic: addition, subtraction,
multiplication, squaring and remainder. C++17, standard library only.

## Build

From the repository root:

```
cmake -S lab1 -B build
cmake --build build
```

With MinGW on Windows add `-G "MinGW Makefiles"` to the first command.

## Run

```
build/bdp add A B [M]
build/bdp mul A B [M]
build/bdp sqr A [M]
build/bdp mod A M
build/bdp powmod A E M [--trace]
```

With `M` the result of `add`, `mul` and `sqr` is reduced modulo `M`.
`--trace` prints one row per bit of `E`, from the most significant one:
bit index, bit value, `r` after squaring, `r` after multiplication
(`-` when the bit is 0), followed by the result.

```
$ build/bdp powmod 4 13 497 --trace
bit	value	after square	after multiply
3	1	1	4
2	1	16	64
1	0	120	-
0	1	484	445
445
```

Exit codes: 0 on success, 1 for an invalid number or a zero modulus,
2 for wrong arguments.

## Tests

```
ctest --test-dir build
```

`lab1/tests/vectors.txt` holds the test cases, one per line as
`op a b m expected`, with `-` for an unused argument. It is produced by
`python lab1/tests/gen_vectors.py` from Python integers with a fixed seed.
