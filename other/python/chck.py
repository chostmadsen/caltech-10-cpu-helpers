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
    ("ADC m",      "01100000XXXXXXXX"),
    ("ADC X, o",   "01100001XXXXXXXX"),
    ("ADC S, o",   "01100010XXXXXXXX"),
    ("ADCI k",     "01100011XXXXXXXX"),
    ("ADD m",      "01101000XXXXXXXX"),
    ("ADD X, o",   "01101001XXXXXXXX"),
    ("ADD S, o",   "01101010XXXXXXXX"),
    ("ADDI k",     "01101011XXXXXXXX"),
    ("AND m",      "01000100XXXXXXXX"),
    ("AND X, o",   "01000101XXXXXXXX"),
    ("AND S, o",   "01000110XXXXXXXX"),
    ("ANDI k",     "01000111XXXXXXXX"),
    ("ASR",        "0111000100000001"),
    ("CMP m",      "00110000XXXXXXXX"),
    ("CMP X, o",   "00110001XXXXXXXX"),
    ("CMP S, o",   "00110010XXXXXXXX"),
    ("CMPI k",     "00110011XXXXXXXX"),
    ("DEC",        "0111101100000000"),
    ("INC",        "0000000000000000"),
    ("LSL",        "0101100000000000"),
    ("LSR",        "0111000100000000"),
    ("NEG",        "0010011100000000"),
    ("NOT",        "0010110100000000"),
    ("OR m",       "01110100XXXXXXXX"),
    ("OR X, o",    "01110101XXXXXXXX"),
    ("OR S, o",    "01110110XXXXXXXX"),
    ("ORI k",      "01110111XXXXXXXX"),
    ("RLC",        "0101000000000000"),
    ("ROL",        "0101001000000000"),
    ("ROR",        "0111000100000010"),
    ("RRC",        "0111000100000011"),
    ("SBB m",      "00011000XXXXXXXX"),
    ("SBB X, o",   "00011001XXXXXXXX"),
    ("SBB S, o",   "00011010XXXXXXXX"),
    ("SBBI k",     "00011011XXXXXXXX"),
    ("SUB m",      "00010000XXXXXXXX"),
    ("SUB X, o",   "00010001XXXXXXXX"),
    ("SUB S, o",   "00010010XXXXXXXX"),
    ("SUBI k",     "00010011XXXXXXXX"),
    ("TST m",      "01001100XXXXXXXX"),
    ("TST X, o",   "01001101XXXXXXXX"),
    ("TST S, o",   "01001110XXXXXXXX"),
    ("TSTI k",     "01001111XXXXXXXX"),
    ("XOR m",      "00110100XXXXXXXX"),
    ("XOR X, o",   "00110101XXXXXXXX"),
    ("XOR S, o",   "00110110XXXXXXXX"),
    ("XORI k",     "00110111XXXXXXXX"),

    # Set/Clear flag
    ("STI",        "0111111110000001"),
    ("CLI",        "0000011101101001"),
    ("STU",        "0111111100100010"),
    ("CLU",        "0000011111001010"),
    ("STC",        "0111111100001100"),
    ("CLC",        "0000011111100100"),

    # Index register
    ("TAX",        "0000011110000000"),
    ("TXA",        "0110011100000001"),
    ("INX",        "0000010110000000"),
    ("DEX",        "0000110110000000"),
    ("TAS",        "0000011101010000"),
    ("TSA",        "0110011100000000"),
    ("INS",        "0000011001000000"),
    ("DES",        "0000111001000000"),

    # Load
    ("LDI k",      "10001001XXXXXXXX"),
    ("LDD m",      "10000000XXXXXXXX"),
    ("LD X+o",     "10010111XXXXXXXX"),
    ("LD X++o",    "10010110XXXXXXXX"),
    ("LD X-+o",    "10011110XXXXXXXX"),
    ("LD +X+o",    "10000110XXXXXXXX"),
    ("LD -X+o",    "10001110XXXXXXXX"),
    ("LD S+o",     "10010011XXXXXXXX"),
    ("LD S++o",    "10010010XXXXXXXX"),
    ("LD S-+o",    "10011010XXXXXXXX"),
    ("LD +S+o",    "10000010XXXXXXXX"),
    ("LD -S+o",    "10001010XXXXXXXX"),

    # Store
    ("STD m",      "10100000XXXXXXXX"),
    ("ST X+o",     "10110111XXXXXXXX"),
    ("ST X++o",    "10110110XXXXXXXX"),
    ("ST X-+o",    "10111110XXXXXXXX"),
    ("ST +X+o",    "10100110XXXXXXXX"),
    ("ST -X+o",    "10101110XXXXXXXX"),
    ("ST S+o",     "10110011XXXXXXXX"),
    ("ST S++o",    "10110010XXXXXXXX"),
    ("ST S-+o",    "10111010XXXXXXXX"),
    ("ST +S+o",    "10100010XXXXXXXX"),
    ("ST -S+o",    "10101010XXXXXXXX"),

    # Branch
    ("JMP a",      "110XXXXXXXXXXXXX"),
    ("JA r",       "10001000XXXXXXXX"),
    ("JAE/JNC r",  "10001100XXXXXXXX"),
    ("JB/JC r",    "10001111XXXXXXXX"),
    ("JBE r",      "10001011XXXXXXXX"),
    ("JE/JZ r",    "10011111XXXXXXXX"),
    ("JG r",       "10101111XXXXXXXX"),
    ("JGE r",      "10111011XXXXXXXX"),
    ("JL r",       "10111000XXXXXXXX"),
    ("JLE r",      "10101100XXXXXXXX"),
    ("JNE/JNZ r",  "10011100XXXXXXXX"),
    ("JNS r",      "10011000XXXXXXXX"),
    ("JNU r",      "10111100XXXXXXXX"),
    ("JNV r",      "10101000XXXXXXXX"),
    ("JS r",       "10011011XXXXXXXX"),
    ("JU r",       "10111111XXXXXXXX"),
    ("JV r",       "10101011XXXXXXXX"),

    # Subroutine / stack
    ("CALL a",     "111XXXXXXXXXXXXX"),
    ("RTS",        "0001111100000000"),
    ("POPF",       "0000001000000000"),
    ("PUSHF",      "0000111000000000"),

    # I/O
    ("IN p",       "10010000XXXXXXXX"),
    ("OUT p",      "10110000XXXXXXXX"),

    # Misc
    ("NOP",        "0001111110000000"),
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
        bit_num = 15 - i
        terms.append(f" ir_{bit_num}" if c == '1' else f"!ir_{bit_num}")
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
        if len(upper) > 16:
            print(f"Error: pattern \"{raw}\" is {len(upper)} bits — must be at most 16.",
                  file=sys.stderr)
            had_error = True
            continue
        if len(upper) < 16:
            print(f"Warning: pattern \"{raw}\" is fewer than 16 bits, "
                  f"padded with X's on the right.")
            upper = upper.ljust(16, 'X')
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
