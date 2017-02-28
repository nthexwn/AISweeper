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
const char* error_messages[90] =
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
  "GENERAL_OUT_OF_MEMORY", // 9
  "COMMAND_NO_ERROR", // 10
  "COMMAND_NO_INPUT", // 11
  "COMMAND_CODE_NOT_VALID", // 12
  "COMMAND_INSUFFICIENT_ARGUMENT_DATA", // 13
  "COMMAND_EXCESSIVE_ARGUMENT_DATA", // 14
  "COMMAND_UNDEFINED_ERROR", // 15
  "COMMAND_UNDEFINED_ERROR", // 16
  "COMMAND_UNDEFINED_ERROR", // 17
  "COMMAND_UNDEFINED_ERROR", // 18
  "COMMAND_OUT_OF_MEMORY", // 19
  "RESPONSE_NO_ERROR", // 20
  "RESPONSE_NO_INPUT", // 21
  "RESPONSE_CODE_NOT_VALID", // 22
  "RESPONSE_INSUFFICIENT_ARGUMENT_DATA", // 23
  "RESPONSE_EXCESSIVE_ARGUMENT_DATA", // 24
  "RESPONSE_UNDEFINED_ERROR", // 25
  "RESPONSE_UNDEFINED_ERROR", // 26
  "RESPONSE_UNDEFINED_ERROR", // 27
  "RESPONSE_UNDEFINED_ERROR", // 28
  "RESPONSE_OUT_OF_MEMORY", // 29
  "SHUT_DOWN_NO_ERROR", // 30
  "SHUT_DOWN_UNDEFINED_ERROR", // 31
  "SHUT_DOWN_UNDEFINED_ERROR", // 32
  "SHUT_DOWN_UNDEFINED_ERROR", // 33
  "SHUT_DOWN_UNDEFINED_ERROR", // 34
  "SHUT_DOWN_UNDEFINED_ERROR", // 35
  "SHUT_DOWN_UNDEFINED_ERROR", // 36
  "SHUT_DOWN_UNDEFINED_ERROR", // 37
  "SHUT_DOWN_UNDEFINED_ERROR", // 38
  "SHUT_DOWN_OUT_OF_MEMORY", // 39
  "START_GAME_NO_ERROR", // 40
  "START_GAME_ALREADY_IN_PROGRESS", // 41
  "START_GAME_HEIGHT_TOO_SMALL", // 42
  "START_GAME_HEIGHT_TOO_LARGE", // 43
  "START_GAME_WIDTH_TOO_SMALL", // 44
  "START_GAME_WIDTH_TOO_LARGE", // 45
  "START_GAME_NOT_ENOUGH_MINES", // 46
  "START_GAME_TOO_MANY_MINES", // 47
  "START_GAME_UNDEFINED_ERROR", // 48
  "START_GAME_OUT_OF_MEMORY", // 49
  "SYNC_GAME_NO_ERROR", // 50
  "SYNC_GAME_NOT_IN_PROGRESS", // 51
  "SYNC_GAME_UNDEFINED_ERROR", // 52
  "SYNC_GAME_UNDEFINED_ERROR", // 53
  "SYNC_GAME_UNDEFINED_ERROR", // 54
  "SYNC_GAME_UNDEFINED_ERROR", // 55
  "SYNC_GAME_UNDEFINED_ERROR", // 56
  "SYNC_GAME_UNDEFINED_ERROR", // 57
  "SYNC_GAME_UNDEFINED_ERROR", // 58
  "SYNC_GAME_OUT_OF_MEMORY", // 59
  "REVEAL_NO_ERROR", // 60
  "REVEAL_GAME_NOT_IN_PROGRESS", // 61
  "REVEAL_GAME_ALREADY_FINISHED", // 62
  "REVEAL_X_COORDINATE_TOO_LOW", // 63
  "REVEAL_X_COORDINATE_TOO_HIGH", // 64
  "REVEAL_Y_COORDINATE_TOO_LOW", // 65
  "REVEAL_Y_COORDINATE_TOO_HIGH", // 66
  "REVEAL_MUST_BE_UNREVEALED", // 67
  "REVEAL_MUST_UNFLAG_FIRST", // 68
  "REVEAL_OUT_OF_MEMORY", // 69
  "TOGGLE_FLAG_NO_ERROR", // 70
  "TOGGLE_FLAG_GAME_NOT_IN_PROGRESS", // 71
  "TOGGLE_FLAG_GAME_ALREADY_FINISHED", // 72
  "TOGGLE_FLAG_X_COORDINATE_TOO_LOW", // 73
  "TOGGLE_FLAG_X_COORDINATE_TOO_HIGH", // 74
  "TOGGLE_FLAG_Y_COORDINATE_TOO_LOW", // 75
  "TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH", // 76
  "TOGGLE_FLAG_MUST_BE_UNREVEALED", // 77
  "TOGGLE_FLAG_UNDEFINED_ERROR", // 78
  "TOGGLE_FLAG_OUT_OF_MEMORY", // 79
  "QUIT_GAME_NO_ERROR", // 80
  "QUIT_GAME_NOT_IN_PROGRESS", // 81
  "QUIT_GAME_UNDEFINED_ERROR", // 82
  "QUIT_GAME_UNDEFINED_ERROR", // 83
  "QUIT_GAME_UNDEFINED_ERROR", // 84
  "QUIT_GAME_UNDEFINED_ERROR", // 85
  "QUIT_GAME_UNDEFINED_ERROR", // 86
  "QUIT_GAME_UNDEFINED_ERROR", // 87
  "QUIT_GAME_UNDEFINED_ERROR", // 88
  "QUIT_GAME_OUT_OF_MEMORY" // 89
};

