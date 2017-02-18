#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "utilities.h"

void print_bits(unsigned char* start, unsigned char count)
{
  for(unsigned char* current_byte = start; current_byte < start + count / 8; current_byte++)
  {
    for(unsigned char current_bit_index = 0; current_bit_index < 8; current_bit_index++)
    {
      unsigned char bit_mask = 1 << (7 - current_bit_index);
      if(*current_byte & bit_mask) printf("1");
      else printf("0");
    }
    printf(" ");
  }
  printf("\n");
}

Endian_type machine_endian()
{
  unsigned short n = 256;
  if(*(unsigned char *)&n == 1) return ENDIAN_BIG;
  return ENDIAN_LITTLE;
}

unsigned char* extract_value(unsigned char* data, unsigned short length, Endian_type endian_type)
{
  // Alas, a previous implementation here which utilized bit-shifting did not work since the operators in C are
  // implemented to shift numeric values in the registers and not the actual bits in memory.  This leads to
  // counter-intuitive situations along byte boundaries such as when shifting little endian 0b0000000110000000 to the
  // right by one.  One might expect this to simply become 0b0000000011000000, but in the register the original value
  // is re-ordered from memory as 0b1000000000000001 (absolute highest order to lowest order bits without any
  // distinction between bytes), becomes 0b0100000000000000 after the right shift, and is written back to memory as
  // 0b0000000001000000 with the formerly low order bit having been unintentionally eradicated.  This is frustrating!
  // Instead, the current implementation relies on copying the original byte values from memory directly into the
  // return buffer (also in memory) one at a time.
  unsigned char* buffer = (unsigned char*)malloc(length * sizeof(unsigned char));
  if(machine_endian() == endian_type)
  {
    for(unsigned char index = 0; index < length; index++) *(buffer + index) = *(data + index);
  }
  else
  {
    for(unsigned char index = 0; index < length; index++) *(buffer + index) = *(data + length - index - 1);
  }
  return buffer;
}

