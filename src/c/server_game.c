#define _GNU_SOURCE

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"
#include "common_structs.h"
#include "server_game.h"
#include "server_mt19937.h"

// Linked list structure for playing field positions.  Ref_nodes refer to the actual contents of the playing field on
// the server.  They should only be used internally by the core game functions so that the client cannot cheat by
// inspecting the true contents of the playing field (IE: whether or not a position contains a mine or an unrevealed
// position has adjacent mines).
typedef struct ref_node
{
  unsigned char x;
  unsigned char y;
  unsigned char* position;
  struct ref_node* next;
} Ref_node;

// Helper function for clearing linked lists.
static void clear_ref_list(Ref_node* ref_head)
{
  while(ref_head != NULL)
  {
    Ref_node* ref_free = ref_head;
    ref_head = ref_head->next;
    free(ref_free);
  }
}

// Game variables which are shared between functions.
static signed short mines_not_flagged; // Number of mines which haven't been flagged.  Can be negative.
static unsigned short unmined_positions_remaining; // Number of positions which the player must reveal to win.
static unsigned short current_mines; // Number of mines on the playing field after setup.
static unsigned char current_height; // Current height of the playing field.
static unsigned char current_width; // Current width of the playing field.
static unsigned char* field_begin; // Pointer to first position char in field.
static Ref_node* nm_head; // Not-mined list used to track available positions for random mine placement and victory.
static Ref_node* nm_tail; // Not-mined list used to track available positions for random mine placement and victory.
static Game_status game_status; // Status code indicating current game state.

// Helper function to determine if the command+argument data in the command string is of the length required by the
// command handler.  If it's not then the command handler will return an error instead of calling its corresponding
// command function.  Note that no validation is done on the actual argument contents themselves.  They are all
// interpreted as raw numeric values without any delimiters.  Those values are then passed to the individual commands.
// It's up to the commands themselves to assess the values passed in and ensure that they fall within acceptable bounds
// or make sense in the specific context of the command.
static bool ensure_valid_length(unsigned short valid_length, Data_string* command_string,
                                Data_string* response_string)
{
  if(command_string->length < valid_length)
  {
    *(response_string->data + response_string->length) = COMMAND_INSUFFICIENT_DATA;
    response_string->length += sizeof(unsigned char);
    return false;
  }
  if(command_string->length > valid_length)
  {
    *(response_string->data + response_string->length) = COMMAND_EXCESSIVE_DATA;
    response_string->length += sizeof(unsigned char);
    return false;
  }
  return true;
}

// Calculate adjacency counts for all of the positions on the playing field which are adjacent to mines and place the
// detected sum into the leading data bits for each position.
static void calculate_adjacency_data()
{
  for(unsigned char y = 0; y < current_height; y++)
  {
    for(unsigned char x = 0; x < current_width; x++)
    {
      unsigned char* position = field_begin + (current_width * y + x) * sizeof(unsigned char);
      if(!is_mined(position))
      {
        unsigned char adjacent_mines = 0;
        if(x > 0 && y > 0)
        {
          unsigned char* above_left = field_begin + (current_width * (y - 1) + x - 1) * sizeof(unsigned char);
          if(is_mined(above_left)) adjacent_mines++;
        }
        if(y > 0)
        {
          unsigned char* above = field_begin + (current_width * (y - 1) + x) * sizeof(unsigned char);
          if(is_mined(above)) adjacent_mines++;
        }
        if(x < current_width - 1 && y > 0)
        {
          unsigned char* above_right = field_begin + (current_width * (y - 1) + x + 1) * sizeof(unsigned char);
          if(is_mined(above_right)) adjacent_mines++;
        }
        if(x > 0)
        {
          unsigned char* left = field_begin + (current_width * y + x - 1) * sizeof(unsigned char);
          if(is_mined(left)) adjacent_mines++;
        }
        if(x < current_width - 1)
        {
          unsigned char* right = field_begin + (current_width * y + x + 1) * sizeof(unsigned char);
          if(is_mined(right)) adjacent_mines++;
        }
        if(x > 0 && y < current_height - 1)
        {
          unsigned char* below_left = field_begin + (current_width * (y + 1) + x - 1) * sizeof(unsigned char);
          if(is_mined(below_left)) adjacent_mines++;
        }
        if(y < current_height - 1)
        {
          unsigned char* below = field_begin + (current_width * (y + 1) + x) * sizeof(unsigned char);
          if(is_mined(below)) adjacent_mines++;
        }
        if(x < current_width - 1 && y < current_height - 1)
        {
          unsigned char* below_right = field_begin + (current_width * (y + 1) + x + 1) * sizeof(unsigned char);
          if(is_mined(below_right)) adjacent_mines++;
        }
        set_adjacent(position, adjacent_mines);
      }
    }
  }
}

