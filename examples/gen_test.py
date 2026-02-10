#!/usr/bin/env python3
import sys
import random


def main() -> int:
    if len(sys.argv) < 2:
        print(f"Usage: {sys.argv[0]} <N>", file=sys.stderr)
        return 1

    try:
        n = int(sys.argv[1])
    except ValueError:
        print(f"Usage: {sys.argv[0]} <N>", file=sys.stderr)
        return 1

    print(n)
    for _ in range(n):
        print(random.randint(-10000, 10000))

    return 0


if __name__ == "__main__":
    raise SystemExit(main())

# python3 test_gen.py N
# python3 test_gen.py 1000 | ./build/bitonic_sort