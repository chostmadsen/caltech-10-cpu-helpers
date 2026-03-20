#!/usr/bin/env python3

"""
chkdecod.py: Caltech10 CPU Instruction Set Decoder

See https://wolverine.caltech.edu/tools/chkdecod.htm.

Usage: chkdecod.py bit_pattern [bit_pattern ...]

Each bit_pattern is up to 16 characters of 0, 1, or X (don't care).
Patterns shorter than 16 bits are padded with X's on the right.

Revision History:
03/11/2026  Chris Hiszpanski  Initial port from chkdecod.exe.
"""

import sys

# Instruction table
# Each pattern is exactly 16 chars of '0', '1', or 'X' (don't care).
# Bit 15 is the leftmost character; bit 0 is the rightmost.
INSTRUCTIONS = [
    # ALU
    ("ADC m",      "000101100000XXXXXXXX"),
    ("ADC X, o",   "000101100001XXXXXXXX"),
    ("ADC S, o",   "000101100010XXXXXXXX"),
    ("ADCI k",     "000101100011XXXXXXXX"),
    ("ADD m",      "000101101000XXXXXXXX"),
    ("ADD X, o",   "000101101001XXXXXXXX"),
    ("ADD S, o",   "000101101010XXXXXXXX"),
    ("ADDI k",     "000101101011XXXXXXXX"),
    ("AND m",      "000101000100XXXXXXXX"),
    ("AND X, o",   "000101000101XXXXXXXX"),
    ("AND S, o",   "000101000110XXXXXXXX"),
    ("ANDI k",     "000101000111XXXXXXXX"),
    ("ASR",        "00010111000100000001"),
    ("CMP m",      "000100110000XXXXXXXX"),
    ("CMP X, o",   "000100110001XXXXXXXX"),
    ("CMP S, o",   "000100110010XXXXXXXX"),
    ("CMPI k",     "000100110011XXXXXXXX"),
    ("DEC",        "00010111101100000000"),
    ("INC",        "00010000000000000000"),
    ("LSL",        "00010101100000000000"),
    ("LSR",        "00010111000100000000"),
    ("NEG",        "00010010011100000000"),
    ("NOT",        "00010010110100000000"),
    ("OR m",       "000101110100XXXXXXXX"),
    ("OR X, o",    "000101110101XXXXXXXX"),
    ("OR S, o",    "000101110110XXXXXXXX"),
    ("ORI k",      "000101110111XXXXXXXX"),
    ("RLC",        "00010101000000000000"),
    ("ROL",        "00010101001000000000"),
    ("ROR",        "00010111000100000010"),
    ("RRC",        "00010111000100000011"),
    ("SBB m",      "000100011000XXXXXXXX"),
    ("SBB X, o",   "000100011001XXXXXXXX"),
    ("SBB S, o",   "000100011010XXXXXXXX"),
    ("SBBI k",     "000100011011XXXXXXXX"),
    ("SUB m",      "000100010000XXXXXXXX"),
    ("SUB X, o",   "000100010001XXXXXXXX"),
    ("SUB S, o",   "000100010010XXXXXXXX"),
    ("SUBI k",     "000100010011XXXXXXXX"),
    ("TST m",      "000101001100XXXXXXXX"),
    ("TST X, o",   "000101001101XXXXXXXX"),
    ("TST S, o",   "000101001110XXXXXXXX"),
    ("TSTI k",     "000101001111XXXXXXXX"),
    ("XOR m",      "000100110100XXXXXXXX"),
    ("XOR X, o",   "000100110101XXXXXXXX"),
    ("XOR S, o",   "000100110110XXXXXXXX"),
    ("XORI k",     "000100110111XXXXXXXX"),

    # Set/Clear fla"0001
    ("STI",        "00010111111110000001"),
    ("CLI",        "00010000011101101001"),
    ("STU",        "00010111111100100010"),
    ("CLU",        "00010000011111001010"),
    ("STC",        "00010111111100001100"),
    ("CLC",        "00010000011111100100"),

    # Index registe"0001
    ("TAX",        "00010000011110000000"),
    ("TXA",        "00010110011100000001"),
    ("INX",        "00010000010110000000"),
    ("DEX",        "00010000110110000000"),
    ("TAS",        "00010000011101010000"),
    ("TSA",        "00010110011100000000"),
    ("INS",        "00010000011001000000"),
    ("DES",        "00010000111001000000"),

    # Load
    ("LDI k",      "000110001001XXXXXXXX"),
    ("LDD m",      "000110000000XXXXXXXX"),
    ("LD X+o",     "000110010111XXXXXXXX"),
    ("LD X++o",    "000110010110XXXXXXXX"),
    ("LD X-+o",    "000110011110XXXXXXXX"),
    ("LD +X+o",    "000110000110XXXXXXXX"),
    ("LD -X+o",    "000110001110XXXXXXXX"),
    ("LD S+o",     "000110010011XXXXXXXX"),
    ("LD S++o",    "000110010010XXXXXXXX"),
    ("LD S-+o",    "000110011010XXXXXXXX"),
    ("LD +S+o",    "000110000010XXXXXXXX"),
    ("LD -S+o",    "000110001010XXXXXXXX"),

    # Store
    ("STD m",      "000110100000XXXXXXXX"),
    ("ST X+o",     "000110110111XXXXXXXX"),
    ("ST X++o",    "000110110110XXXXXXXX"),
    ("ST X-+o",    "000110111110XXXXXXXX"),
    ("ST +X+o",    "000110100110XXXXXXXX"),
    ("ST -X+o",    "000110101110XXXXXXXX"),
    ("ST S+o",     "000110110011XXXXXXXX"),
    ("ST S++o",    "000110110010XXXXXXXX"),
    ("ST S-+o",    "000110111010XXXXXXXX"),
    ("ST +S+o",    "000110100010XXXXXXXX"),
    ("ST -S+o",    "000110101010XXXXXXXX"),

    # Branch
    ("JMP a",      "0001110XXXXXXXXXXXXX"),
    ("JA r",       "000110001000XXXXXXXX"),
    ("JAE/JNC r",  "000110001100XXXXXXXX"),
    ("JB/JC r",    "000110001111XXXXXXXX"),
    ("JBE r",      "000110001011XXXXXXXX"),
    ("JE/JZ r",    "000110011111XXXXXXXX"),
    ("JG r",       "000110101111XXXXXXXX"),
    ("JGE r",      "000110111011XXXXXXXX"),
    ("JL r",       "000110111000XXXXXXXX"),
    ("JLE r",      "000110101100XXXXXXXX"),
    ("JNE/JNZ r",  "000110011100XXXXXXXX"),
    ("JNS r",      "000110011000XXXXXXXX"),
    ("JNU r",      "000110111100XXXXXXXX"),
    ("JNV r",      "000110101000XXXXXXXX"),
    ("JS r",       "000110011011XXXXXXXX"),
    ("JU r",       "000110111111XXXXXXXX"),
    ("JV r",       "000110101011XXXXXXXX"),

    # Subroutine / "0001tack
    ("CALL i a",   "0001111XXXXXXXXXXXXX"),
    ("CALL 3 a",   "1000111XXXXXXXXXXXXX"),
    ("CALL 2 a",   "0100111XXXXXXXXXXXXX"),
    ("CALL 1 a",   "0010111XXXXXXXXXXXXX"),
    ("RTS i",      "00010001111100000000"),
    ("RTS 2",      "01000001111100000000"),
    ("RTS 1",      "00100001111100000000"),
    ("POPF",       "00010000001000000000"),
    ("PUSHF",      "00010000111000000000"),

    # I/O
    ("IN p",       "000110010000XXXXXXXX"),
    ("OUT p",      "000110110000XXXXXXXX"),

    # Misc
    ("NOP",        "00010001111110000000"),
]

