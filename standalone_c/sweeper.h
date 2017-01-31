#ifndef SWEEPER_H_
#define SWEEPER_H_

#include <linux/random.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

unsigned char getAdjacent(unsigned char *position);
void setAdjacent(unsigned char* position, unsigned char adjacent_mines);
bool isChecked(unsigned char* position);
void setChecked(unsigned char* position, bool value);
bool isMined(unsigned char* position);
void setMined(unsigned char* position, bool value);
bool isFlagged(unsigned char* position);
void setFlagged(unsigned char* position, bool value);
bool isRevealed(unsigned char* position);
void setRevealed(unsigned char* position, bool value);
int start_game();

#endif // SWEEPER_H_

