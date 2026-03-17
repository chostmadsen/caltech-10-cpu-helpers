#include    <stdio.h>
#include    <stdlib.h>
#include    <stdbool.h>
#include    <string.h>
#include    "types.h"
#include    "minimizer.h"

/*-LONE-MINIMIZER-----------------------------------------------------------------------------------------------------*/

static int num_x_(const Mask mask) {                                            // number of don't cares
    int num = 0;
    for (int i = 0; i < IR_SIZE; ++i) {
        if (mask.mask & (1u << i))      ++num;
    }
    return num;
}

static bool conflicts_(const Mask mask, const InstrSet set, const int idx) {    // check mask set matching
    for (int i = 0; i < set.size; ++i) {
        if (i == idx)       continue;
        const mask_t not_mask = ~mask.mask & ~set.instrs[i].instr.mask;
        if ((mask.val & not_mask) == (set.instrs[i].instr.val & not_mask))      return true;
    }
    return false;
}

/**
 * Assuming all instructions are given in full-form, determines the best way to recognize each instruction with the
 * minimum amount of terms anded together.
 * Changes instruction set to match minimization.
 *
 * @param set       instruction set.
 * @return          status code.
 * */
int lone_min(const InstrSet *set) {                                             // full instruction minimization
    // create org
    Instr *org_instrs = malloc(set->size * sizeof(Instr));
    if (org_instrs == NULL) {
        fprintf(stderr, "unable to allocate memory for original instruction set reference\n");
        return 1;
    }
    memcpy(org_instrs, set->instrs, set->size * sizeof(Instr));
    const InstrSet org = { .instrs=org_instrs, .size=set->size };

    // minimize
    for (int i = 0; i < set->size; ++i) {
        Mask       *org_m   = &set->instrs[i].instr;
        const Mask  org_mc  = org.instrs[i].instr;

        mask_t      mask    =   0u;
        Mask        best    =  *org_m;
        int         max_x   =   num_x_(*org_m);
        do {
            const Mask masked = { .val=org_mc.val, .mask=org_mc.mask | mask };
            const int xs = num_x_(masked);
            if (xs <= max_x)                    continue;
            if (conflicts_(masked, org, i))     continue;

            // no conflict
            best    =   masked;
            max_x   =   xs;
        } while (mask++ < MAX);
        *org_m = best;
    }

    free(org_instrs);
    return 0;
}