// Secondary setup function which is triggered when the player performs their first reveal action.
static void first_reveal_setup(unsigned char x, unsigned char y)
{
  // Remove the position to reveal from the non-mined list so that we don't consider it for mine placement during
  // generation.  This prevents the player from unfairly blowing up on their first reveal.
  if(x == 0 && y == 0)
  {
    Ref_node* nm_free = nm_head;
    nm_head = nm_head->next;
    free(nm_free);
  }
  else
  {
    Ref_node* nm_index = nm_head;
    unsigned short positions_left_to_traverse = current_width * y + x;
    while(positions_left_to_traverse > 1)
    {
      nm_index = nm_index->next;
      positions_left_to_traverse--;
    }
    Ref_node* nm_free = nm_index->next;
    nm_index->next = nm_index->next->next;
    free(nm_free);
  }

  // Initialize random number generator.  This will make a system call to dev/urandom to retrieve entropy bits for a
  // random seed which are generated by the system from process IDs, temperature sensors, network packet timings, and
  // other quasi-random entities.  This is a GNU/Linux specific feature which serves as a more secure alternative to
  // the common practice of using the current time as a random seed.  This seed will be used in conjunction with a
  // Mersenne Twister pseudo-random number generator to generate random positions for mine placements on the playing
  // field.  This PRNG is purportedly faster and less predictable than the default implementation of the standard
  // 'rand' function.
  long urand;
  syscall(SYS_getrandom, &urand, sizeof(long), 0);
  seed_mt19937(urand);
  mines_not_flagged = current_mines;
  unsigned short mines_placed = 0;
  while(mines_placed < current_mines)
  {
    // The most straight-forward approach here would be to generate random numbers for the x and y coordinates of the
    // next mine placement.  If those coordinates happened to be mined already we would simply generate a new set of
    // random coordinates.  Unfortunately, as more mines are placed it would become more and more probable that we'd
    // run into these sorts of collision scenarios.  Random number generation, while generally operating in O(1) time,
    // is computationally expensive and there is no guaranteed upper limit on the number of collisions (and therefore
    // recalculations) which we'd have to perform.  Instead, I've decided to utilize a safer and more performant
    // approach which uses a linked list of remaining unmined locations.  A random position in this list will be chosen
    // for each new mine placement.  We'll traverse the list to that position, place the mine, and then remove the
    // position from the list.  While this is technically an O(N) operation each traverse will take less and less time
    // since the unmined list will continue to shorten as more mines are placed.  This is also guaranteed to work
    // without collisions, and the bounds on N are fairly trivial since the playing field has relatively small maximum
    // dimensions.
    unsigned short random_number = (unsigned short)extract_mt19937();
    unsigned short positions_left_to_traverse = random_number % (unmined_positions_remaining - 1);
    if(positions_left_to_traverse == 0)
    {
      set_mined(nm_head->position, true);
      Ref_node* nm_free = nm_head;
      nm_head = nm_head->next;
      free(nm_free);
    }
    else
    {
      Ref_node* nm_index = nm_head;
      while(positions_left_to_traverse > 1)
      {
        nm_index = nm_index->next;
        positions_left_to_traverse--;
      }
      set_mined(nm_index->next->position, true);
      Ref_node* nm_free = nm_index->next;
      nm_index->next = nm_index->next->next;
      free(nm_free);
    }
    mines_placed++;
    unmined_positions_remaining--;
  }

  // Now that the mines have all been placed we can calculate the adjacency data for unmined positions.
  calculate_adjacency_data();

  // First reveal has been performed.  Subsequent reveals in the current game will no longer trigger this function.
  game_status = GAME_STATUS_IN_PROGRESS;
}

