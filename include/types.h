#ifndef     HELPERS_TYPES_H
#define     HELPERS_TYPES_H

#include    <stdint.h>

// for file reading
#define     LINE_SIZE       127
#define     OPCODE_SIZE     15
#define     OPCODE_NUM      256
#define     DLM             ','

// for code minimization
#define     IR_SIZE         16
typedef     uint16_t        mask_t;
#define     MAX             UINT16_MAX
#define     MAX_DEPTH       5;

/*-STRUCTS------------------------------------------------------------------------------------------------------------*/

typedef struct {                                                                // mask type
    mask_t      val;
    mask_t      mask;
} Mask;

typedef struct {                                                                // instruction type
    char        opcode[OPCODE_SIZE + 1];
    Mask        instr;
} Instr;

typedef struct {                                                                // instruction set type
    Instr      *instrs;
    int         size;
} InstrSet;

typedef struct {                                                                // string set type
    char        opcode[OPCODE_NUM][OPCODE_SIZE + 1];
    int         size;
} StringSet;

#endif   // HELPERS_TYPES_H
