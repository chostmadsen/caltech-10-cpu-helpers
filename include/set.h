#ifndef     HELPERS_SET_H
#define     HELPERS_SET_H

#include    <stdio.h>
#include    "types.h"

InstrSet instr_set(FILE *fp);

void free_instrs(InstrSet *set);

StringSet string_set(FILE *fp);

void print_instr_set(const InstrSet *set);

void print_instr_subset(const InstrSet *set, const StringSet *subset);

void print_mask(const Mask *mask);

void print_mask_set(const MaskSet *masks);

void write_instr_set(const InstrSet *set, FILE *fp);

void write_mask(const Mask *mask, FILE *fp);

#endif   // HELPERS_SET_H
