#include    "set.h"
#include    "minimizer.h"

/** TODO
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./lone_minimizer <instruction file.csv> ?<out file.csv>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open input instruction set `%s`\n", argv[1]);
        return 1;
    }
    const InstrSet instrs = read_instr_set(fp);
    fclose(fp);

    // minimize instructions
    lone_min(&instrs);
    print_instr_set(&instrs);

    // write instruction set
    if (argc == 3) {
        FILE *fp_out = fopen(argv[2], "w");
        if (fp_out == NULL) {
            fprintf(stderr, "unable to open output instruction set `%s`\n", argv[3]);
            return 1;
        }
        write_instr_set(&instrs, fp_out);
        fclose(fp_out);
    }

    return 0;
}
