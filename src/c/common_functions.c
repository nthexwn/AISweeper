#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"

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

void print_bits(unsigned char* start, unsigned char count)
{
  for(unsigned char* current_byte = start; current_byte < start + count * sizeof(unsigned char); current_byte +=
      sizeof(unsigned char))
  {
    for(unsigned char current_bit_index = 0; current_bit_index < BITS_PER_CHAR; current_bit_index++)
    {
      unsigned char bit_mask = 1 << (BITS_PER_CHAR - current_bit_index - 1);
      if(*current_byte & bit_mask) printf("1");
      else printf("0");
    }
    printf(" ");
  }
}

Endian_type machine_endian()
{
  unsigned short n = 256;
  if(*(unsigned char *)&n == 1) return ENDIAN_BIG;
  return ENDIAN_LITTLE;
}

void transfer_value(unsigned char* source, Endian_type source_type, unsigned char* destination,
                    Endian_type destination_type, unsigned short length)
{
  // Alas, a previous implementation here which utilized bit-shifting did not work since the operators in C are
  // implemented to shift numeric values in the registers and not the actual bits in memory.  This leads to
  // counter-intuitive situations along byte boundaries such as when shifting little endian 0b0000000110000000 to the
  // right by one.  One might expect this to simply become 0b0000000011000000, but in the register the original value
  // is re-ordered from memory as 0b1000000000000001 (absolute highest order to lowest order bits without any
  // distinction between bytes), becomes 0b0100000000000000 after the right shift, and is written back to memory as
  // 0b0000000001000000 with the formerly low order bit having been unintentionally eradicated.  This is frustrating!
  // Instead, the current implementation relies on copying the source byte values from memory directly into the
  // destination buffer (also in memory) one at a time.
  if(source_type == destination_type)
  {
    for(unsigned char index = 0; index < length; index++) *(destination + index * sizeof(unsigned char)) = *(source +
        index * sizeof(unsigned char));
  }
  else
  {
    for(unsigned char index = 0; index < length; index++) *(destination + index * sizeof(unsigned char)) = *(source +
        (length - index - 1) * sizeof(unsigned char));
  }
}

