#include    "set.h"

/** TODO
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./print_instr_set <instruction set.csv>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open instruction set `%s`\n", argv[1]);
        return 1;
    }
    const InstrSet instrs = read_instr_set(fp);
    fclose(fp);
    print_instr_set(&instrs);

    return 0;
}
