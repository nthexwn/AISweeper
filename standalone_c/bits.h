#ifndef BITS_H
#define BITS_h

#include <stdbool.h>

// Getters and setters for position data.
unsigned char get_adjacent(unsigned char *position);
void set_adjacent(unsigned char* position, unsigned char adjacent_mines);
bool is_mined(unsigned char* position);
void set_mined(unsigned char* position, bool value);
bool is_flagged(unsigned char* position);
void set_flagged(unsigned char* position, bool value);
bool is_revealed(unsigned char* position);
void set_revealed(unsigned char* position, bool value);

#endif // BITS_H

