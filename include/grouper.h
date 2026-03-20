#ifndef     HELPERS_GROUPER_H
#define     HELPERS_GROUPER_H

#include    "types.h"

Mask *lone_group(const InstrSet *set, const StringSet *subset);

MaskSet *auto_group(const InstrSet *full_set, StringSet *subset);

#endif   // HELPERS_GROUPER_H
