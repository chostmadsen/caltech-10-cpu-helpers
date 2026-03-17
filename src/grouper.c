#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdbool.h>
#include    "grouper.h"

/*-LONE-GROUPER-------------------------------------------------------------------------------------------------------*/

static int num_x_(const Mask mask) {                                            // number of don't cares
    int num = 0;
    for (int i = 0; i < IR_SIZE; ++i) {
        if (mask.mask & (1u << i))      ++num;
    }
    return num;
}

static bool proper_subset_opcode_(const char *opcode, const InstrSet *set) {    // check if an element is in the set
    for (int i = 0; i < set->size; ++i)
        if (strcmp(set->instrs[i].opcode, opcode) == 0)     return true;
    return false;
}


static bool set_contains_(const char *new, const StringSet *cmp) {              // check if the set contains a string
    // set contains
    for (int i = 0; i < cmp->size; ++i) {
        if (strcmp(cmp->opcode[i], new) == 0)               return true;
    }
    return false;
}

static bool groups_(const Mask *mask, const InstrSet *set, const StringSet *subset) {   // check if subset groups
    for (int i = 0; i < set->size; ++i) {
        const Mask     *m           = &set->instrs[i].instr;
        const mask_t    not_mask    = ~mask->mask & ~m->mask;
        const bool      matches     = (mask->val & not_mask) == (m->val & not_mask);
        const bool      contains    = set_contains_(set->instrs[i].opcode, subset);

        // invalid group conditionals
        if ( matches && !contains)                          return false;
        if (!matches &&  contains)                          return false;
    }
    return true;
}

/**
 * Attempts to find one mask that groups a subset of the instruction set.
 *
 * @param set       instruction set.
 * @param subset    minimization group.
 * @return          group mask. NULL if fail.
 */
Mask *lone_group(const InstrSet *set, const StringSet *subset) {                // tries to group instructions
    // check subset
    for (int i = 0; i < subset->size; ++i) {
        if (!proper_subset_opcode_(subset->opcode[i], set)) {
            fprintf(stderr, "invalid subset\n");
            return NULL;
        }
    }
    // get reference seed
    int seed = -1;
    for (int i = 0; i < set->size; ++i) {
        if (set_contains_(set->instrs[i].opcode, subset)) {
            seed = i;
            break;
        }
    }
    if (seed == -1) {
        fprintf(stderr, "unable to seed subset\n");
        return NULL;
    }
    const Mask  ref         =   set->instrs[seed].instr;

    // attempt grouping
    bool        grouped     =   false;
    mask_t      mask_v      =   0u;
    Mask       *best        =   malloc(sizeof(Mask));
    int         max_x       =   0;
    do {
        const Mask masked = { .val=ref.val, .mask=mask_v };
        // no difference from reference
        if (!(~ref.mask | mask_v))              continue;

        // no improvement
        const int xs = num_x_(masked);
        if (grouped && xs <= max_x)             continue;

        if (!groups_(&masked, set, subset))     continue;
        // set best
        memcpy(best, &masked, sizeof(Mask));
        max_x   =   xs;
        grouped =   true;
    } while (mask_v++ < MAX);

    // return
    if (!grouped) {
        free(best);
        return NULL;
    }
    return best;
}
