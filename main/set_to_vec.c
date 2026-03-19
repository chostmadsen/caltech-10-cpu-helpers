#include    "set.h"

/**
 * Prints a instruction set for able to use it.
 *
 * @param argc
 * @param argv      ./set_to_vec <instr.csv>
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./set_to_vec <instr.csv>\n");
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
    vec_instr_set(&instrs);

    return 0;
}
