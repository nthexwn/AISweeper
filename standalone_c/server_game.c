#define _GNU_SOURCE

#include <stdio.h> // TODO:  Remove this along with temporary output in this file once testing is complete.
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>
#include "bits.h"
#include "constants.h"
#include "mt19937.h"
#include "server_game.h"

// Linked list structure for playing field positions.  Ref_nodes refer to the actual contents of the playing field on
// the server.  They should only be used internally by the core game functions so that the client cannot cheat by
// inspecting the true contents of the playing field (IE: whether or not a position contains a mine or an unrevealed
// position has adjacent mines).
typedef struct ref_node
{
  unsigned char* position;
  unsigned char x;
  unsigned char y;
  struct ref_node* next;
} Ref_node;

// Helper function for clearing linked lists.
static void clear_ref_list(Ref_node* ref_head, Ref_node* ref_tail)
{
  while(ref_head != NULL)
  {
    Ref_node* ref_free = ref_head;
    ref_head = ref_head->next;
    free(ref_free);
  }
  ref_tail = NULL;
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
static unsigned long seconds_started; // Number of seconds after 12:00AM 1/1/1970 that the first reveal was performed.
static unsigned long seconds_finished; // Number of seconds after seconds_started that the game was finished.
static Status_type game_status; // Status code indicating current game state.

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

  // Start the timer.
  seconds_started = (unsigned long)time(NULL);

  // First reveal has been performed.  Subsequent reveals in the current game will no longer trigger this function.
  game_status = GAME_STATUS_IN_PROGRESS;
}

// Helper method for reveal actions.  Used to reveal unmined nodes in a specificed direction adjacent to the current
// position at the head of the reveal queue.  The x_mod and y_mod characters determine the direction.  If the
// directional position is also not adjacent to any other mines then it will be added to the tail of the reveal queue.
// Subsequent calls to this function will be made for every direction surrounding every position in the reveal queue.
// Utilizing this queue structure is slightly less memory and performance intensive than making these calls recursively
// would be.  Each time this method is called the revealed position will be added to the modified by last action list
// (mbla) which is ultimately returned to the client.
static void handle_reveal_queue_direction(Ref_node** rq_head, Ref_node** rq_tail, Copy_node** mbla_tail,
                                          signed char x_mod, signed char y_mod)
{
  unsigned char* position = field_begin + current_width * ((*rq_head)->y + y_mod) + (*rq_head)->x + x_mod;
  if(!is_mined(position) && !is_revealed(position))
  {
    // Note that since the positions modified here are already revealed that we don't need to perform any filtering of
    // mine or adjacency info when adding these positions to the modified by last action list.  There's no remaining
    // information about the positions that would enable the client to cheat.
    set_revealed(position, true);
    (*mbla_tail)->next = (Copy_node*)malloc(sizeof(Copy_node));
    *(*mbla_tail)->next = (Copy_node){*position, (*rq_head)->x + x_mod, (*rq_head)->y + y_mod, NULL};
    *mbla_tail = (*mbla_tail)->next;
    unmined_positions_remaining--;
    if(get_adjacent(position) == 0)
    {
      (*rq_tail)->next = (Ref_node*)malloc(sizeof(Ref_node));
      *(*rq_tail)->next = (Ref_node){position, (*rq_head)->x + x_mod, (*rq_head)->y + y_mod, NULL};
      *rq_tail = (*rq_tail)->next;
    }
  }
}

Game_info* start_game(unsigned char height, unsigned char width, unsigned short mines)
{
  // Error handling.
  Game_info* game_info = (Game_info*)malloc(sizeof(Game_info));
  if(game_status != GAME_STATUS_NOT_IN_PROGRESS)
  {
    game_info->error_type = START_GAME_ALREADY_IN_PROGRESS;
    return game_info;
  }
  if(height < NEW_GAME_MIN_HEIGHT)
  {
    game_info->error_type = START_GAME_HEIGHT_TOO_SMALL;
    return game_info;
  }
  if(height > NEW_GAME_MAX_HEIGHT)
  {
    game_info->error_type = START_GAME_HEIGHT_TOO_LARGE;
    return game_info;
  }
  if(width < NEW_GAME_MIN_WIDTH)
  {
    game_info->error_type = START_GAME_WIDTH_TOO_SMALL;
    return game_info;
  }
  if(width > NEW_GAME_MAX_WIDTH)
  {
    game_info->error_type = START_GAME_WIDTH_TOO_LARGE;
    return game_info;
  }
  if(mines < NEW_GAME_MIN_MINES)
  {
    game_info->error_type = START_GAME_NOT_ENOUGH_MINES;
    return game_info;
  }
  if(mines > (height - 1) * (width - 1))
  {
    game_info->error_type = START_GAME_TOO_MANY_MINES;
    return game_info;
  }

  // Initialize shared variables.
  mines_not_flagged = mines;
  unmined_positions_remaining = height * width;
  current_mines = mines;
  current_height = height;
  current_width = width;
  seconds_started = 0;
  seconds_finished = 0;

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
      *nm_tail->next = (Ref_node){position, x, y, NULL};
      nm_tail = nm_tail->next;
    }
  }
  Ref_node* nm_free = nm_head;
  nm_head = nm_head->next;
  free(nm_free);

  // Game is ready to play!
  game_status = GAME_STATUS_IN_PROGRESS_NO_REVEAL;

  // Return with new game info.
  game_info->error_type = START_GAME_NO_ERROR;
  game_info->game_status = game_status;
  game_info->height = current_height;
  game_info->width = current_width;
  game_info->mines_not_flagged = mines_not_flagged;
  game_info->seconds_elapsed = 0; // Timer won't be started until the first reveal has been performed.
  game_info->copy_field_begin = NULL;
  return game_info;
}