// Helper method for reveal actions.  Adds coordinates and position data to the modified by last action list in the
// response string of the reveal action.  Takes a pointer to the response string and a pointer to a pointer of the
// current tail of the modified by last action list.  The original pointer to the tail of the modified by last action
// list is maintained in the reveal function so that it can be updated by this method and then re-used when the method
// is called again.
static void add_position_to_mbla_list(Data_string* response_string, unsigned char** response_mbla_index,
                                      unsigned char x, unsigned char y, unsigned char position)
{
  // Add x
  **response_mbla_index = x;
  *response_mbla_index += sizeof(unsigned char);
  response_string->length += sizeof(unsigned char);

  // Add y
  **response_mbla_index = y;
  *response_mbla_index += sizeof(unsigned char);
  response_string->length += sizeof(unsigned char);

  // Add position
  **response_mbla_index = position;
  *response_mbla_index += sizeof(unsigned char);
  response_string->length += sizeof(unsigned char);
}

// Helper method for reveal actions.  Used to reveal unmined nodes in a specificed direction adjacent to the current
// position at the head of the reveal queue.  The x_mod and y_mod characters determine the direction.  If the
// directional position is also not adjacent to any other mines then it will be added to the tail of the reveal queue.
// Subsequent calls to this function will be made for every direction surrounding every position in the reveal queue.
// Utilizing this queue structure is slightly less memory and performance intensive than making these calls recursively
// would be.  Each time this method is called the revealed position will be added to the modified by last action list
// (mbla) which is ultimately returned to the client.
static void handle_reveal_queue_direction(Ref_node** rq_head, Ref_node** rq_tail, Data_string* response_string,
                                          unsigned char** response_mbla_index, signed char x_mod, signed char y_mod)
{
  unsigned char* position = field_begin + current_width * ((*rq_head)->y + y_mod) + (*rq_head)->x + x_mod;
  if(!is_mined(position) && !is_revealed(position))
  {
    // Note that since the positions modified here are already revealed that we don't need to perform any filtering of
    // mine or adjacency info when adding these positions to the modified by last action list.  There's no remaining
    // information about the positions that would enable the client to cheat.
    set_revealed(position, true);
    add_position_to_mbla_list(response_string, response_mbla_index, (*rq_head)->x + x_mod, (*rq_head)->y + y_mod,
                              *position);
    unmined_positions_remaining--;
    if(get_adjacent(position) == 0)
    {
      (*rq_tail)->next = (Ref_node*)malloc(sizeof(Ref_node));
      *(*rq_tail)->next = (Ref_node){(*rq_head)->x + x_mod, (*rq_head)->y + y_mod, position, NULL};
      *rq_tail = (*rq_tail)->next;
    }
  }
}

// Called when either quitting the game or shutting down the server.
static void reset_shared_variables_and_lists()
{
  clear_ref_list(nm_head);
  nm_head = NULL;
  nm_tail = NULL;
  free(field_begin);
  field_begin = NULL;
  mines_not_flagged = 0;
  unmined_positions_remaining = 0;
  current_mines = 0;
  current_height = 0;
  current_width = 0;
  game_status = GAME_STATUS_NOT_IN_PROGRESS;
}

