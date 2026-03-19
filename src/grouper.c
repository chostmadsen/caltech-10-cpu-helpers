#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h>
#include    <stdbool.h>
#include    "grouper.h"
#include    "minimizer.h"
#include    "set.h"

/*-LONE-GROUPER-------------------------------------------------------------------------------------------------------*/

static int num_x_(const Mask mask) {                                            // number of don't cares
    int num = 0;
    for (int i = 0; i < IR_SIZE; ++i) {
        if (mask.mask & (1u << i))      ++num;
    }
    return num;
}

static bool set_contains_opcode_(const char *opcode, const InstrSet *set) {     // check if an element is in the set
    for (int i = 0; i < set->size; ++i)
        if (strcmp(set->instrs[i].opcode, opcode) == 0)     return true;
    return false;
}


static bool strset_contains_itm_(const char *new, const StringSet *cmp) {       // check if the set contains a string
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
        const bool      contains    = strset_contains_itm_(set->instrs[i].opcode, subset);

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
        if (!set_contains_opcode_(subset->opcode[i], set)) {
            fprintf(stderr, "invalid subset; invalid opcode `%s`\n", subset->opcode[i]);
            return NULL;
        }
    }
    // get reference seed
    int seed = -1;
    for (int i = 0; i < set->size; ++i) {
        if (strset_contains_itm_(set->instrs[i].opcode, subset)) {
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
        // if (!(~ref.mask | mask_v))              continue;

        // no improvement
        const int xs = num_x_(masked);
        if (grouped && xs <= max_x)             continue;

        if (!groups_(&masked, set, subset))     continue;
        // set best
        memcpy(best, &masked, sizeof(Mask));
        max_x   =   xs;
        grouped =   true;
    } while (mask_v++ < MAX_N);

    // return
    if (!grouped) {
        return NULL;
    }
    return best;
}

/*-FULL-GROUPER-------------------------------------------------------------------------------------------------------*/

Mask *get_mask(const InstrSet *set, const char *opcode) {
    for (int i = 0; i < set->size; ++i) {
        if (strcmp(set->instrs[i].opcode, opcode) == 0)         return &set->instrs[i].instr;
    }
    return NULL;
}

static bool conflicts_(const Mask *mask, const InstrSet *set) {                 // check mask set matching
    for (int i = 0; i < set->size; ++i) {
        const mask_t not_mask = ~mask->mask & ~set->instrs[i].instr.mask;
        if ((mask->val & not_mask) == (set->instrs[i].instr.val & not_mask))    return true;
    }
    return false;
}

static int groups_n_(const Mask *mask, const InstrSet *set) {
    int num = 0;
    for (int i = 0; i < set->size; ++i) {
        const mask_t not_mask = ~mask->mask & ~set->instrs[i].instr.mask;
        if ((mask->val & not_mask) == (set->instrs[i].instr.val & not_mask))    ++num;
    }
    return num;
}

static Mask *single_min_(const InstrSet *set, const Mask *org_mask) {           // minimize one mask
    mask_t      mask    =   0u;
    Mask       *best    =   malloc(sizeof(Mask));
    *best               =  *org_mask;
    int         max_x   =   num_x_(*org_mask);

    do {
        const Mask masked = { .val=org_mask->val, .mask=org_mask->mask | mask };
        const int xs = num_x_(masked);
        if (xs <= max_x)                    continue;
        if (conflicts_(&masked, set))       continue;

        // no conflict
        memcpy(best, &masked, sizeof(Mask));
        max_x   =   xs;
    } while (mask++ < MAX_N);

    return best;
}

static Mask *best_group_(const InstrSet *set, const StringSet *subset, const InstrSet *cleaned) {
    // minimize subset opcode
    int     best_g      =   0;
    Mask   *best        =   malloc(sizeof(Mask));
    for (int i = 0; i < subset->size; ++i) {
        Mask   *min             =   single_min_(cleaned, get_mask(set, subset->opcode[i]));
        const int num_groups    =   groups_n_(min, set);
        if (num_groups <= best_g)   continue;
        best_g                  =   num_groups;
        memcpy(best, min, sizeof(Mask));
    }

    // free and return
    return best;
}

static void clean_subset_(const Mask *mask, const InstrSet *set, StringSet *substring) {
    StringSet res = { .size=0 };

    for (int i = 0; i < set->size; ++i) {
        if (!(strset_contains_itm_(set->instrs[i].opcode, substring)))          continue;

        const mask_t not_mask = ~mask->mask & ~set->instrs[i].instr.mask;
        if ((mask->val & not_mask) == (set->instrs[i].instr.val & not_mask))    continue;
        strncpy(res.opcode[res.size++], set->instrs[i].opcode, OPCODE_SIZE);
    }

    *substring = res;
}

MaskSet *full_group(const InstrSet *set, StringSet *subset) {             // heuristically finds optimal groups
    // create new instruction set
    int         capacity    =   8;
    int         count       =  -1;

    Instr *instrs = malloc(capacity * sizeof(Instr));
    for (int i = 0; i < set->size; ++i) {
        if (strset_contains_itm_(set->instrs[i].opcode, subset))       continue;

        // add instruction
        if (++count >= capacity) {
            capacity    *=  2;
            Instr *new  =   realloc(instrs, capacity * sizeof(Instr));
            if (new == NULL) {
                fprintf(stderr, "error reallocating instruction set\n");
                return NULL;
            }
            instrs = new;
        }
        instrs[count]       =   set->instrs[i];
    }
    const InstrSet cleaned  =  (InstrSet){ .instrs=instrs, .size=count + 1 };

    // create res
    int         mask_cap    =   4;
    int         mask_count  =  -1;
    Mask       *masks       =   malloc(mask_cap * sizeof(Mask));

    // continuously group out
    while (subset->size > 0) {
        const Mask *best    =   best_group_(set, subset, &cleaned);
        if (best == NULL) {
            fprintf(stderr, "if you're seeing this, you've fucked up\n");
            free(instrs); free(masks);
            return NULL;
        }
        if (++mask_count >= mask_cap) {
            mask_cap    *=  2;
            Mask *new   =   realloc(masks, mask_cap * sizeof(Mask));
            if (new == NULL) {
                fprintf(stderr, "error reallocating mask set\n");
                free(instrs); free(masks);
                return NULL;
            }
            masks               =   new;
        }
        masks[mask_count]   =  *best;
        clean_subset_(best, set, subset);
    }

    MaskSet *res    =   malloc(sizeof(MaskSet));
    res->masks      =   masks;
    res->size       =   mask_count + 1;
    return res;
}
