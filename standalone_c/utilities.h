#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include "constants.h"

// Print bits starting at the base of the provided memory location.  This is useful for debugging.
void print_bits(unsigned char* start, unsigned char count);

// Utility function for determining endianness of the machine at runtime.  The premise here is that the 9th bit in a 16
// 16 bit unsigned short short will appear as the 1st bit in the 2nd byte of data.  On little endian machines the bytes
// are stored from least significant to most significant.  This means that the first sequential byte in the number 256
// will appear as 0 (0b00000000) and the second byte will appear as 1 (0b00000001).  On big endian machines the bytes
// are stored from most significant to least significant.  This means that the first sequential byte in the number 256
// will appear as 1 (0b00000001) and the second byte will appear as 0 (0b00000000).  We prefer to utilize the number
// 256 here since it requires at least 9 bits of storage and ensures that there will be two bytes which differ in order
// between little endian and big endian architectures.  If we simply used the number 1, the compiler could potentially
// compress the 16-bit data into a single 8-bit character which would appear the same in memory on either architecture.
Endian_type machine_endian();

// Utility method for extracting values from data in memory.  Caller must provide a reference to the memory location of
// the data, the length (in bytes) of the value to extract, and a flag indicating whether or not the data is encoded
// using big or little endian format.  If the data was serialized it will most likely be in network byte order (AKA:
// big endian) though the code which originally performed the serialization should be manually checked before making
// this assumption.  If the data was directly placed in memory by the machine then it will depend on the machine's
// endianness (typically little endian).  In these cases a call can be made to machine_endian to determine the
// machine's endianness prior to extracting the data.  NOTE: It is up to the caller to cast the returned pointer from
// this function back to a data type which matches the length value which they originally passed in and to dispose of
// the data in the buffer when they are through with it.
unsigned char* extract_value(unsigned char* data, unsigned short length, Endian_type endian_type);

#endif // UTILITIES_H

