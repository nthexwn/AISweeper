#ifndef COMMON_FUNCTIONS_H
#define COMMON_FUNCTIONS_H

#include <stdbool.h>
#include "common_enums.h"

// Getters and setters for position data.
unsigned char get_adjacent(unsigned char *position);
void set_adjacent(unsigned char* position, unsigned char adjacent_mines);
bool is_mined(unsigned char* position);
void set_mined(unsigned char* position, bool value);
bool is_flagged(unsigned char* position);
void set_flagged(unsigned char* position, bool value);
bool is_revealed(unsigned char* position);
void set_revealed(unsigned char* position, bool value);

// Print bits starting at the base of the provided memory location.  This is useful for debugging.
void print_bits(unsigned char* start, unsigned char count);

// Utility function for determining endianness of the machine at runtime.  The premise here is that the 9th bit in a 16
// bit unsigned short short will appear as the 1st bit in the 2nd byte of data.  On little endian machines the bytes
// are stored from least significant to most significant.  This means that the first sequential byte in the number 256
// will appear as 0 (0b00000000) and the second byte will appear as 1 (0b00000001).  On big endian machines the bytes
// are stored from most significant to least significant.  This means that the first sequential byte in the number 256
// will appear as 1 (0b00000001) and the second byte will appear as 0 (0b00000000).  We prefer to utilize the number
// 256 here since it requires at least 9 bits of storage and ensures that there will be two bytes which differ in order
// between little endian and big endian architectures.  If we simply used the number 1, the compiler could potentially
// compress the 16-bit data into a single 8-bit character which would appear the same in memory on either architecture.
Byte_order detect_machine_byte_order();

// Utility method for transfering numeric values between locations in memory and (optionally) converting between endian
// types.  Caller must provide buffer pointers and endian types for both source data and destination data as well as
// the length (in bytes) of the value to transfer.
void transfer_value(unsigned char* source, Byte_order source_byte_order, unsigned char* destination,
                    Byte_order destination_byte_order, unsigned short length);

#endif // COMMON_FUNCTIONS_H

