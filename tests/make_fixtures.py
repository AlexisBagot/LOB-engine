#!/usr/bin/env python3
import os

HERE = os.path.dirname(os.path.abspath(__file__))
FIX = os.path.join(HERE, "fixtures")
EXP = os.path.join(HERE, "expected")

S = "000c" + "53" + "0000" + "0000" + "0a2df4921d67" + "4f"
R = (
    "0027"
    + "52"
    + "0001"
    + "0000"
    + "0a66a0e0dc44"
    + "41"
    + "20" * 7
    + "4e"
    + "20"
    + "00000064"
    + "4e"
    + "43"
    + "5a"
    + "20"
    + "50"
    + "4e"
    + "20"
    + "31"
    + "4e"
    + "00000000"
    + "4e"
)
S_UNKNOWN = "000c" + "ff" + "0000" + "0000" + "0a2df4921d67" + "4f"
S_TRUNC = "000c" + "53" + "0000" + "0000" + "0a2df4"

CASES = {
    "empty": "",
    "one_byte": "00",
    "prefix_only": "000c",
    "single_s": S,
    "truncated_body": S_TRUNC,
    "two_same": S + S,
    "two_mixed": S + R,
    "trailing_junk": S + R + "00",
    "unknown_type": S_UNKNOWN,
    "valid_then_truncated": S + S_TRUNC,
}

os.makedirs(FIX, exist_ok=True)
os.makedirs(EXP, exist_ok=True)
for name, hexstr in CASES.items():
    raw = bytes.fromhex(hexstr)
    with open(os.path.join(FIX, name + ".itch"), "wb") as f:
        f.write(raw)
    exp = os.path.join(EXP, name + ".txt")
    if not os.path.exists(exp):
        with open(exp, "w") as f:
            f.write("TODO\n")
    print("%-24s %4d bytes" % (name + ".itch", len(raw)))
