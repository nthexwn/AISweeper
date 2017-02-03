#define _GNU_SOURCE
#ifndef SWEEPER_H
#define SWEEPER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "constants.h"

unsigned char get_adjacent(unsigned char *position);
void set_adjacent(unsigned char* position, unsigned char adjacent_mines);
bool is_checked(unsigned char* position);
void set_checked(unsigned char* position, bool value);
bool is_mined(unsigned char* position);
void set_mined(unsigned char* position, bool value);
bool is_flagged(unsigned char* position);
void set_flagged(unsigned char* position, bool value);
bool is_revealed(unsigned char* position);
void set_revealed(unsigned char* position, bool value);
unsigned char start_game(unsigned char width, unsigned char height, unsigned short mines);
unsigned char reveal(unsigned char x, unsigned char y);
unsigned char flag(unsigned char x, unsigned char y);
unsigned char unflag(unsigned char x, unsigned char y);
unsigned char quit();

#endif // SWEEPER_H

