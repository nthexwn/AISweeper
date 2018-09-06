#ifndef COMMON_ENUMS_H
#define COMMON_ENUMS_H

// Endian modes.
typedef enum byte_order
{
  // Technically there are other types of endian byte orders which are utilized on ancient and exotic systems such as
  // the Univac, LINC, or PDP-6.  These systems often utilize signed magnitude or 1's complement instead of 2's
  // complement for storing bits and follow an inverted byte order for negative values.  It is extremely unlikely that
  // this program will ever be run on such a machine.  In the unforseen event that it is we will add other types to
  // this enum and add supporting utility functions then.
  ENDIAN_LITTLE,
  ENDIAN_BIG
} Byte_order;

// Commands types.
typedef enum command_type
{
  COMMAND_SHUT_DOWN = 0,
  COMMAND_START_GAME = 1,
  COMMAND_SYNC_GAME = 2,
  COMMAND_REVEAL_POSITION = 3,
  COMMAND_TOGGLE_FLAG = 4,
  COMMAND_QUIT_GAME = 5
} Command_type;

// Response codes.
typedef enum response_code
{
  SHUT_DOWN_NO_ERROR = 0,
  START_GAME_NO_ERROR = 10,
  START_GAME_ALREADY_IN_PROGRESS = 11,
  START_GAME_HEIGHT_TOO_SMALL = 12,
  START_GAME_HEIGHT_TOO_LARGE = 13,
  START_GAME_WIDTH_TOO_SMALL = 14,
  START_GAME_WIDTH_TOO_LARGE = 15,
  START_GAME_NOT_ENOUGH_MINES = 16,
  START_GAME_TOO_MANY_MINES = 17,
  SYNC_GAME_NO_ERROR = 20,
  SYNC_GAME_NOT_IN_PROGRESS = 21,
  REVEAL_POSITION_NO_ERROR = 30,
  REVEAL_POSITION_GAME_NOT_IN_PROGRESS = 31,
  REVEAL_POSITION_GAME_ALREADY_FINISHED = 32,
  REVEAL_POSITION_X_COORDINATE_TOO_HIGH = 33,
  REVEAL_POSITION_Y_COORDINATE_TOO_HIGH = 34,
  REVEAL_POSITION_MUST_BE_UNREVEALED = 35,
  REVEAL_POSITION_MUST_UNFLAG_FIRST = 36,
  TOGGLE_FLAG_NO_ERROR = 40,
  TOGGLE_FLAG_GAME_NOT_IN_PROGRESS = 41,
  TOGGLE_FLAG_GAME_ALREADY_FINISHED = 42,
  TOGGLE_FLAG_X_COORDINATE_TOO_HIGH = 43,
  TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH = 44,
  TOGGLE_FLAG_MUST_BE_UNREVEALED = 45,
  QUIT_GAME_NO_ERROR = 50,
  QUIT_GAME_NOT_IN_PROGRESS = 51,
  COMMAND_NO_INPUT = 61,
  COMMAND_CODE_NOT_VALID = 62,
  COMMAND_INSUFFICIENT_DATA = 63,
  COMMAND_EXCESSIVE_DATA = 64,
  RESPONSE_NO_ERROR = 70,
  RESPONSE_NO_INPUT = 71,
  RESPONSE_CODE_NOT_VALID = 72,
  RESPONSE_INSUFFICIENT_DATA = 73,
  RESPONSE_EXCESSIVE_DATA = 74,
  RESPONSE_OUT_OF_MEMORY = 79,
  PROGRAM_NO_ERROR = 80,
  PROGRAM_OUT_OF_MEMORY = 89
} Response_code;

// Game statuses.
typedef enum game_status
{
  GAME_STATUS_NOT_IN_PROGRESS = 0,
  GAME_STATUS_IN_PROGRESS_NO_REVEAL = 1,
  GAME_STATUS_IN_PROGRESS = 2,
  GAME_STATUS_LOST = 3,
  GAME_STATUS_WON = 4
} Game_status;

#endif // COMMON_ENUMS_H

