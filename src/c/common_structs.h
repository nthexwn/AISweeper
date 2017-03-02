#ifndef COMMON_STRUCTS_H
#define COMMON_STRUCTS_H

#include "common_enums.h"

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

#endif // COMMON_STRUCTS_H
