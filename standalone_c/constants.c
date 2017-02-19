#include <stdbool.h>
#include "constants.h"

// Characters.
const unsigned char BITS_PER_CHAR = 8;
const unsigned char DATA_TO_CHARACTER_OFFSET = 48;

// Transmission info.
const unsigned short MAXIMUM_POSSIBLE_COMMAND_LENGTH = 5;
const unsigned short MAXIMUM_POSSIBLE_RESPONSE_LENGTH = 2136;

// Bit flags for position scheme.
const unsigned char BITS_ADJACENT = 0xfu;   // 0b00001111
const unsigned char BIT_MINED = 0x10u;      // 0b00010000
const unsigned char BITS_SENSITIVE = 0x1fu; // 0b00011111
const unsigned char BIT_FLAGGED = 0x20u;    // 0b00100000
const unsigned char BIT_REVEALED = 0x40u;   // 0b01000000
const unsigned char BIT_UNUSED = 0x80u;     // 0b10000000

// Parameter constraints for new game.
const unsigned char NEW_GAME_MIN_HEIGHT = 9;
const unsigned char NEW_GAME_MIN_WIDTH = 9;
const unsigned char NEW_GAME_MAX_HEIGHT = 24;
const unsigned char NEW_GAME_MAX_WIDTH = 30;
const unsigned char NEW_GAME_MIN_MINES = 10;

