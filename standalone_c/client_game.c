#include <stdio.h>
#include <stdlib.h>
#include "bits.h"
#include "client_game.h"
#include "constants.h"
#include "time.h"

// Game variables which are shared between functions.
static signed short client_mines_not_flagged; // Number of mines which haven't been flagged.  Can be negative.
static unsigned char client_current_height; // Current height of the playing field.
static unsigned char client_current_width; // Current width of the playing field.
static unsigned char* client_field_begin; // Pointer to first position char in field.
static unsigned long client_seconds_started; // Number of seconds after 12:00AM on 1/1/1970 that the first reveal was
                                             // performed.  Will be 0 if the first reveal has not yet occurred.
static unsigned long client_seconds_finished; // Number of seconds after client_seconds_started that the game
                                              // became finished.
static Status_type game_status; // Status code indicating current game state.

// Display the current game variables and playing field.
static void client_render()
{
  // Display variables.
  printf("Game status: %s", status_messages[game_status]);
  printf("Mines not flagged: %u\n", client_mines_not_flagged);
  printf("Height: %u\n", client_current_height);
  printf("Width: %u\n", client_current_width);

  // Display time.
  printf("Seconds elapsed: %lu\n", game_status == GAME_STATUS_IN_PROGRESS ? (unsigned long)time(NULL) -
                                  client_seconds_started : client_seconds_finished);

  // Display playing field.
  for(int y = 0; y < client_current_height; y++)
  {
    for(int x = 0; x < client_current_width; y++)
    {
      unsigned char* position = client_field_begin + client_current_width * y + x;
      if(is_revealed(position))
      {
        if(is_mined(position)) putc('*', stdout);
        else
        {
          unsigned short adjacent_count = get_adjacent(position);
          if(adjacent_count == 0) putc('_', stdout);
          else putc(adjacent_count + DATA_TO_CHARACTER_OFFSET, stdout);
        }
      }
      else
      {
        if(game_status < GAME_STATUS_LOST)
        {
          if(is_flagged(position)) putc('^', stdout);
          else putc('_', stdout);
        }
        else
        {
          if(is_mined(position))
          {
            if(is_flagged(position)) putc('+', stdout);
            else putc('-', stdout);
          }
          else putc('_', stdout);
        }
      }
    }
    putc('\n', stdout);
  }
}

void client_action_update(Action_info* action_info)
{
  // Copy variables.
  game_status = action_info->game_status;
  client_mines_not_flagged = action_info->mines_not_flagged;

  // Traverse the modified by last action list inside the updated action info structure.
  Copy_node* mbla_index = action_info->mbla_head;
  while(mbla_index != NULL)
  {
    // If there's no active playing field then we'll skip transfering data from the modified by last action list since
    // we have no valid location to transfer it to.
    if(client_field_begin != NULL)
    {
      // Transfer position data from the modified by last action list to the client's local copy of the playing field.
      *(client_field_begin + client_current_width * mbla_index->y + mbla_index->x) = mbla_index->position;
    }

    // Delete each node from the modified by last action list as we traverse it.
    Copy_node* mbla_free = mbla_index;
    mbla_index = mbla_index->next;
    free(mbla_free);
  }

  // Delete the action info structure since we've copied everything we need from it.
  free(action_info);
  client_render();
}

void client_game_update(Game_info* game_info)
{
  // Copy variables.
  game_status = game_info->game_status;
  client_current_height = game_info->height;
  client_current_width = game_info->width;
  client_mines_not_flagged = game_info->mines_not_flagged;

  // Update seconds started and finished variables.
  switch(game_status)
  {
    case GAME_STATUS_NOT_IN_PROGRESS:
    case GAME_STATUS_IN_PROGRESS_NO_REVEAL:
      client_seconds_started = 0;
      client_seconds_finished = 0;
      break;
    case GAME_STATUS_IN_PROGRESS:
      client_seconds_started = (unsigned long)time(NULL) - game_info->seconds_elapsed;
      client_seconds_finished = 0;
      break;
    case GAME_STATUS_WON:
    case GAME_STATUS_LOST:
      client_seconds_started = (unsigned long)time(NULL) - game_info->seconds_elapsed;
      client_seconds_finished = game_info->seconds_elapsed;
      break;
  }

  // Copy playing field.
  if(client_field_begin != NULL) free(client_field_begin);
  client_field_begin = game_info->copy_field_begin;

  // Delete the game info structure since we've copied everything we need from it.
  free(game_info);
  client_render();
}

