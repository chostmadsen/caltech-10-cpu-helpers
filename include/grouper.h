#ifndef     HELPERS_GROUPER_H
#define     HELPERS_GROUPER_H

#include    "types.h"

Mask *lone_group(const InstrSet *set, const StringSet *subset);

MaskSet *full_group(const InstrSet *set, StringSet *subset);

#endif   // HELPERS_GROUPER_H
