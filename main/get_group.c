#include    "set.h"
#include    "minimizer.h"

/** TODO
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./get_group <group file.txt> <instruction file.csv>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[2], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open input instruction set `%s`\n", argv[1]);
        return 1;
    }
    const InstrSet instrs = instr_set(fp);
    fclose(fp);

    // create group string set
    FILE *fp_g = fopen(argv[1], "r");
    if (fp_g == NULL) {
        fprintf(stderr, "unable to open group set `%s`\n", argv[2]);
        return 1;
    }
    const StringSet subset = string_set(fp_g);
    fclose(fp_g);

    print_instr_subset(&instrs, &subset);

    return 0;
}
