#define _GNU_SOURCE
#ifndef SWEEPER_H
#define SWEEPER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "mt19937.h"

unsigned char start_game(unsigned char width, unsigned char height, unsigned short mines);
unsigned char reveal(unsigned char x, unsigned char y);
unsigned char flag(unsigned char x, unsigned char y);
unsigned char unflag(unsigned char x, unsigned char y);
unsigned char quit();

// Temporary display calls for testing purposes (display will ultimately be handled by client)
void display_as_client();
void display_as_server();

#endif // SWEEPER_H

