#ifndef SERVER_MT19937_H
#define SERVER_MT19937_H

#include <stdint.h>

// Seed the random number generator.
void seed_mt19937(const uint32_t seed);

// Extract the next number from the random number generator (must be seeded first!).
uint32_t extract_mt19937();

#endif // SERVER_MT19937_H

