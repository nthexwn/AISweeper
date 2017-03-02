#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>

// Commands.
typedef enum command_type
{
  COMMAND_SHUT_DOWN = 0,
  COMMAND_START_GAME = 1,
  COMMAND_SYNC_GAME = 2,
  COMMAND_REVEAL = 3,
  COMMAND_TOGGLE_FLAG = 4,
  COMMAND_QUIT_GAME = 5
} Command_type;

// Status.
extern const char* status_messages[5];
typedef enum status_type
{
  GAME_STATUS_NOT_IN_PROGRESS = 0,
  GAME_STATUS_IN_PROGRESS_NO_REVEAL = 1,
  GAME_STATUS_IN_PROGRESS = 2,
  GAME_STATUS_LOST = 3,
  GAME_STATUS_WON = 4
} Status_type;

// Errors.
extern const char* error_messages[90];
typedef enum error_type
{
  GENERAL_NO_ERROR = 0,
  GENERAL_UNEXPECTED_ERROR = 9,
  COMMAND_NO_ERROR = 10,
  COMMAND_NO_INPUT = 11,
  COMMAND_CODE_NOT_VALID = 12,
  COMMAND_INSUFFICIENT_ARGUMENT_DATA = 13,
  COMMAND_EXCESSIVE_ARGUMENT_DATA = 14,
  COMMAND_OUT_OF_MEMORY = 19,
  RESPONSE_NO_ERROR = 20,
  RESPONSE_NO_INPUT = 21,
  RESPONSE_CODE_NOT_VALID = 22,
  RESPONSE_INSUFFICIENT_ARGUMENT_DATA = 23,
  RESPONSE_EXCESSIVE_ARGUMENT_DATA = 24,
  RESPONSE_OUT_OF_MEMORY = 29,
  SHUT_DOWN_NO_ERROR = 30,
  SHUT_DOWN_OUT_OF_MEMORY = 39,
  START_GAME_NO_ERROR = 40,
  START_GAME_ALREADY_IN_PROGRESS = 41,
  START_GAME_HEIGHT_TOO_SMALL = 42,
  START_GAME_HEIGHT_TOO_LARGE = 43,
  START_GAME_WIDTH_TOO_SMALL = 44,
  START_GAME_WIDTH_TOO_LARGE = 45,
  START_GAME_NOT_ENOUGH_MINES = 46,
  START_GAME_TOO_MANY_MINES = 47,
  START_GAME_OUT_OF_MEMORY = 49,
  SYNC_GAME_NO_ERROR = 50,
  SYNC_GAME_NOT_IN_PROGRESS = 51,
  SYNC_GAME_OUT_OF_MEMORY = 59,
  REVEAL_NO_ERROR = 60,
  REVEAL_GAME_NOT_IN_PROGRESS = 61,
  REVEAL_GAME_ALREADY_FINISHED = 62,
  REVEAL_X_COORDINATE_TOO_LOW = 63,
  REVEAL_X_COORDINATE_TOO_HIGH = 64,
  REVEAL_Y_COORDINATE_TOO_LOW = 65,
  REVEAL_Y_COORDINATE_TOO_HIGH = 66,
  REVEAL_MUST_BE_UNREVEALED = 67,
  REVEAL_MUST_UNFLAG_FIRST = 68,
  REVEAL_OUT_OF_MEMORY = 69,
  TOGGLE_FLAG_NO_ERROR = 70,
  TOGGLE_FLAG_GAME_NOT_IN_PROGRESS = 71,
  TOGGLE_FLAG_GAME_ALREADY_FINISHED = 72,
  TOGGLE_FLAG_X_COORDINATE_TOO_LOW = 73,
  TOGGLE_FLAG_X_COORDINATE_TOO_HIGH = 74,
  TOGGLE_FLAG_Y_COORDINATE_TOO_LOW = 75,
  TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH = 76,
  TOGGLE_FLAG_MUST_BE_UNREVEALED = 77,
  TOGGLE_FLAG_OUT_OF_MEMORY = 79,
  QUIT_GAME_NO_ERROR = 80,
  QUIT_GAME_NOT_IN_PROGRESS = 81,
  QUIT_GAME_OUT_OF_MEMORY = 89,
} Error_type;

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

// Container for unsigned strings which contain raw data.  Relies on a length variable instead of null termination
// since numerical values are stored directly in these strings instead of being translated through characters.  This
// means that the null character (AKA: "\0", #0, end of string) is frequently used to represent the actual number 0 and
// can't be utilized in this structure.  Without any kind of terminator it's up to the function which is handling one
// of these data strings to never read beyond the provided length of the string.  Likewise, any function which writes
// to one of these strings must accurately update the length variable after doing so.
typedef struct data_string
{
  unsigned short length;
  unsigned char* data;
} Data_string;

