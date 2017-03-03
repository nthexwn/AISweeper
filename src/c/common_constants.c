#include "common_constants.h"

// Buffer lengths.
const unsigned short COMMAND_BUFFER_LENGTH = 8;
const unsigned short RESPONSE_BUFFER_LENGTH = 4096;

// Command lengths and offsets.
const unsigned char COMMAND_SHUT_DOWN_REQUIRED_LENGTH = 1;
const unsigned char COMMAND_START_GAME_REQUIRED_LENGTH = 5;
const unsigned char COMMAND_START_GAME_HEIGHT_OFFSET = 1;
const unsigned char COMMAND_START_GAME_WIDTH_OFFSET = 2;
const unsigned char COMMAND_START_GAME_MINES_OFFSET = 3;
const unsigned char COMMAND_SYNC_GAME_REQUIRED_LENGTH = 1;
const unsigned char COMMAND_REVEAL_POSITION_REQUIRED_LENGTH = 3;
const unsigned char COMMAND_REVEAL_POSITION_X_OFFSET = 1;
const unsigned char COMMAND_REVEAL_POSITION_Y_OFFSET = 2;
const unsigned char COMMAND_TOGGLE_FLAG_REQUIRED_LENGTH = 3;
const unsigned char COMMAND_TOGGLE_FLAG_X_OFFSET = 1;
const unsigned char COMMAND_TOGGLE_FLAG_Y_OFFSET = 2:
const unsigned char COMMAND_QUIT_GAME_REQUIRED_LENGTH = 1;

// Response lengths and offsets.
const unsigned char RESPONSE_SHUT_DOWN_REQUIRED_LENGTH = 1;
const unsigned char RESPONSE_START_GAME_REQUIRED_LENGTH = 1;
const unsigned char RESPONSE_SYNC_GAME_REQUIRED_LENGTH = 6;
const unsigned char RESPONSE_SYNC_GAME_GAME_STATUS_OFFSET = 1;
const unsigned char RESPONSE_SYNC_GAME_HEIGHT_OFFSET = 2;
const unsigned char RESPONSE_SYNC_GAME_WIDTH_OFFSET = 3;
const unsigned char RESPONSE_SYNC_GAME_MINES_OFFSET = 4;
const unsigned char RESPONSE_SYNC_GAME_FIELD_COPY_OFFSET = 6;
const unsigned char RESPONSE_REVEAL_POSITION_REQUIRED_LENGTH = 2;
const unsigned char RESPONSE_REVEAL_POSITION_GAME_STATUS_OFFSET = 1;
const unsigned char RESPONSE_REVEAL_POSITION_MBLA_HEAD_OFFSET = 2;
const unsigned char RESPONSE_TOGGLE_FLAG_REQUIRED_LENGTH = 1;
const unsigned char RESPONSE_QUIT_GAME_REQUIRED_LENGTH = 1;

// Copy node length and offsets.
const unsigned char COPY_NODE_REQUIRED_LENGTH = 3;
const unsigned char COPY_NODE_X_OFFSET = 0;
const unsigned char COPY_NODE_Y_OFFSET = 1;
const unsigned char COPY_NODE_POSITION_OFFSET = 2;

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

// Miscellaneous.
const unsigned char BITS_PER_CHAR = 8;
const unsigned char CHAR_SPACE = 32;
const unsigned char CONTROL_CHARACTER_RANGE = 31;
const unsigned char DATA_TO_CHARACTER_OFFSET = 48;
const unsigned char SUPPORTED_COMMAND_COUNT = 6;
const unsigned char RESPONSE_CODE_GROUP_WIDTH = 10;
const unsigned char RESPONSE_CODE_OUT_OF_MEMORY_INDICATOR = 9;

