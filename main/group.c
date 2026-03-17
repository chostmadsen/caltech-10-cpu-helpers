#include    <stdio.h>
#include    <stdlib.h>

#include    "set.h"
#include    "grouper.h"

/** TODO
 */
int main(const int argc, const char **argv) {                                   // main
    // main
    if (argc < 3) {
        fprintf(stderr, "usage: ./group_minimizer <instruction file.csv> <group file.txt> ?<out file.txt>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open input instruction set `%s`\n", argv[1]);
        return 1;
    }
    const InstrSet instrs = instr_set(fp);
    fclose(fp);

    // create group string set
    FILE *fp_g = fopen(argv[2], "r");
    if (fp_g == NULL) {
        fprintf(stderr, "unable to open group set `%s`\n", argv[2]);
        return 1;
    }
    const StringSet subset = string_set(fp_g);
    fclose(fp_g);

    // minimize instructions
    Mask *min_eq = lone_group(&instrs, &subset);
    if (min_eq == NULL) {
        fprintf(stderr, "unable to group subset; split subset and try again\n");
        return 1;
    }
    print_mask(min_eq);

    // write boolean expression
    if (argc == 4) {
        FILE *fp_out = fopen(argv[3], "w");
        if (fp_out == NULL) {
            fprintf(stderr, "unable to open output file `%s`\n", argv[3]);
            return 1;
        }
        write_mask(min_eq, fp_out);
        fclose(fp_out);
    }
    free(min_eq);

    return 0;
}