// Action info data offsets.
const unsigned char ACTION_INFO_ERROR_TYPE_OFFSET = 0;
const unsigned char ACTION_INFO_GAME_STATUS_OFFSET = 1;
const unsigned char ACTION_INFO_MINES_NOT_FLAGGED_OFFSET = 2;
const unsigned char ACTION_INFO_MBLA_COUNT_OFFSET = 4;
const unsigned char ACTION_INFO_MBLA_HEAD_OFFSET = 6;

// Game info data offsets.
const unsigned char GAME_INFO_ERROR_TYPE_OFFSET = 0;
const unsigned char GAME_INFO_GAME_STATUS_OFFSET = 1;
const unsigned char GAME_INFO_MINES_NOT_FLAGGED_OFFSET = 2;
const unsigned char GAME_INFO_HEIGHT_OFFSET = 4;
const unsigned char GAME_INFO_WIDTH_OFFSET = 5;
const unsigned char GAME_INFO_COPY_FIELD_BEGIN_OFFSET = 6;

// Copy node offsets.
const unsigned char COPY_NODE_X_OFFSET = 0;
const unsigned char COPY_NODE_Y_OFFSET = 1;
const unsigned char COPY_NODE_POSITION_OFFSET = 2;

// Argument lengths and offsets.
const unsigned char SHUT_DOWN_EXPECTED_LENGTH = 1;
const unsigned char START_GAME_EXPECTED_LENGTH = 5;
const unsigned char START_GAME_HEIGHT_OFFSET = 1;
const unsigned char START_GAME_WIDTH_OFFSET = 2;
const unsigned char START_GAME_MINES_OFFSET = 3;
const unsigned char SYNC_GAME_EXPECTED_LENGTH = 1;
const unsigned char REVEAL_EXPECTED_LENGTH = 3;
const unsigned char REVEAL_X_OFFSET = 1;
const unsigned char REVEAL_Y_OFFSET = 2;
const unsigned char TOGGLE_FLAG_EXPECTED_LENGTH = 3;
const unsigned char TOGGLE_FLAG_X_OFFSET = 1;
const unsigned char TOGGLE_FLAG_Y_OFFSET = 2;
const unsigned char QUIT_GAME_EXPECTED_LENGTH = 1;

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
const unsigned char ERROR_GROUP_WIDTH = 10;
const unsigned char ERROR_COMMAND_GROUP_OFFSET = 3;
const unsigned short MAXIMUM_POSSIBLE_COMMAND_LENGTH = 8;
const unsigned short MAXIMUM_POSSIBLE_RESPONSE_LENGTH = 4096;
const unsigned char NUMBER_OF_SUPPORTED_COMMANDS = 6;
const unsigned char POSITION_DATA_SIZE = 3;

