#include    "set.h"

/**
 * Prints an instruction set.
 *
 * @param argc
 * @param argv      ./print_instrs <instr.csv>
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./print_instr_set <instr.csv>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open instruction set `%s`\n", argv[1]);
        return 1;
    }
    const StringSet skip = { .size=0 };
    const InstrSet instrs = read_instr_set(fp, &skip);
    fclose(fp);
    print_instr_set(&instrs);

    return 0;
}
