#include    <stdlib.h>
#include    <string.h>
#include    <stdbool.h>
#include    "types.h"
#include    "set.h"

/*-INSTRUCTION-SET----------------------------------------------------------------------------------------------------*/

static Mask encode_(const char *instr) {                                        // encode mask
    Mask mask = { .val = 0u, .mask = 0u };
    for (int i = 0; i < IR_SIZE; ++i) {
        // original don't cares as 0s
        if (instr[i] == '1')    mask.val    |= (mask_t) (1u << i);
        if (instr[i] == 'x')    mask.mask   |= (mask_t) (1u << i);
    }
    return mask;
}

static char *decode_(const Mask mask) {                                         // mask to standard string
    char *instr = malloc(IR_SIZE + 1);
    if (!instr) {
        fprintf(stderr, "error allocating instruction\n");
        return NULL;
    }
    for (int i = 0; i < IR_SIZE; ++i) {
        if      (mask.mask & (1u << i))     instr[i] = 'x';
        else if (mask.val  & (1u << i))     instr[i] = '1';
        else                                instr[i] = '0';
    }
    instr[IR_SIZE] = '\0';
    return instr;
}

static void strip_spaces_(char *s) {                                            // strip spaces
    char *out = s;
    for (const char *in = s; *in != '\0'; ++in)
        if (*in != ' ') *out++ = *in;
    *out = '\0';
}

/**
 * Creates the instruction set from a file pointer.
 * My linter says this causes a memory leak, but I'm pretty sure it doesn't.
 *
 * @param fp        file pointer.
 * @return          instruction set.
 */
InstrSet instr_set(FILE *fp) {                                                  // create instruction set
    int             capacity    =   8;
    int             count       =  -1;
    const InstrSet  fail        =   { .instrs=NULL, .size=0 };

    // initial malloc
    Instr *instrs = malloc(capacity * sizeof(Instr));
    if (instrs == NULL) {
        fprintf(stderr, "error allocating instruction set\n");
        return fail;
    }

    // create instruction set
    char line[LINE_SIZE];
    for (int line_n = 1; fgets(line, LINE_SIZE, fp) != NULL; ++line_n) {
        // set line
        line[strcspn(line, "\n")] = '\0';

        // split with delimiter
        char *delim = strchr(line, DLM);
        if (!delim) {
            fprintf(stderr, "line %d: expected format `opcode,instruction`\n", line_n);
            free(instrs);
            return fail;
        }

        // strip spaces
        *delim              =   '\0';
        const char *opcode  =   line;
        char       *instr   =   delim + 1;
        strip_spaces_(instr);

        // add instruction
        if (++count >= capacity) {
            capacity    *=  2;
            Instr *new  =   realloc(instrs, capacity * sizeof(Instr));
            if (new == NULL) {
                fprintf(stderr, "line %d: error reallocating instruction set\n", line_n);
                free(instrs);
                return fail;
            }
            instrs = new;
        }
        snprintf(instrs[count].opcode,  OPCODE_SIZE + 1,    "%s",   opcode);
        instrs[count].instr = encode_(instr);
    }

    // successful read
    return (InstrSet){ .instrs=instrs, .size=count + 1 };
}

/**
 * Frees an instruction set.
 *
 * @param set       instruction set.
 */
void free_instrs(InstrSet *set) {
    for (int i = 0; i < set->size; ++i) {
        free(set->instrs);
    }
    free(set);
}

/**
 * Creates a set of strings.
 *
 * @param fp        file pointer.
 * @return          string set.
 */
StringSet string_set(FILE *fp) {                                                // write skip set
    StringSet set = { .size=0 };

    // create string set
    char line[OPCODE_SIZE + 1];
    for (; fgets(line, OPCODE_SIZE + 1, fp) != NULL; ++set.size) {
        // set line
        line[strcspn(line, "\n")] = '\0';
        strcpy(set.opcode[set.size], line);
    }

    return set;
}

static void print_colored_mask_(const char *str) {                              // uses coloring
    if (str == NULL)                    return;
    for (int i = 0; str[i] != '\0'; ++i) {
        if          (str[i] == 'x')     printf("\x1b[0m\x1b[2mx");
        else if     (str[i] == '1')     printf("\x1b[0m\x1b[92m1");
        else                            printf("\x1b[0m\x1b[31m0");
    }
    printf("\x1b[0m\n");
}

/**
 * Prints the instruction set, with don't cares as xs.
 *
 * @param set       instruction set.
 */
void print_instr_set(const InstrSet *set) {                                     // print instruction set
    for (int i = 0; i < set->size; ++i) {
        printf("%s", set->instrs[i].opcode);
        for (int j = 0; j < OPCODE_SIZE - strlen(set->instrs[i].opcode); ++j)    printf(" ");
        char *instr_str = decode_(set->instrs[i].instr);
        print_colored_mask_(instr_str);
        free(instr_str);
    }
}

static bool in_set_(const StringSet *set, const char *instr) {
    for (int i = 0; i < set->size; ++i) {
        if (strcmp(set->opcode[i], instr) == 0)  return true;
    }
    return false;
}

/**
 * Prints a subset of the instruction set, with don't cares as xs.
 *
 * @param set       instruction set.
 * @param subset    subset opcodes.
 */
void print_instr_subset(const InstrSet *set, const StringSet *subset) {
    for (int i = 0; i < set->size; ++i) {
        if (in_set_(subset, set->instrs[i].opcode)) {
            printf("%s", set->instrs[i].opcode);
            for (int j = 0; j < OPCODE_SIZE - strlen(set->instrs[i].opcode); ++j)    printf(" ");
            char *instr_str = decode_(set->instrs[i].instr);
            print_colored_mask_(instr_str);
            free(instr_str);
        }
    }
}

/**
 * Prints a mask, with don't cares as xs.
 *
 * @param mask      mask.
 */
void print_mask(const Mask *mask) {
    char *str = decode_(*mask);
    print_colored_mask_(str);
    free(str);
}

/**
 * Prints a mask set, with don't cares as xs.
 *
 * @param masks     mask set.
 */
void print_mask_set(const MaskSet *masks) {
    for (int i = 0; i < masks->size; ++i) {
        print_mask(&masks->bits[i]);
    }
}

/**
 * Writes the instruction set, with don't cares as xs.
 * Formatted as a CSV.
 *
 * @param set       instruction set.
 * @param fp        output file.
 */
void write_instr_set(const InstrSet *set, FILE *fp) {                           // write instruction set
    for (int i = 0; i < set->size; ++i) {
        fprintf(fp, "%s,", set->instrs[i].opcode);
        for (int j = 0; j < OPCODE_SIZE - strlen(set->instrs[i].opcode); ++j)    fprintf(fp, " ");
        char *instr_str = decode_(set->instrs[i].instr);
        fprintf(fp, "    %s\n", instr_str);
        free(instr_str);
    }
}

/**
 * Writes a mask, with don't cares as xs.
 *
 * @param mask      mask.
 * @param fp        output file.
 */
void write_mask(const Mask *mask, FILE *fp) {
    char *str = decode_(*mask);
    fprintf(fp, "%s", str);
    free(str);
}
