#include    <stdlib.h>

#include    "set.h"
#include    "grouper.h"

/**
 * Finds the largest single bounding box required to group a list of opcodes based on an instruction set.
 * Uses a skip file to set more don't cares.
 *
 * @param argc
 * @param argv      ./group <instr.csv> <skip.txt> <group.txt> ?<out.txt>
 */
int main(const int argc, const char **argv) {
    // main
    // main
    if (argc < 4) {
        fprintf(stderr, "usage: ./group_auto <instr.csv> <skip.txt> <group.txt> ?<out.txt>\n");
        return 1;
    }

    // create skip set
    FILE *fp_skip = fopen(argv[2], "r");
    if (fp_skip == NULL) {
        fprintf(stderr, "unable to open skip set `%s`\n", argv[2]);
        return 1;
    }
    const StringSet skip = read_string_set(fp_skip);
    fclose(fp_skip);

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open instruction set `%s`\n", argv[1]);
        return 1;
    }
    const InstrSet instrs = read_instr_set(fp, &skip);
    fclose(fp);

    // create group string set
    FILE *fp_g = fopen(argv[3], "r");
    if (fp_g == NULL) {
        fprintf(stderr, "unable to open group set `%s`\n", argv[3]);
        return 1;
    }
    StringSet subset = read_string_set(fp_g);
    fclose(fp_g);

    const MaskSet *res = auto_group(&instrs, &subset);
    for (int i = 0; i < res->size; i++) {
        print_mask_ln(&res->masks[i]);
    }

    // write boolean expression
    if (argc == 5) {
        FILE *fp_out = fopen(argv[4], "w");
        if (fp_out == NULL) {
            fprintf(stderr, "unable to open output file `%s`\n", argv[3]);
            return 1;
        }
        write_masks(res, fp_out);
        fclose(fp_out);
    }

    return 0;
}