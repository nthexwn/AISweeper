#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include <time.h>

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

// This provides a complete top-down view of the game to the client.  It is useful for establishing the status of a
// new or resumed game on the client.  Most of this information would be redundant to return after every action, so
// the action_info structure is used for that instead.
typedef struct game_info
{
  Error_type error_type;
  unsigned char game_status;
  unsigned char height;
  unsigned char width;
  short mines_not_flagged;
  struct timespec time_started;
  unsigned char* copy_field_begin;
} Game_info;

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
  short mines_not_flagged;
  Copy_node* modified_positions_head;
} Action_info;

// Starts the game on the server.  Note that the copy_field_begin returned in the game_info object will be null after
// a call to start_game since the client can safely assume that the playing field contains x * y unrevealed positions.
// It would be a waste of bandwidth to send an x * y segment of zero'd position chars to indicate this.
Game_info start_game(unsigned char width, unsigned char height, unsigned short mines);

// This should be called by the client when resuming old games, or upon winning/losing the current game.  When resuming
// a game this will provide an overview of the current playing field from the client's perspective (IE: mine positions
// and adjacency info for unrevealed positions are still hidden).  When calling this function after winning or losing
// the game the full playing field will be revealed for analysis by players or AI routines.  Note that this information
// is not returned implicitly by action_info objects from other functions and must be explicitly requested by the
// client after the game status has changed to won/lost.  NOTE:  It is up to the caller of this function to delete the
// copied field position data after it is used.
Game_info sync_game();

// This is the functional equivalent of left-clicking on a tile at the coordinates (x,y).  The returned information
// will include either an error code (IE: invalid coordinates, position already revealed, etc.), or a linked list of
// modified playing field positions.  This will, logically, either contain a single detonated mine or a series of
// revealed positions.  Note that there is no explicit function for performing an "area-reveal" (simultaneous
// left-click+right-click equivalent).  The client may choose to implement this on its own by first determining if an
// area reveal is valid for a given position and then making a series of individual reveal calls to surrounding
// positions.  This allows the server to return individual action_info items for the revealed positions which will be
// easier for the client to parse.  NOTE:  It is up to the caller of this function to delete the linked list of copied
// field positions after it is used.
Action_info reveal(unsigned char x, unsigned char y);

// This is the functional equivalent of right-clicking on a tile at the coordinates (x,y).  The returned information
// will include either an error code (IE: invalid coordinates, position already flagged, etc.) or a list containing the
// flagged/unflagged position.  NOTE:  It is up to the caller of this function to delete the linked list of copied
// field positions after it is used.
Action_info toggle_flag(unsigned char x, unsigned char y);

// This call must be made explicitly after the game is won or lost.  Optionally, a call to sync_game can be made before
// this if the client wishes to reveal the entire playing field to analyze missed mines/flags.  Upon calling the quit
// action all of the position data on the playing field will be erased from memory, the game variables will be reset,
// and the game status will change to GAME_STATUS_NOT_IN_PROGRESS.
Action_info quit_game();

// Temporary display calls for testing purposes (display will ultimately be handled by client).
void display_as_client();
void display_as_server();

#endif // SERVER_GAME_H