Game_info* sync_game()
{
  // Error handling.
  Game_info* game_info = (Game_info*)malloc(sizeof(Game_info));
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    game_info->error_type = SYNC_GAME_NOT_IN_PROGRESS;
    return game_info;
  }

  // Create a copy of the game field if any positions have already been revealed (otherwise there's nothing to sync).
  unsigned char* copy_field_begin = NULL;
  if(game_status > GAME_STATUS_IN_PROGRESS_NO_REVEAL)
  {
    copy_field_begin = (unsigned char*)calloc(current_height * current_width, sizeof(unsigned char));
    unsigned char* copy_field_index = copy_field_begin;
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
        *copy_field_index = position;
        copy_field_index += sizeof(unsigned char);
      }
    }
  }

  // Return with the current game info and copy of the game field.
  game_info->error_type = SYNC_GAME_NO_ERROR;
  game_info->game_status = game_status;
  game_info->height = current_height;
  game_info->width = current_width;
  game_info->mines_not_flagged = mines_not_flagged;
  switch(game_status)
  {
    case GAME_STATUS_NOT_IN_PROGRESS:
    case GAME_STATUS_IN_PROGRESS_NO_REVEAL:
      game_info->seconds_elapsed = 0;
      break;
    case GAME_STATUS_IN_PROGRESS:
      game_info->seconds_elapsed = (unsigned long)time(NULL) - seconds_started;
      break;
    case GAME_STATUS_WON:
    case GAME_STATUS_LOST:
      game_info->seconds_elapsed = seconds_finished;
      break;
  }
  game_info->copy_field_begin = copy_field_begin;
  return game_info;
}

Action_info* reveal(unsigned char x, unsigned char y)
{
  // Error handling.
  Action_info* action_info = (Action_info*)malloc(sizeof(Action_info));
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    action_info->error_type = REVEAL_GAME_NOT_IN_PROGRESS;
    return action_info;
  }
  if(game_status > GAME_STATUS_IN_PROGRESS)
  {
    action_info->error_type = REVEAL_GAME_ALREADY_FINISHED;
    return action_info;
  }
  if(x >= current_width)
  {
    action_info->error_type = REVEAL_X_COORDINATE_TOO_HIGH;
    return action_info;
  }
  if(y >= current_height)
  {
    action_info->error_type = REVEAL_Y_COORDINATE_TOO_HIGH;
    return action_info;
  }
  unsigned char* position = field_begin + (current_width * y + x) * sizeof(unsigned char);
  if(is_revealed(position))
  {
    action_info->error_type = REVEAL_MUST_BE_UNREVEALED;
    return action_info;
  }

  // Setup playing field if this is the first time the player has performed a reveal.
  if(game_status == GAME_STATUS_IN_PROGRESS_NO_REVEAL)
  {
    first_reveal_setup(x, y);
  }

  // Perform reveal.
  set_revealed(position, true);

  // Add the revealed position to the modified by last action list.
  Copy_node* mbla_head = (Copy_node*)malloc(sizeof(Copy_node));
  *mbla_head = (Copy_node){*position, x, y, NULL};
  Copy_node* mbla_tail = mbla_head;

  // Handle lose condition.
  if(is_mined(position))
  {
    // Boom!
    game_status = GAME_STATUS_LOST;
    seconds_finished = (unsigned long)time(NULL) - seconds_started;
    action_info->error_type = GENERAL_NO_ERROR;
    action_info->game_status = game_status;
    action_info->mines_not_flagged = mines_not_flagged;
    action_info->mbla_head = mbla_head;
    return action_info;
  }
  unmined_positions_remaining--;

  // Populate the reveal queue starting with the first revealed position.
  Ref_node* rq_head = (Ref_node*)malloc(sizeof(Ref_node));
  *rq_head = (Ref_node){position, x, y, NULL};
  Ref_node* rq_tail = rq_head;

  // Begin revealing additional positions and adding surrounding unmined+unrevealed+unadjacent positions to the queue.
  // Revealing is performed first as a method of marking which positions have already been added to the queue (in order
  // to prevent duplicate entries).
  while(rq_head != NULL)
  {
    if(rq_head->x > 0 && rq_head->y > 0)
    {
      // Above left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, -1, -1);
    }
    if(rq_head->y > 0)
    {
      // Above.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, 0, -1);
    }
    if(rq_head->x < current_width - 1 && rq_head->y > 0)
    {
      // Above right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, 1, -1);
    }
    if(rq_head->x > 0)
    {
      // Left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, -1, 0);
    }
    if(rq_head->x < current_width - 1)
    {
      // Right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, 1, 0);
    }
    if(rq_head->x > 0 && rq_head->y < current_height - 1)
    {
      // Below left.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, -1, 1);
    }
    if(rq_head->y < current_height - 1)
    {
      // Below.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, 0, 1);
    }
    if(rq_head->x < current_width - 1 && rq_head->y < current_height - 1)
    {
      // Below right.
      handle_reveal_queue_direction(&rq_head, &rq_tail, &mbla_tail, 1, 1);
    }

    // Chop off the reveal queue head and advance to the next position.
    Ref_node* rq_free = rq_head;
    rq_head = rq_head->next;
    free(rq_free);
    rq_free = NULL;
  }
  rq_tail = NULL;

  // Handle win condition.
  if(unmined_positions_remaining == 0)
  {
    game_status = GAME_STATUS_WON;
    seconds_finished = (unsigned long)time(NULL) - seconds_started;
  }

  // Return with results of reveal action and list of revealed positions.
  action_info->error_type = REVEAL_NO_ERROR;
  action_info->game_status = game_status;
  action_info->mines_not_flagged = mines_not_flagged;
  action_info->mbla_head = mbla_head;
  return action_info;
}

