#include <stdbool.h>
#include "bits.h"
#include "constants.h"

unsigned char get_adjacent(unsigned char *position)
{
  return *position & BITS_ADJACENT;
}

void set_adjacent(unsigned char* position, unsigned char adjacent_mines)
{
  *position |= adjacent_mines;
}

bool is_mined(unsigned char* position)
{
  return (*position & BIT_MINED) == BIT_MINED;
}

void set_mined(unsigned char* position, bool value)
{
  if(value) *position |= BIT_MINED;
  else *position &= ~(BIT_MINED);
}

bool is_flagged(unsigned char* position)
{
  return (*position & BIT_FLAGGED) == BIT_FLAGGED;
}

void set_flagged(unsigned char* position, bool value)
{
  if(value) *position |= BIT_FLAGGED;
  else *position &= ~(BIT_FLAGGED);
}

bool is_revealed(unsigned char* position)
{
  return (*position & BIT_REVEALED) == BIT_REVEALED;
}

void set_revealed(unsigned char* position, bool value)
{
  if(value) *position |= BIT_REVEALED;
  else *position &= ~(BIT_REVEALED);
}

