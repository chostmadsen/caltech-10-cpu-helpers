#ifndef     HELPERS_GROUPER_H
#define     HELPERS_GROUPER_H

#include    "types.h"

Mask *lone_group(const InstrSet *set, const StringSet *subset);

MaskSet *join_groups(const Mask *masks, const int num);

#endif   // HELPERS_GROUPER_H