void server_start_game(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_START_GAME_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }
  if(game_status != GAME_STATUS_NOT_IN_PROGRESS)
  {
    *response_string->data = START_GAME_ALREADY_IN_PROGRESS;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char height = *(command_string->data + COMMAND_START_GAME_HEIGHT_OFFSET * sizeof(unsigned char));
  if(height < NEW_GAME_MIN_HEIGHT)
  {
    *response_string->data = START_GAME_HEIGHT_TOO_SMALL;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(height > NEW_GAME_MAX_HEIGHT)
  {
    *response_string->data = START_GAME_HEIGHT_TOO_LARGE;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char width = *(command_string->data + COMMAND_START_GAME_WIDTH_OFFSET * sizeof(unsigned char));
  if(width < NEW_GAME_MIN_WIDTH)
  {
    *response_string->data = START_GAME_WIDTH_TOO_SMALL;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(width > NEW_GAME_MAX_WIDTH)
  {
    *response_string->data = START_GAME_WIDTH_TOO_LARGE;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned short mines = 0;
  transfer_value(command_string->data + COMMAND_START_GAME_MINES_OFFSET * sizeof(unsigned char), ENDIAN_BIG,
                 (unsigned char*)&mines, detect_machine_byte_order(), sizeof(unsigned short));
  if(mines < NEW_GAME_MIN_MINES)
  {
    *response_string->data = START_GAME_NOT_ENOUGH_MINES;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(mines > (height - 1) * (width - 1))
  {
    *response_string->data = START_GAME_TOO_MANY_MINES;
    response_string->length += sizeof(unsigned char);
    return;
  }

  // Initialize shared variables.
  mines_not_flagged = mines;
  unmined_positions_remaining = height * width;
  current_mines = mines;
  current_height = height;
  current_width = width;

  // Allocate memory for playing field.  Note that each position on the field is represented by a single character
  // where the individual bits in that character are utilized for an adjacent mine count, mined flag, revealed flag,
  // and flagged flag.  Coordinate and dimension values are not stored in the field memory itself, so separate
  // variables must be provided for these in order to calculate the specific memory location needed to access a
  // particular position character within the playing field.  A pointer to a position character is not sufficient to
  // determine where that position is on the playing field without also knowing the memory address of the start of the
  // playing field and the width of the playing field.
  field_begin = (unsigned char*)calloc(height * width, sizeof(unsigned char));

  // Initialize the non-mined linked list.  At this point every position on the playing field belongs in this list
  // since no mines have been placed yet.
  nm_head = (Ref_node*)malloc(sizeof(Ref_node));
  *nm_head = (Ref_node){0};
  nm_tail = nm_head;
  for(unsigned char y = 0; y < height; y++)
  {
    for(unsigned char x = 0; x < width; x++)
    {
      unsigned char* position = field_begin + (width * y + x) * sizeof(unsigned char);
      nm_tail->next = (Ref_node*)malloc(sizeof(Ref_node));
      *nm_tail->next = (Ref_node){x, y, position, NULL};
      nm_tail = nm_tail->next;
    }
  }
  Ref_node* nm_free = nm_head;
  nm_head = nm_head->next;
  free(nm_free);

  // Game is ready to play!
  game_status = GAME_STATUS_IN_PROGRESS_NO_REVEAL;

  // Add response code to response string.
  *response_string->data = START_GAME_NO_ERROR;
  response_string->length += sizeof(unsigned char);
}

void server_sync_game(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_SYNC_GAME_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    *response_string->data = SYNC_GAME_NOT_IN_PROGRESS;
    response_string->length += sizeof(unsigned char);
    return;
  }

  // Add response code to response string.
  *response_string->data = SYNC_GAME_NO_ERROR;
  response_string->length += sizeof(unsigned char);

  // Add game status to response string.
  *(response_string->data + RESPONSE_SYNC_GAME_GAME_STATUS_OFFSET) = game_status;
  response_string->length += sizeof(unsigned char);

  // Add height to response string.
  *(response_string->data + RESPONSE_SYNC_GAME_HEIGHT_OFFSET) = current_height;
  response_string->length += sizeof(unsigned char);

  // Add width to response string.
  *(response_string->data + RESPONSE_SYNC_GAME_WIDTH_OFFSET) = current_width;
  response_string->length += sizeof(unsigned char);

  // Add mine count to response string.
  transfer_value((unsigned char*)&current_mines, detect_machine_byte_order(), response_string->data +
                 RESPONSE_SYNC_GAME_MINES_OFFSET, ENDIAN_BIG, sizeof(unsigned short));
  response_string->length += sizeof(unsigned short) / sizeof(unsigned char);

  // Add a copy of each position on the game field to the response string if any positions have already been revealed
  // (otherwise there's nothing to sync).
  unsigned char* response_string_index = response_string->data + RESPONSE_SYNC_GAME_FIELD_COPY_OFFSET;
  if(game_status > GAME_STATUS_IN_PROGRESS_NO_REVEAL)
  {
    for(unsigned char y = 0; y < current_height; y++)
    {
      for(unsigned char x = 0; x < current_width; x++)
      {
        unsigned char position = *(field_begin + (current_width * y + x) * sizeof(unsigned char));
        if(game_status == GAME_STATUS_IN_PROGRESS && !is_revealed(&position))
        {
          // If the game isn't finished yet and the position is still unrevealed then we'll filter out the mine and
          // adjacency information from the copied position data so that the client can't cheat by inspecting these
          // details.
          position &= ~BITS_SENSITIVE;
        }
        *response_string_index = position;
        response_string->length += sizeof(unsigned char);
        response_string_index += sizeof(unsigned char);
      }
    }
  }
}

void server_reveal_position(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_REVEAL_POSITION_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }
  unsigned char x = *(command_string->data + COMMAND_REVEAL_POSITION_X_OFFSET * sizeof(unsigned char));
  unsigned char y = *(command_string->data + COMMAND_REVEAL_POSITION_Y_OFFSET * sizeof(unsigned char));
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    *response_string->data = REVEAL_POSITION_GAME_NOT_IN_PROGRESS;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(game_status > GAME_STATUS_IN_PROGRESS)
  {
    *response_string->data = REVEAL_POSITION_GAME_ALREADY_FINISHED;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(x >= current_width)
  {
    *response_string->data = REVEAL_POSITION_X_COORDINATE_TOO_HIGH;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(y >= current_height)
  {
    *response_string->data = REVEAL_POSITION_Y_COORDINATE_TOO_HIGH;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char* position = field_begin + (current_width * y + x) * sizeof(unsigned char);
  if(is_revealed(position))
  {
    *response_string->data = REVEAL_POSITION_MUST_BE_UNREVEALED;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(is_flagged(position))
  {
    *response_string->data = REVEAL_POSITION_MUST_UNFLAG_FIRST;
    response_string->length += sizeof(unsigned char);
    return;
  }

  // Setup playing field if this is the first time the player has performed a reveal.
  if(game_status == GAME_STATUS_IN_PROGRESS_NO_REVEAL)
  {
    first_reveal_setup(x, y);
  }

  // Perform reveal.
  set_revealed(position, true);

  // Add the revealed position to the response's modified by last action list.
  unsigned char* response_mbla_index = response_string->data +
                                       RESPONSE_REVEAL_POSITION_MBLA_HEAD_OFFSET * sizeof(unsigned char);
  add_position_to_mbla_list(response_string, &response_mbla_index, x, y, *position);

  // Handle lose condition.
  if(is_mined(position))
  {
    // Boom!
    game_status = GAME_STATUS_LOST;

    *response_string->data = REVEAL_POSITION_NO_ERROR;
    response_string->length += sizeof(unsigned char);

    // Add game status to response.
    *(response_string->data + RESPONSE_REVEAL_POSITION_GAME_STATUS_OFFSET * sizeof(unsigned char)) = game_status;
    response_string->length += sizeof(unsigned char);
    return;
  }

  // At this point, we know we didn't lose, so we'll count down the positions remaining.
  unmined_positions_remaining--;

  // Populate the reveal queue starting with the first revealed position.
  Ref_node* rq_head = (Ref_node*)malloc(sizeof(Ref_node));
  *rq_head = (Ref_node){x, y, position, NULL};
  Ref_node* rq_tail = rq_head;

  // Begin revealing additional positions and adding surrounding unmined+unrevealed+unadjacent positions to the queue.
  // Revealing is performed first as a method of marking which positions have already been added to the queue (in order
  // to prevent duplicate entries).
  while(rq_head != NULL)
  {
    if(rq_head->x > 0 && rq_head->y > 0)
    {
      // Above left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, -1, -1);
    }
    if(rq_head->y > 0)
    {
      // Above.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, 0, -1);
    }
    if(rq_head->x < current_width - 1 && rq_head->y > 0)
    {
      // Above right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, 1, -1);
    }
    if(rq_head->x > 0)
    {
      // Left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, -1, 0);
    }
    if(rq_head->x < current_width - 1)
    {
      // Right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, 1, 0);
    }
    if(rq_head->x > 0 && rq_head->y < current_height - 1)
    {
      // Below left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, -1, 1);
    }
    if(rq_head->y < current_height - 1)
    {
      // Below.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, 0, 1);
    }
    if(rq_head->x < current_width - 1 && rq_head->y < current_height - 1)
    {
      // Below right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, response_string, &response_mbla_index, 1, 1);
    }

    // Chop off the reveal queue head and advance to the next position.
    Ref_node* rq_free = rq_head;
    rq_head = rq_head->next;
    free(rq_free);
    rq_free = NULL;
  }
  rq_tail = NULL;

  // Handle win condition.
  if(unmined_positions_remaining == 0) game_status = GAME_STATUS_WON;

  // Add return code to response.
  unsigned char* response_string_index = response_string->data;
  *response_string_index = REVEAL_POSITION_NO_ERROR;
  response_string_index += sizeof(unsigned char);
  response_string->length += sizeof(unsigned char);

  // Add game status to response.
  *response_string_index = game_status;
  response_string->length += sizeof(unsigned char);
  response_string->length += sizeof(unsigned char);
}

void server_toggle_flag(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_TOGGLE_FLAG_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    *response_string->data = TOGGLE_FLAG_GAME_NOT_IN_PROGRESS;
    response_string->length += sizeof(unsigned char);
    return;
  }
  if(game_status > GAME_STATUS_IN_PROGRESS)
  {
    *response_string->data = TOGGLE_FLAG_GAME_ALREADY_FINISHED;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char x = *(command_string->data + COMMAND_TOGGLE_FLAG_X_OFFSET);
  if(x >= current_width)
  {
    *response_string->data = TOGGLE_FLAG_X_COORDINATE_TOO_HIGH;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char y = *(command_string->data + COMMAND_TOGGLE_FLAG_Y_OFFSET);
  if(y >= current_height)
  {
    *response_string->data = TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH;
    response_string->length += sizeof(unsigned char);
    return;
  }
  unsigned char* position = field_begin + current_width * y + x;
  if(is_revealed(position))
  {
    *response_string->data = TOGGLE_FLAG_MUST_BE_UNREVEALED;
    response_string->length += sizeof(unsigned char);
    return;
  }

  // Toggle the flag.
  if(is_flagged(position))
  {
    set_flagged(position, false);
    mines_not_flagged++;
  }
  else
  {
    set_flagged(position, true);
    mines_not_flagged--;
  }

  // Add return code to response.
  *response_string->data = TOGGLE_FLAG_NO_ERROR;
  response_string->length += sizeof(unsigned char);

  // Add flagged position data to response.
  unsigned char* response_string_index = response_string->data + RESPONSE_TOGGLE_FLAG_MBLA_HEAD_OFFSET;
  add_position_to_mbla_list(response_string, &response_string_index, x, y, *position);
}

void server_quit_game(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_QUIT_GAME_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    *response_string->data = QUIT_GAME_NOT_IN_PROGRESS;
    response_string->length++;
    return;
  }

  // Reset all shared variables and lists.
  reset_shared_variables_and_lists();

  // Update response string.
  *response_string->data = QUIT_GAME_NO_ERROR;
  response_string->length += sizeof(unsigned char);
}

void server_disconnect_client(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_DISCONNECT_CLIENT_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }

  // Reset all shared variables and lists.
  reset_shared_variables_and_lists();

  // Add response code to response string.
  *response_string->data = DISCONNECT_CLIENT_NO_ERROR;
  response_string->length += sizeof(unsigned char);
}

void server_shut_down(Data_string* command_string, Data_string* response_string)
{
  // Error handling.
  if(!ensure_valid_length(COMMAND_SHUT_DOWN_REQUIRED_LENGTH, command_string, response_string))
  {
    return;
  }

  // Reset all shared variables and lists.
  reset_shared_variables_and_lists();

  // Add response code to response string.
  *response_string->data = SHUT_DOWN_NO_ERROR;
  response_string->length += sizeof(unsigned char);
}
