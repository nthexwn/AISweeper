#ifndef MT19937_H
#define MT19937_H

#include <stdint.h>

void seed_mt19937(const uint32_t seed);
uint32_t extract_mt19937();

#endif // MT19937_H