Action_info* toggle_flag(unsigned char x, unsigned char y)
{
  // Error handling.
  Action_info* action_info = (Action_info*)malloc(sizeof(Action_info));
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    action_info->error_type = TOGGLE_FLAG_GAME_NOT_IN_PROGRESS;
    return action_info;
  }
  if(game_status > GAME_STATUS_IN_PROGRESS)
  {
    action_info->error_type = TOGGLE_FLAG_GAME_ALREADY_FINISHED;
    return action_info;
  }
  if(x >= current_width)
  {
    action_info->error_type = TOGGLE_FLAG_X_COORDINATE_TOO_HIGH;
    return action_info;
  }
  if(y >= current_height)
  {
    action_info->error_type = TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH;
    return action_info;
  }
  unsigned char* position = field_begin + current_width * y + x;
  if(is_revealed(position))
  {
    action_info->error_type = TOGGLE_FLAG_MUST_BE_UNREVEALED;
    return action_info;
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

  // Update modified by last action list with a filtered copy of the toggled position.
  Copy_node* mbla_head = (Copy_node*)malloc(sizeof(Copy_node));
  *mbla_head = (Copy_node){*position & ~BITS_SENSITIVE, x, y, NULL};

  // Return with results of toggle flag action and list containing toggled position.
  action_info->error_type = TOGGLE_FLAG_NO_ERROR;
  action_info->game_status = game_status;
  action_info->mines_not_flagged = mines_not_flagged;
  action_info->mbla_head = mbla_head;
  return action_info;
}

Action_info* quit_game()
{
  // Error handling.
  Action_info* action_info = (Action_info*)malloc(sizeof(Action_info));
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    action_info->error_type = QUIT_GAME_NOT_IN_PROGRESS;
    return action_info;
  }

  // Reset all shared variables and lists.
  mines_not_flagged = 0;
  unmined_positions_remaining = 0;
  current_mines = 0;
  current_height = 0;
  current_width = 0;
  free(field_begin);
  field_begin = NULL;
  clear_ref_list(nm_head, nm_tail);
  seconds_started = 0;
  seconds_finished = 0;
  game_status = GAME_STATUS_NOT_IN_PROGRESS;

  // Return with updated action info.
  action_info->error_type = QUIT_GAME_NO_ERROR;
  action_info->game_status = game_status;
  action_info->mines_not_flagged = mines_not_flagged;
  action_info->mbla_head = NULL;
  return action_info;
}

// TODO: Remove this temporary output once testing is complete.
void display_as_client()
{
  printf("Field from client perspective:\n");
  for(unsigned char y = 0; y < current_height; y++)
  {
    for(unsigned char x = 0; x < current_width; x++)
    {
      unsigned char* position = field_begin + (current_width * y + x) * sizeof(unsigned char);
      if(!is_revealed(position))
      {
        printf("?");
      }
      else
      {
        if(is_mined(position))
        {
          printf("X");
        }
        else
        {
          unsigned char adjacent_mines = get_adjacent(position);
          if(adjacent_mines != 0) printf("%d", adjacent_mines);
          else printf("_");
        }
      }
    }
    printf("\n");
  }
  printf("\n");
}

// TODO: Remove this temporary output once testing is complete.
void display_as_server()
{
  printf("Field from server perspective:\n");
  for(unsigned char y = 0; y < current_height; y++)
  {
    for(unsigned char x = 0; x < current_width; x++)
    {
      unsigned char* position = field_begin + (current_width * y + x) * sizeof(unsigned char);
      if(is_mined(position))
      {
        printf("X");
      }
      else
      {
        unsigned char adjacent_mines = get_adjacent(position);
        if(adjacent_mines != 0) printf("%d", adjacent_mines);
        else printf("_");
      }
    }
    printf("\n");
  }
  printf("\n");
}

