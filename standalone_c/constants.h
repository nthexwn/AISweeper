#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <stdbool.h>

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
extern const char* error_messages[80];
typedef enum error_type
{
  GENERAL_NO_ERROR = 0,
  GENERAL_UNEXPECTED_ERROR = 9,
  COMMAND_NO_ERROR = 10,
  COMMAND_NO_INPUT = 11,
  COMMAND_DOES_NOT_EXIST = 12,
  COMMAND_INSUFFICIENT_ARGUMENT_DATA = 13,
  COMMAND_EXCESSIVE_ARGUMENT_DATA = 14,
  COMMAND_UNEXPECTED_ERROR = 19,
  START_GAME_NO_ERROR = 20,
  START_GAME_ALREADY_IN_PROGRESS = 21,
  START_GAME_HEIGHT_TOO_SMALL = 22,
  START_GAME_HEIGHT_TOO_LARGE = 23,
  START_GAME_WIDTH_TOO_SMALL = 24,
  START_GAME_WIDTH_TOO_LARGE = 25,
  START_GAME_NOT_ENOUGH_MINES = 26,
  START_GAME_TOO_MANY_MINES = 27,
  START_GAME_UNEXPECTED_ERROR = 29,
  SYNC_GAME_NO_ERROR = 30,
  SYNC_GAME_NOT_IN_PROGRESS = 31,
  SYNC_GAME_UNEXPECTED_ERROR = 39,
  REVEAL_NO_ERROR = 40,
  REVEAL_GAME_NOT_IN_PROGRESS = 41,
  REVEAL_GAME_ALREADY_FINISHED = 42,
  REVEAL_X_COORDINATE_TOO_LOW = 43,
  REVEAL_X_COORDINATE_TOO_HIGH = 44,
  REVEAL_Y_COORDINATE_TOO_LOW = 45,
  REVEAL_Y_COORDINATE_TOO_HIGH = 46,
  REVEAL_MUST_BE_UNREVEALED = 47,
  REVEAL_MUST_UNFLAG_FIRST = 48,
  REVEAL_UNEXPECTED_ERROR = 49,
  TOGGLE_FLAG_NO_ERROR = 50,
  TOGGLE_FLAG_GAME_NOT_IN_PROGRESS = 51,
  TOGGLE_FLAG_GAME_ALREADY_FINISHED = 52,
  TOGGLE_FLAG_X_COORDINATE_TOO_LOW = 53,
  TOGGLE_FLAG_X_COORDINATE_TOO_HIGH = 54,
  TOGGLE_FLAG_Y_COORDINATE_TOO_LOW = 55,
  TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH = 56,
  TOGGLE_FLAG_MUST_BE_UNREVEALED = 57,
  TOGGLE_FLAG_UNEXPECTED_ERROR = 59,
  QUIT_GAME_NO_ERROR = 60,
  QUIT_GAME_NOT_IN_PROGRESS = 61,
  QUIT_GAME_UNEXPECTED_ERROR = 69,
  SHUT_DOWN_NO_ERROR = 70,
  SHUT_DOWN_UNEXPECTED_ERROR = 79
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
  unsigned char position;
  unsigned char x;
  unsigned char y;
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
  unsigned char height;
  unsigned char width;
  signed short mines_not_flagged;
  unsigned long seconds_elapsed;
  unsigned char* copy_field_begin;
} Game_info;

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