// Response code strings.
const char* response_code_strings[90] =
{
  "SHUT_DOWN_NO_ERROR", // 0
  "SHUT_DOWN_UNDEFINED_ERROR", // 1
  "SHUT_DOWN_UNDEFINED_ERROR", // 2
  "SHUT_DOWN_UNDEFINED_ERROR", // 3
  "SHUT_DOWN_UNDEFINED_ERROR", // 4
  "SHUT_DOWN_UNDEFINED_ERROR", // 5
  "SHUT_DOWN_UNDEFINED_ERROR", // 6
  "SHUT_DOWN_UNDEFINED_ERROR", // 7
  "SHUT_DOWN_UNDEFINED_ERROR", // 8
  "SHUT_DOWN_OUT_OF_MEMORY", // 9
  "START_GAME_NO_ERROR", // 10
  "START_GAME_ALREADY_IN_PROGRESS", // 11
  "START_GAME_HEIGHT_TOO_SMALL", // 12
  "START_GAME_HEIGHT_TOO_LARGE", // 13
  "START_GAME_WIDTH_TOO_SMALL", // 14
  "START_GAME_WIDTH_TOO_LARGE", // 15
  "START_GAME_NOT_ENOUGH_MINES", // 16
  "START_GAME_TOO_MANY_MINES", // 17
  "START_GAME_UNDEFINED_ERROR", // 18
  "START_GAME_OUT_OF_MEMORY", // 19
  "SYNC_GAME_NO_ERROR", // 20
  "SYNC_GAME_NOT_IN_PROGRESS", // 21
  "SYNC_GAME_UNDEFINED_ERROR", // 22
  "SYNC_GAME_UNDEFINED_ERROR", // 23
  "SYNC_GAME_UNDEFINED_ERROR", // 24
  "SYNC_GAME_UNDEFINED_ERROR", // 25
  "SYNC_GAME_UNDEFINED_ERROR", // 26
  "SYNC_GAME_UNDEFINED_ERROR", // 27
  "SYNC_GAME_UNDEFINED_ERROR", // 28
  "SYNC_GAME_OUT_OF_MEMORY", // 29
  "REVEAL_POSITION_NO_ERROR", // 30
  "REVEAL_POSITION_GAME_NOT_IN_PROGRESS", // 31
  "REVEAL_POSITION_GAME_ALREADY_FINISHED", // 32
  "REVEAL_POSITION_X_COORDINATE_TOO_LOW", // 33
  "REVEAL_POSITION_X_COORDINATE_TOO_HIGH", // 34
  "REVEAL_POSITION_Y_COORDINATE_TOO_LOW", // 35
  "REVEAL_POSITION_Y_COORDINATE_TOO_HIGH", // 36
  "REVEAL_POSITION_MUST_BE_UNREVEALED", // 37
  "REVEAL_POSITION_MUST_UNFLAG_FIRST", // 38
  "REVEAL_POSITION_OUT_OF_MEMORY", // 39
  "TOGGLE_FLAG_NO_ERROR", // 40
  "TOGGLE_FLAG_GAME_NOT_IN_PROGRESS", // 41
  "TOGGLE_FLAG_GAME_ALREADY_FINISHED", // 42
  "TOGGLE_FLAG_X_COORDINATE_TOO_LOW", // 43
  "TOGGLE_FLAG_X_COORDINATE_TOO_HIGH", // 44
  "TOGGLE_FLAG_Y_COORDINATE_TOO_LOW", // 45
  "TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH", // 46
  "TOGGLE_FLAG_MUST_BE_UNREVEALED", // 47
  "TOGGLE_FLAG_UNDEFINED_ERROR", // 48
  "TOGGLE_FLAG_OUT_OF_MEMORY", // 49
  "QUIT_GAME_NO_ERROR", // 50
  "QUIT_GAME_NOT_IN_PROGRESS", // 51
  "QUIT_GAME_UNDEFINED_ERROR", // 52
  "QUIT_GAME_UNDEFINED_ERROR", // 53
  "QUIT_GAME_UNDEFINED_ERROR", // 54
  "QUIT_GAME_UNDEFINED_ERROR", // 55
  "QUIT_GAME_UNDEFINED_ERROR", // 56
  "QUIT_GAME_UNDEFINED_ERROR", // 57
  "QUIT_GAME_UNDEFINED_ERROR", // 58
  "QUIT_GAME_OUT_OF_MEMORY", // 59
  "COMMAND_NO_ERROR", // 60
  "COMMAND_NO_INPUT", // 61
  "COMMAND_CODE_NOT_VALID", // 62
  "COMMAND_INSUFFICIENT_DATA", // 63
  "COMMAND_EXCESSIVE_DATA", // 64
  "COMMAND_UNDEFINED_ERROR", // 65
  "COMMAND_UNDEFINED_ERROR", // 66
  "COMMAND_UNDEFINED_ERROR", // 67
  "COMMAND_UNDEFINED_ERROR", // 68
  "COMMAND_OUT_OF_MEMORY", // 69
  "RESPONSE_NO_ERROR", // 70
  "RESPONSE_NO_INPUT", // 71
  "RESPONSE_CODE_NOT_VALID", // 72
  "RESPONSE_INSUFFICIENT_DATA", // 73
  "RESPONSE_EXCESSIVE_DATA", // 74
  "RESPONSE_UNDEFINED_ERROR", // 75
  "RESPONSE_UNDEFINED_ERROR", // 76
  "RESPONSE_UNDEFINED_ERROR", // 77
  "RESPONSE_UNDEFINED_ERROR", // 78
  "RESPONSE_OUT_OF_MEMORY", // 79
  "PROGRAM_NO_ERROR", // 80
  "PROGRAM_UNDEFINED_ERROR", // 81
  "PROGRAM_UNDEFINED_ERROR", // 82
  "PROGRAM_UNDEFINED_ERROR", // 83
  "PROGRAM_UNDEFINED_ERROR", // 84
  "PROGRAM_UNDEFINED_ERROR", // 85
  "PROGRAM_UNDEFINED_ERROR", // 86
  "PROGRAM_UNDEFINED_ERROR", // 87
  "PROGRAM_UNDEFINED_ERROR", // 88
  "PROGRAM_OUT_OF_MEMORY" // 89
};

// Game status strings.
const char* game_status_strings[5] =
{
  "NOT_IN_PROGRESS", // 0
  "IN_PROGRESS_NO_REVEAL", // 1
  "IN_PROGRESS", // 2
  "LOST", // 3
  "WON" // 4
};

