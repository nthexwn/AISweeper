#include <stdint.h>
#include "mt19937.h"

/*
I copied this Mersenne Twister implementation from https://en.wikipedia.org/wiki/Mersenne_Twister on 2/4/2017.
I'm not gonna lie...  I have no idea how this works, but it seems to generate pretty decent random numbers!
*/

enum
{
    N = 624,
    M = 397,
    R = 31,
    A = 0x9908B0DF,
    F = 1812433253,
    U = 11,
    S = 7,
    B = 0x9D2C5680,
    T = 15,
    C = 0xEFC60000,
    L = 18,
    MASK_LOWER = (1ull << R) - 1,
    MASK_UPPER = (1ull << R)
};

static uint32_t mt[N];
static uint16_t index;

void seed_mt19937(const uint32_t seed)
{
    uint32_t i;
    mt[0] = seed;
    for(i = 1; i < N; i++)
    {
        mt[i] = (F * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i);
    }
    index = N;
}

static void twist()
{
   uint32_t i, x, xA;
   for(i = 0; i < N; i++)
   {
      x = (mt[i] & MASK_UPPER) + (mt[(i + 1) % N] & MASK_LOWER);
      xA = x >> 1;
      if(x & 0x1) xA ^= A;
      mt[i] = mt[(i + M) % N] ^ xA;
   }
   index = 0;
}

uint32_t extract_mt19937()
{
   uint32_t y;
   int i = index;
   if(index >= N)
   {
      twist();
      i = index;
   }
   y = mt[i];
   index = i + 1;
   y ^= (mt[i] >> U);
   y ^= (y << S) & B;
   y ^= (y << T) & C;
   y ^= (y >> L);
   return y;
}

