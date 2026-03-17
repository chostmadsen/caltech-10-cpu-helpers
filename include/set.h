#ifndef     HELPERS_SET_H
#define     HELPERS_SET_H

#include    <stdio.h>
#include    "types.h"

InstrSet read_instr_set(FILE *fp);

StringSet read_string_set(FILE *fp);

Mask read_mask(FILE *fp);

void free_instrs(InstrSet *set);

void print_instr_set(const InstrSet *set);

void print_mask(const Mask *mask);

void write_instr_set(const InstrSet *set, FILE *fp);

void write_mask(const Mask *mask, FILE *fp);

#endif   // HELPERS_SET_H
