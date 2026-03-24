#!/usr/bin/env python3
import sys
import random
import string
from pathlib import Path


def random_string(rng: random.Random, length: int, alphabet: str) -> str:
    return "".join(rng.choice(alphabet) for _ in range(length))


def count_overlapping(text: str, pattern: str) -> int:
    count = 0
    start = 0

    while True:
        pos = text.find(pattern, start)
        if pos == -1:
            break

        count += 1
        start = pos + 1

    return count


def build_patterns(
    rng: random.Random,
    text: str,
    pattern_count: int,
    min_pat_len: int,
    max_pat_len: int,
    alphabet: str,
    hit_percent: int,) -> list[str]:
    patterns: list[str] = []
    text_len = len(text)

    for _ in range(pattern_count):
        use_hit = text_len > 0 and rng.randint(1, 100) <= hit_percent

        if use_hit:
            pat_len = rng.randint(min_pat_len, min(max_pat_len, text_len))
            start = rng.randint(0, text_len - pat_len)
            pat = text[start:start + pat_len]
        else:
            pat_len = rng.randint(min_pat_len, max_pat_len)
            pat = random_string(rng, pat_len, alphabet)

        patterns.append(pat)

    return patterns


def write_dat(path: Path, text: str, patterns: list[str]) -> None:
    with path.open("w", encoding="utf-8") as f:
        f.write(f"{len(text)} {text}\n")
        f.write(f"{len(patterns)}\n")

        for pat in patterns:
            f.write(f"{len(pat)} {pat}\n")


def write_ans(path: Path, text: str, patterns: list[str]) -> None:
    with path.open("w", encoding="utf-8") as f:
        for i, pat in enumerate(patterns, start=1):
            cnt = count_overlapping(text, pat)
            f.write(f"{i} {cnt}\n")


def main() -> int:
    if len(sys.argv) < 6:
        print(
            f"Usage: {sys.argv[0]} <output_prefix> <text_len> <pattern_count> "
            f"<min_pat_len> <max_pat_len> [alphabet] [seed] [hit_percent]",
            file=sys.stderr,
        )
        return 1

    output_prefix = sys.argv[1]

    try:
        text_len = int(sys.argv[2])
        pattern_count = int(sys.argv[3])
        min_pat_len = int(sys.argv[4])
        max_pat_len = int(sys.argv[5])
    except ValueError:
        print("Numeric parameters are invalid", file=sys.stderr)
        return 1

    alphabet = sys.argv[6] if len(sys.argv) > 6 else string.ascii_lowercase
    seed = int(sys.argv[7]) if len(sys.argv) > 7 else None
    hit_percent = int(sys.argv[8]) if len(sys.argv) > 8 else 70

    if text_len < 0:
        print("text_len must be >= 0", file=sys.stderr)
        return 1

    if pattern_count < 0:
        print("pattern_count must be >= 0", file=sys.stderr)
        return 1

    if min_pat_len <= 0 or max_pat_len <= 0:
        print("Pattern lengths must be > 0", file=sys.stderr)
        return 1

    if min_pat_len > max_pat_len:
        print("min_pat_len must be <= max_pat_len", file=sys.stderr)
        return 1

    if not alphabet:
        print("alphabet must not be empty", file=sys.stderr)
        return 1

    if not (0 <= hit_percent <= 100):
        print("hit_percent must be in range [0, 100]", file=sys.stderr)
        return 1

    if text_len > 0 and min_pat_len > text_len and hit_percent > 0:
        print(
            "Warning: min_pat_len > text_len, so matching patterns cannot be sampled from text",
            file=sys.stderr,
        )

    rng = random.Random(seed)

    text = random_string(rng, text_len, alphabet)
    patterns = build_patterns(
        rng=rng,
        text=text,
        pattern_count=pattern_count,
        min_pat_len=min_pat_len,
        max_pat_len=max_pat_len,
        alphabet=alphabet,
        hit_percent=hit_percent,
    )

    dat_path = Path(output_prefix + ".dat")
    ans_path = Path(output_prefix + ".ans")

    write_dat(dat_path, text, patterns)
    write_ans(ans_path, text, patterns)

    print(f"Generated: {dat_path}")
    print(f"Generated: {ans_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())


# python3 gen_multi_match.py <output_prefix> <text_len> <pattern_count> <min_pat_len> <max_pat_len> [alphabet] [seed] [hit_percent]

# python3 gen_multi_match.py unit_tests/data/multi_match/common/test_1000 1000 100 2 8
