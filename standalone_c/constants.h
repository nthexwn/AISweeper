#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>

// Errors.
typedef enum error_type
{
  GENERAL_NO_ERROR = 0,
  GENERAL_UNEXPECTED_ERROR = 9,
  COMMAND_NO_INPUT = 11,
  COMMAND_DOES_NOT_EXIST = 12,
  COMMAND_INSUFFICIENT_ARGUMENT_DATA = 13,
  COMMAND_EXCESSIVE_ARGUMENT_DATA = 14,
  COMMAND_UNEXPECTED_ERROR = 19,
  START_GAME_ALREADY_IN_PROGRESS = 21,
  START_GAME_HEIGHT_TOO_SMALL = 22,
  START_GAME_HEIGHT_TOO_LARGE = 23,
  START_GAME_WIDTH_TOO_SMALL = 24,
  START_GAME_WIDTH_TOO_LARGE = 25,
  START_GAME_NOT_ENOUGH_MINES = 26,
  START_GAME_TOO_MANY_MINES = 27,
  START_GAME_UNEXPECTED_ERROR = 29,
  SYNC_GAME_NOT_IN_PROGRESS = 31,
  SYNC_GAME_UNEXPECTED_ERROR = 39,
  REVEAL_GAME_NOT_IN_PROGRESS = 41,
  REVEAL_GAME_ALREADY_FINISHED = 42,
  REVEAL_X_COORDINATE_TOO_LOW = 43,
  REVEAL_X_COORDINATE_TOO_HIGH = 44,
  REVEAL_Y_COORDINATE_TOO_LOW = 45,
  REVEAL_Y_COORDINATE_TOO_HIGH = 46,
  REVEAL_MUST_BE_UNREVEALED = 47,
  REVEAL_MUST_UNFLAG_FIRST = 48,
  REVEAL_UNEXPECTED_ERROR = 49,
  TOGGLE_FLAG_GAME_NOT_IN_PROGRESS = 51,
  TOGGLE_FLAG_GAME_ALREADY_FINISHED = 52,
  TOGGLE_FLAG_X_COORDINATE_TOO_LOW = 53,
  TOGGLE_FLAG_X_COORDINATE_TOO_HIGH = 54,
  TOGGLE_FLAG_Y_COORDINATE_TOO_LOW = 55,
  TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH = 56,
  TOGGLE_FLAG_MUST_BE_UNREVEALED = 57,
  TOGGLE_FLAG_UNEXPECTED_ERROR = 59,
  QUIT_GAME_NOT_IN_PROGRESS = 61,
  QUIT_GAME_UNEXPECTED_ERROR = 69
} Error_type;

// Status.
typedef enum status_type
{
  GAME_STATUS_NOT_IN_PROGRESS,
  GAME_STATUS_IN_PROGRESS_NO_REVEAL,
  GAME_STATUS_IN_PROGRESS,
  GAME_STATUS_LOST,
  GAME_STATUS_WON
} Status_type;

// Endianness.
typedef enum endian_type
{
  // Technically there are other types of endian byte orders which are utilized on ancient and exotic systems such as
  // the Univac, LINC, or PDP-6.  These systems often utilize signed magnitude or 1's complement instead of 2's
  // complement for storing bits and follow an inverted byte order for negative values.  It is extremely unlikely that
  // this program will ever be run on such a machine.  In the unforseen event that it is we will add other types to
  // this enum and add supporting utility functions then.
  ENDIAN_LITTLE,
  ENDIAN_BIG
} Endian_type;

// Characters.
extern const unsigned char BITS_PER_CHAR;
extern const unsigned char CHAR_SPACE;
extern const unsigned char CONTROL_CHARACTER_RANGE;
extern const unsigned char DATA_TO_CHARACTER_OFFSET;

// Transmission info.
extern const unsigned short MAXIMUM_POSSIBLE_COMMAND_LENGTH;
extern const unsigned short MAXIMUM_POSSIBLE_RESPONSE_LENGTH;

// Bit flags for position scheme.
extern const unsigned char BITS_ADJACENT;  // 0b00001111
extern const unsigned char BIT_MINED;      // 0b00010000
extern const unsigned char BITS_SENSITIVE; // 0b00011111
extern const unsigned char BIT_FLAGGED;    // 0b00100000
extern const unsigned char BIT_REVEALED;   // 0b01000000
extern const unsigned char BIT_UNUSED;     // 0b10000000

// Parameter constraints for new game.
extern const unsigned char NEW_GAME_MIN_HEIGHT;
extern const unsigned char NEW_GAME_MIN_WIDTH;
extern const unsigned char NEW_GAME_MAX_HEIGHT;
extern const unsigned char NEW_GAME_MAX_WIDTH;
extern const unsigned char NEW_GAME_MIN_MINES;

#endif // CONSTANTS_H

