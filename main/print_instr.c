#include    <string.h>

#include    "set.h"

/**
 * Prints an instruction from an instruction set.
 *
 * @param argc
 * @param argv      ./print_instr <instr.csv> <instruction>
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 3) {
        fprintf(stderr, "usage: ./print_instr <instr.csv> <instruction>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open instruction set `%s`\n", argv[1]);
        return 1;
    }
    const StringSet skip = { .size = 0 };
    const InstrSet instrs = read_instr_set(fp, &skip);
    fclose(fp);
    for (int i = 0; i < instrs.size; ++i) {
        if (strcmp(instrs.instrs[i].opcode, argv[2]) == 0) {
            print_mask(&instrs.instrs[i].instr);
            break;
        }
    }

    return 0;
}