// Linked list structure for playing field positions.  Copy_nodes list the filtered contents of playing field
// positions.  Information regarding unrevealed adjacency and mine presence will be removed from the position data when
// creating these nodes if the game has not yet been finished.  A separate ref_node structure is used internally for
// reading and writing unfiltered data directly to/from the playing field.
typedef struct copy_node
{
  unsigned char x;
  unsigned char y;
  unsigned char position;
  struct copy_node* next;
} Copy_node;

// Data returned to client after each individual game action.  This contains the first error that was found while
// processing the action (if any), the current game status, the number of unflagged mines remaining, and a linked
// list of all the game positions which were modified by the action.  NOTE: although the values for game status and
// mines flagged could technically be calculated independently by the client it makes more sense from an overall
// processing standpoint to just have the server function authoritatively regarding these values.  The server already
// has data structures in place which make it easy to determine what these values are, whereas the client would have
// to re-model the entire game independently to come up with them on its own.
typedef struct action_info
{
  Error_type error_type;
  unsigned char game_status;
  signed short mines_not_flagged;
  Copy_node* mbla_head;
} Action_info;

// This provides a complete top-down view of the game to the client.  It is useful for establishing the status of a
// new or resumed game on the client.  Most of this information would be redundant to return after every action, so
// the action_info structure is used for that instead.
typedef struct game_info
{
  Error_type error_type;
  unsigned char game_status;
  signed short mines_not_flagged;
  unsigned char height;
  unsigned char width;
  unsigned char* copy_field_begin;
} Game_info;

// Action info data offsets.
extern const unsigned char ACTION_INFO_ERROR_TYPE_OFFSET;
extern const unsigned char ACTION_INFO_GAME_STATUS_OFFSET;
extern const unsigned char ACTION_INFO_MINES_NOT_FLAGGED_OFFSET;
extern const unsigned char ACTION_INFO_MBLA_COUNT_OFFSET;
extern const unsigned char ACTION_INFO_MBLA_HEAD_OFFSET;

// Game info data offsets.
extern const unsigned char GAME_INFO_ERROR_TYPE_OFFSET;
extern const unsigned char GAME_INFO_GAME_STATUS_OFFSET;
extern const unsigned char GAME_INFO_MINES_NOT_FLAGGED_OFFSET;
extern const unsigned char GAME_INFO_HEIGHT_OFFSET;
extern const unsigned char GAME_INFO_WIDTH_OFFSET;
extern const unsigned char GAME_INFO_COPY_FIELD_BEGIN_OFFSET;

// Copy node offsets.
extern const unsigned char COPY_NODE_POSITION_OFFSET;
extern const unsigned char COPY_NODE_X_OFFSET;
extern const unsigned char COPY_NODE_Y_OFFSET;

// Argument lengths and offsets.
extern const unsigned char SHUT_DOWN_EXPECTED_LENGTH;
extern const unsigned char START_GAME_EXPECTED_LENGTH;
extern const unsigned char START_GAME_HEIGHT_OFFSET;
extern const unsigned char START_GAME_WIDTH_OFFSET;
extern const unsigned char START_GAME_MINES_OFFSET;
extern const unsigned char SYNC_GAME_EXPECTED_LENGTH;
extern const unsigned char REVEAL_EXPECTED_LENGTH;
extern const unsigned char REVEAL_X_OFFSET;
extern const unsigned char REVEAL_Y_OFFSET;
extern const unsigned char TOGGLE_FLAG_EXPECTED_LENGTH;
extern const unsigned char TOGGLE_FLAG_X_OFFSET;
extern const unsigned char TOGGLE_FLAG_Y_OFFSET;
extern const unsigned char QUIT_GAME_EXPECTED_LENGTH;

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

// Miscellaneous.
extern const unsigned char BITS_PER_CHAR;
extern const unsigned char CHAR_SPACE;
extern const unsigned char CONTROL_CHARACTER_RANGE;
extern const unsigned char DATA_TO_CHARACTER_OFFSET;
extern const unsigned char ERROR_GROUP_WIDTH;
extern const unsigned char ERROR_COMMAND_GROUP_OFFSET;
extern const unsigned short MAXIMUM_POSSIBLE_COMMAND_LENGTH;
extern const unsigned short MAXIMUM_POSSIBLE_RESPONSE_LENGTH;
extern const unsigned char NUMBER_OF_SUPPORTED_COMMANDS;
extern const unsigned char POSITION_DATA_SIZE;

#endif // CONSTANTS_H

