#ifndef     HELPERS_SET_H
#define     HELPERS_SET_H

#include    <stdio.h>
#include    "types.h"

InstrSet read_instr_set(FILE *fp, const StringSet *skip);

StringSet read_string_set(FILE *fp);

Mask read_mask(FILE *fp);

MaskSet *read_masks(FILE *fp);

void free_instrs(InstrSet *set);

void print_instr_set(const InstrSet *set);

void vec_instr_set(const InstrSet *set);

void print_mask_ln(const Mask *mask);

void print_mask(const Mask *mask);

void print_masks_ln(const MaskSet *masks);

void print_masks(const MaskSet *masks);

void write_instr_set(const InstrSet *set, FILE *fp);

void write_mask(const Mask *mask, FILE *fp);

void write_masks(const MaskSet *masks, FILE *fp);

#endif   // HELPERS_SET_H
