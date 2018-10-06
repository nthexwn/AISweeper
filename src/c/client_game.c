#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_game.h"
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"

// Game variables which are shared between functions.
static signed short mines_not_flagged; // Number of mines which haven't been flagged.  Can be negative.
static unsigned char height; // Current height of the playing field.
static unsigned char width; // Current width of the playing field.
static unsigned char* field_begin; // Pointer to first position char in field.
static unsigned char game_status; // Status code indicating current game state.
static bool post_finish_sync_performed; // Used to determine how unrevealed tiles should be rendered.

// Called when either quitting the game or shutting down the server.
static void reset_shared_variables_and_lists()
{
  if(field_begin != NULL)
  {
    free(field_begin);
    field_begin = NULL;
  }
  mines_not_flagged = 0;
  height = 0;
  width = 0;
  game_status = GAME_STATUS_NOT_IN_PROGRESS;
  post_finish_sync_performed = false;
}

static void update_position(unsigned char* source_index)
{
  if(field_begin != NULL)
  {
    unsigned char x = *source_index;
    unsigned char y = *(source_index + sizeof(unsigned char));
    unsigned char position_data = *(source_index + sizeof(unsigned char) * 2);
    unsigned char* field_index = field_begin + width * y + x;
    if(!is_flagged(field_index) && is_flagged(&position_data))
    {
      mines_not_flagged--;
    }
    else if(is_flagged(field_index) && !is_flagged(&position_data))
    {
      mines_not_flagged++;
    }
    *field_index = position_data;
  }
}

void client_start_game(Data_string* command_string, Data_string* response_string)
{
  reset_shared_variables_and_lists();
  transfer_value(command_string->data + COMMAND_START_GAME_MINES_OFFSET * sizeof(unsigned char), ENDIAN_BIG,
                 (unsigned char*)&mines_not_flagged, detect_machine_byte_order(), sizeof(unsigned short));
  height = *(command_string->data + COMMAND_START_GAME_HEIGHT_OFFSET);
  width = *(command_string->data + COMMAND_START_GAME_WIDTH_OFFSET);
  field_begin = (unsigned char*)calloc(height * width, sizeof(unsigned char));
  game_status = GAME_STATUS_IN_PROGRESS_NO_REVEAL;
}

void client_sync_game(Data_string* command_string, Data_string* response_string)
{
  reset_shared_variables_and_lists();
  game_status = *(response_string->data + RESPONSE_SYNC_GAME_GAME_STATUS_OFFSET);
  if(game_status > GAME_STATUS_NOT_IN_PROGRESS)
  {
    transfer_value(response_string->data + RESPONSE_SYNC_GAME_MINES_OFFSET * sizeof(unsigned char), ENDIAN_BIG,
                   (unsigned char*)&mines_not_flagged, detect_machine_byte_order(), sizeof(unsigned short));
    height = *(response_string->data + RESPONSE_SYNC_GAME_HEIGHT_OFFSET);
    width = *(response_string->data + RESPONSE_SYNC_GAME_WIDTH_OFFSET);
    field_begin = (unsigned char*)calloc(height * width, sizeof(unsigned char));
    unsigned char* response_index = response_string->data + RESPONSE_SYNC_GAME_FIELD_COPY_OFFSET;
    unsigned char* field_index = field_begin;
    for(int i = 0; i < height * width; i++)
    {
      if(is_flagged(response_index))
      {
        mines_not_flagged--;
      }
      *field_index = *response_index;
      response_index += sizeof(unsigned char);
      field_index += sizeof(unsigned char);
    }
    if(game_status > GAME_STATUS_IN_PROGRESS)
    {
      post_finish_sync_performed = true;
    }
  }
}

void client_reveal_position(Data_string* command_string, Data_string* response_string)
{
  game_status = *(response_string->data + RESPONSE_REVEAL_POSITION_GAME_STATUS_OFFSET);
  int mbla_length = response_string->length - RESPONSE_REVEAL_POSITION_MBLA_HEAD_OFFSET / sizeof(unsigned char);
  for(unsigned char* i = response_string->data + RESPONSE_REVEAL_POSITION_MBLA_HEAD_OFFSET; i < response_string->data +
      mbla_length; i += 3 * sizeof(unsigned char))
  {
    update_position(i);
  }
}

void client_toggle_flag(Data_string* command_string, Data_string* response_string)
{
  update_position(response_string->data + RESPONSE_TOGGLE_FLAG_MBLA_HEAD_OFFSET);
}

void client_quit_game(Data_string* command_string, Data_string* response_string)
{
  reset_shared_variables_and_lists();
}

void client_disconnect_client(Data_string* command_string, Data_string* response_string)
{
  reset_shared_variables_and_lists();
}

void client_shut_down(Data_string* command_string, Data_string* response_string)
{
  reset_shared_variables_and_lists();
}

// Display the current game variables and playing field.
void client_render()
{
  // Display variables.
  fprintf(stdout, "Game status: %s\n", game_status_strings[game_status]);
  fprintf(stdout, "Mines not flagged: %u\n", mines_not_flagged);
  fprintf(stdout, "Height: %u\n", height);
  fprintf(stdout, "Width: %u\n", width);

  // Display playing field.
  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      unsigned char* position = field_begin + (width * y + x) * sizeof(unsigned char);

      // There is no sensitive position data remaining for revealed positions, always display them accurately
      // regardless of the game state.
      if(is_revealed(position))
      {
        // A '*' character represents a detonated mine.  There will only ever be one of these on the playing field
        // and it will indicate the position where the player performed a reveal and lost.
        if(is_mined(position)) fputc('*', stdout);
        else
        {
          // An unmined revealed postion will either be empty ('_') or contain an adjacent mine count.
          unsigned short adjacent_count = get_adjacent(position);
          if(adjacent_count == 0) fputc('_', stdout);
          else fputc(adjacent_count + DATA_TO_CHARACTER_OFFSET, stdout);
        }
      }
      else
      {
        // If the game hasn't been finished yet then non-revealed positions will either be flagged ('^') or unknown
        // ('?').
        if(game_status < GAME_STATUS_LOST)
        {
          if(is_flagged(position)) fputc('^', stdout);
          else fputc('?', stdout);
        }
        else
        {
          // If the game has been finished and we've received a sync_game response then we can display the full
          // contents of the playing field regardless of whether the positions have been revealed or not.
          if(post_finish_sync_performed == true)
          {
            // If a position is mined then we'll either mark it as successfully flagged ('+') or missed ('@').  Note
            // that the game can still be won without flagging every mine (the win condition only checks to ensure that
            // all of the non-mined positions have been revealed).
            if(is_mined(position))
            {
              if(is_flagged(position)) fputc('+', stdout);
              else fputc('@', stdout);
            }
            else
            {
              // The position was flagged, but there's actually no mine present so we'll mark it as incorrect ('-').
              if(is_flagged(position)) fputc('-', stdout);
              else
              {
                // An unmined unflagged position will either be empty ('_') or contain an adjacent mine count.
                unsigned short adjacent_count = get_adjacent(position);
                if(adjacent_count == 0) fputc('_', stdout);
                else fputc(adjacent_count + DATA_TO_CHARACTER_OFFSET, stdout);
              }
            }
          }
          else
          {
            // The game has been finished, but we haven't synced the sensitive position data from the server for
            // non-revealed positions yet so we have no way of knowing what to display for these positions.  The '&'
            // represents an hourglass or waiting icon to indicate that we'll have something accurate to display here
            // once a sync_game response is received.
            fputc('&', stdout);
          }
        }
      }
    }
    fputc('\n', stdout);
  }
}

