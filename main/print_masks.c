#include    "set.h"

/**
 * Prints a mask.
 *
 * @param argc
 * @param argv      ./print_mask <mask.txt>
 */
int main(const int argc, const char **argv) {                                   // main
    if (argc < 2) {
        fprintf(stderr, "usage: ./print_mask <mask.txt>\n");
        return 1;
    }

    // create instruction set
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        fprintf(stderr, "unable to open mask `%s`\n", argv[1]);
        return 1;
    }
    const MaskSet *mask = read_masks(fp);
    fclose(fp);
    print_masks(mask);

    return 0;
}