def patterns_overlap(a, b):
    """Return True if two 16-char 0/1/X patterns share at least one common value."""
    for ca, cb in zip(a, b):
        if ca != 'X' and cb != 'X' and ca != cb:
            return False
    return True

def abel_term(pattern):
    """Build an ABEL product term for the fixed bits of a pattern."""
    terms = []
    for i, c in enumerate(pattern):
        if c == 'X':
            continue
        bit_num = 19 - i
        if (bit_num < 16):
            terms.append(f" ir_{bit_num}" if c == '1' else f"!ir_{bit_num}")
        else:
            if (bit_num == 19):
                terms.append(" stall_3" if c == "1" else "!stall_3")
            if (bit_num == 18):
                terms.append(" stall_2" if c == "1" else "!stall_2")
            if (bit_num == 17):
                terms.append(" stall_1" if c == "1" else "!stall_1")
            if (bit_num == 16):
                terms.append(" idle" if c == "1" else "!idle")
    return ' & '.join(terms) if terms else '1'

def parse_patterns(args):
    """
    Validate and pad raw argument strings.
    Returns (patterns, had_error) where patterns is a list of 16-char strings.
    Prints warnings/errors to stderr as it goes.
    """
    patterns = []
    had_error = False

    for raw in args:
        upper = raw.upper()
        if not all(c in '01X' for c in upper):
            print(f"Error: invalid pattern \"{raw}\" — only 0, 1, and X are allowed.",
                  file=sys.stderr)
            had_error = True
            continue
        if len(upper) > 20:
            print(f"Error: pattern \"{raw}\" is {len(upper)} bits — must be at most 20.",
                  file=sys.stderr)
            had_error = True
            continue
        if len(upper) < 20:
            print(f"Warning: pattern \"{raw}\" is fewer than 20 bits, "
                  f"padded with X's on the right.")
            upper = upper.ljust(20, 'X')
        patterns.append(upper)

    return patterns, had_error

def main():
    if len(sys.argv) < 2 or sys.argv[1] in ('-h', '--help'):
        print(__doc__.strip())
        sys.exit(0)

    patterns, had_error = parse_patterns(sys.argv[1:])

    if not patterns:
        if not had_error:
            print("Error: no patterns specified.", file=sys.stderr)
        sys.exit(1)

    seen = set()
    matched = []
    for name, instr_pat in INSTRUCTIONS:
        if name in seen:
            continue
        if any(patterns_overlap(p, instr_pat) for p in patterns):
            matched.append((name, instr_pat))
            seen.add(name)

    if matched:
        print("\nMatching Instructions:")
        col = max(len(name) for name, _ in matched)
        for name, instr_pat in matched:
            print(f"  {name:<{col}}  {instr_pat}")
    else:
        print("\nNo instructions match the given patterns.")

    terms = [f"({abel_term(p)} )" for p in patterns]
    print(f"\nABEL Equation:\n{' # '.join(terms)};")

if __name__ == "__main__":
    main()
