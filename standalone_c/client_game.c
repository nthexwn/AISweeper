#include <stdbool.h>
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
static unsigned long client_seconds_finished; // Number of seconds after client
static Status_type game_status; // Status code indicating current game state.
static bool post_finish_sync_performed; // Used to determine how unrevealed tiles should be rendered.

// Display the current game variables and playing field.
static void client_render()
{
  // Display variables.
  printf("Game status: %s\n", status_messages[game_status]);
  printf("Mines not flagged: %u\n", client_mines_not_flagged);
  printf("Height: %u\n", client_current_height);
  printf("Width: %u\n", client_current_width);

  // Display time.
  printf("Seconds elapsed: %lu\n", game_status == GAME_STATUS_IN_PROGRESS ? (unsigned long)time(NULL) -
                                  client_seconds_started : client_seconds_finished);

  // Display playing field.
  for(int y = 0; y < client_current_height; y++)
  {
    for(int x = 0; x < client_current_width; x++)
    {
      unsigned char* position = client_field_begin + (client_current_width * y + x) * sizeof(unsigned char);

      // There is no sensitive position data reamining for revealed positions, always display them accurately
      // regardless of the game state.
      if(is_revealed(position))
      {
        // A '*' character represents a detonated mine.  There will only ever be one of these on the playing field
        // and it will indicate the position where the player performed a reveal and lost.
        if(is_mined(position)) putc('*', stdout);
        else
        {
          // An unmined revealed postion will either be empty ('_') or contain an adjacent mine count.
          unsigned short adjacent_count = get_adjacent(position);
          if(adjacent_count == 0) putc('_', stdout);
          else putc(adjacent_count + DATA_TO_CHARACTER_OFFSET, stdout);
        }
      }
      else
      {
        // If the game hasn't been finished yet then non-revealed positions will either be flagged ('^') or unknown
        // ('?').
        if(game_status < GAME_STATUS_LOST)
        {
          if(is_flagged(position)) putc('^', stdout);
          else putc('?', stdout);
        }
        else
        {
          // If the game has been finished and we've received a sync_game response then we can display the full
          // contents of the playing field regardless of whether the positions have been revealed or not.
          if(post_finish_sync_performed == true)
          {
            // If a position is mined then we'll either mark it as successfully flagged ('+') or missed ('@').  Note
            // that the game can still be won without flagging every mine (the win condition only checks to ensure that
            // all of the non-mines positions have been revealed).
            if(is_mined(position))
            {
              if(is_flagged(position)) putc('+', stdout);
              else putc('@', stdout);
            }
            else
            {
              // The position was flagged, but there's actually no mine present so we'll mark it as incorrect ('-').
              if(is_flagged(position)) putc('-', stdout);
              else
              {
                // An unmined unflagged position will either be empty ('_') or contain an adjacent mine count.
                unsigned short adjacent_count = get_adjacent(position);
                if(adjacent_count == 0) putc('_', stdout);
                else putc(adjacent_count + DATA_TO_CHARACTER_OFFSET, stdout);
              }
            }
          }
          else
          {
            // The game has been finished, but we haven't synced the sensitive position data from the server for
            // non-revealed positions yet so we have no way of knowing what to display for these positions.  The '&'
            // represents an hourglass or waiting icon to indicate that we'll have something accurate to display here
            // once a sync_game response is received.
            putc('&', stdout);
          }
        }
      }
    }
    putc('\n', stdout);
  }
}

void client_action_update(Action_info* action_info)
{
  // Copy variables.
  if(game_status == GAME_STATUS_IN_PROGRESS_NO_REVEAL && action_info->game_status == GAME_STATUS_IN_PROGRESS)
  {
    // We just got the response from our first reveal action, so we'll start the timer now.
    client_seconds_started = (unsigned long)time(NULL);
  }
  game_status = action_info->game_status;
  if(game_status == GAME_STATUS_NOT_IN_PROGRESS)
  {
    // We just quit the game, so delete everything.
    client_current_height = 0;
    client_current_width = 0;
    free(client_field_begin);
    client_field_begin = NULL;
    client_seconds_started = 0;
    client_seconds_finished = 0;
    post_finish_sync_performed = false;
  }
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
  if(game_status < GAME_STATUS_LOST) post_finish_sync_performed = false;
  else post_finish_sync_performed = true;

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

