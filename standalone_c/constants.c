#include <stdbool.h>
#include "constants.h"

// Status.
const char* status_messages[5] =
{
  "NOT_IN_PROGRESS", // 0
  "IN_PROGRESS_NO_REVEAL", // 1
  "IN_PROGRESS", // 2
  "LOST", // 3
  "WON", // 4
};

// Errors.
const char* error_messages[80] =
{
  "GENERAL_NO_ERROR", // 0
  "GENERAL_UNDEFINED_ERROR", // 1
  "GENERAL_UNDEFINED_ERROR", // 2
  "GENERAL_UNDEFINED_ERROR", // 3
  "GENERAL_UNDEFINED_ERROR", // 4
  "GENERAL_UNDEFINED_ERROR", // 5
  "GENERAL_UNDEFINED_ERROR", // 6
  "GENERAL_UNDEFINED_ERROR", // 7
  "GENERAL_UNDEFINED_ERROR", // 8
  "GENERAL_UNEXPECTED_ERROR", // 9
  "COMMAND_NO_ERROR", // 10
  "COMMAND_NO_INPUT", // 11
  "COMMAND_DOES_NOT_EXIST", // 12
  "COMMAND_INSUFFICIENT_ARGUMENT_DATA", // 13
  "COMMAND_EXCESSIVE_ARGUMENT_DATA", // 14
  "COMMAND_UNDEFINED_ERROR", // 15
  "COMMAND_UNDEFINED_ERROR", // 16
  "COMMAND_UNDEFINED_ERROR", // 17
  "COMMAND_UNDEFINED_ERROR", // 18
  "COMMAND_UNEXPECTED_ERROR", // 19
  "START_GAME_NO_ERROR", // 20
  "START_GAME_ALREADY_IN_PROGRESS", // 21
  "START_GAME_HEIGHT_TOO_SMALL", // 22
  "START_GAME_HEIGHT_TOO_LARGE", // 23
  "START_GAME_WIDTH_TOO_SMALL", // 24
  "START_GAME_WIDTH_TOO_LARGE", // 25
  "START_GAME_NOT_ENOUGH_MINES", // 26
  "START_GAME_TOO_MANY_MINES", // 27
  "START_GAME_UNDEFINED_ERROR", // 28
  "START_GAME_UNEXPECTED_ERROR", // 29
  "SYNC_GAME_NO_ERROR", // 30
  "SYNC_GAME_NOT_IN_PROGRESS", // 31
  "SYNC_GAME_UNDEFINED_ERROR", // 32
  "SYNC_GAME_UNDEFINED_ERROR", // 33
  "SYNC_GAME_UNDEFINED_ERROR", // 34
  "SYNC_GAME_UNDEFINED_ERROR", // 35
  "SYNC_GAME_UNDEFINED_ERROR", // 36
  "SYNC_GAME_UNDEFINED_ERROR", // 37
  "SYNC_GAME_UNDEFINED_ERROR", // 38
  "SYNC_GAME_UNEXPECTED_ERROR", // 39
  "REVEAL_NO_ERROR", // 40
  "REVEAL_GAME_NOT_IN_PROGRESS", // 41
  "REVEAL_GAME_ALREADY_FINISHED", // 42
  "REVEAL_X_COORDINATE_TOO_LOW", // 43
  "REVEAL_X_COORDINATE_TOO_HIGH", // 44
  "REVEAL_Y_COORDINATE_TOO_LOW", // 45
  "REVEAL_Y_COORDINATE_TOO_HIGH", // 46
  "REVEAL_MUST_BE_UNREVEALED", // 47
  "REVEAL_MUST_UNFLAG_FIRST", // 48
  "REVEAL_UNEXPECTED_ERROR", // 49
  "TOGGLE_FLAG_NO_ERROR", // 50
  "TOGGLE_FLAG_GAME_NOT_IN_PROGRESS", // 51
  "TOGGLE_FLAG_GAME_ALREADY_FINISHED", // 52
  "TOGGLE_FLAG_X_COORDINATE_TOO_LOW", // 53
  "TOGGLE_FLAG_X_COORDINATE_TOO_HIGH", // 54
  "TOGGLE_FLAG_Y_COORDINATE_TOO_LOW", // 55
  "TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH", // 56
  "TOGGLE_FLAG_MUST_BE_UNREVEALED", // 57
  "TOGGLE_FLAG_UNDEFINED_ERROR", // 58
  "TOGGLE_FLAG_UNEXPECTED_ERROR", // 59
  "QUIT_GAME_NO_ERROR", // 60
  "QUIT_GAME_NOT_IN_PROGRESS", // 61
  "QUIT_GAME_UNDEFINED_ERROR", // 62
  "QUIT_GAME_UNDEFINED_ERROR", // 63
  "QUIT_GAME_UNDEFINED_ERROR", // 64
  "QUIT_GAME_UNDEFINED_ERROR", // 65
  "QUIT_GAME_UNDEFINED_ERROR", // 66
  "QUIT_GAME_UNDEFINED_ERROR", // 67
  "QUIT_GAME_UNDEFINED_ERROR", // 68
  "QUIT_GAME_UNEXPECTED_ERROR", // 69
  "SHUT_DOWN_NO_ERROR", // 70
  "SHUT_DOWN_UNDEFINED_ERROR", // 71
  "SHUT_DOWN_UNDEFINED_ERROR", // 72
  "SHUT_DOWN_UNDEFINED_ERROR", // 73
  "SHUT_DOWN_UNDEFINED_ERROR", // 74
  "SHUT_DOWN_UNDEFINED_ERROR", // 75
  "SHUT_DOWN_UNDEFINED_ERROR", // 76
  "SHUT_DOWN_UNDEFINED_ERROR", // 77
  "SHUT_DOWN_UNDEFINED_ERROR", // 78
  "SHUT_DOWN_UNEXPECTED_ERROR" // 79
};

// Characters.
const unsigned char BITS_PER_CHAR = 8;
const unsigned char CHAR_SPACE = 32;
const unsigned char CONTROL_CHARACTER_RANGE = 31;
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

