#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "interface.h"
#include "server_game.h"
#include "utilities.h"

// Helper function to determine if the command+argument data in the command string is of the length required by the
// command handler.  If it's not then the command handler will return an error instead of calling its corresponding
// command function.  Note that no validation is done on the actual argument contents themselves.  They are all
// interpreted as raw numeric values without any delimiters.  Those values are then passed to the individual commands.
// It's up to the commands themselves to assess the values passed in and ensure that they fall within acceptable bounds
// or make sense in the specific context of the command.
static bool ensure_valid_length(unsigned short valid_length, Data_string* argument_string,
                                Data_string* response_string)
{
  if(argument_string->length < valid_length)
  {
    *(response_string->data + response_string->length * sizeof(unsigned char)) = COMMAND_INSUFFICIENT_ARGUMENT_DATA;
    response_string->length++;
    return false;
  }
  if(argument_string->length > valid_length)
  {
    *(response_string->data + response_string->length * sizeof(unsigned char)) = COMMAND_EXCESSIVE_ARGUMENT_DATA;
    response_string->length++;
    return false;
  }
  return true;
}

// Helper method for serializing action information prior to returning it to the client.  Deletes action_info structure
// and contents after serialization.
static void serialize_action_info(Action_info* action_info, Data_string* response_string)
{
  // Set up index at beginning of response string.
  unsigned char* response_string_index = response_string->data;

  // Add error type to response string.
  *response_string_index = (unsigned char)action_info->error_type;
  response_string->length++;
  response_string_index += sizeof(unsigned char);

  // We'll only add the remaining data if there was no error when processing the command.  If there was an error then
  // the command will have been aborted, nothing in the game will have changed, and the remaining information would be
  // redundant.
  if(action_info->error_type % ERROR_GROUP_WIDTH == 0)
  {
    // Add game status to the response string.
    *response_string_index = action_info->game_status;
    response_string->length++;
    response_string_index += sizeof(unsigned char);

    // Add unflagged mines count to the response string.
    transfer_value((unsigned char*)&action_info->mines_not_flagged, machine_endian(), response_string_index,
                   ENDIAN_BIG, sizeof(signed short));
    response_string->length += sizeof(signed short);
    response_string_index += sizeof(signed short);

    // Add modified positions to response string.
    unsigned short mbla_count = 0;
    unsigned char* mbla_count_index = response_string_index;
    response_string->length += sizeof(unsigned short);
    response_string_index += sizeof(unsigned short);
    Copy_node* mbla_index = action_info->mbla_head;

    while(mbla_index != NULL)
    {
      // Add x-coordinate of modified position.
      *response_string_index = mbla_index->x;
      response_string->length++;
      response_string_index += sizeof(unsigned char);

      // Add y-coordinate of modified position.
      *response_string_index = mbla_index->y;
      response_string->length++;
      response_string_index += sizeof(unsigned char);

      // Add modified position data.
      *response_string_index = mbla_index->position;
      response_string->length++;
      response_string_index += sizeof(unsigned char);

      // Advance to next position in modified by last action list (and delete the last node).
      Copy_node* mbla_free = mbla_index;
      mbla_index = mbla_index->next;
      free(mbla_free);
      mbla_count++;
    }

    // Add modified positions count to response string.
    transfer_value((unsigned char*)&mbla_count, machine_endian(), mbla_count_index, ENDIAN_BIG,
                   sizeof(unsigned short));
  }

  // Delete action info structure since it's now fully serialized.
  free(action_info);
}

// Helper method for serializing game information prior to returning it to the client.  Deletes game_info structure and
// contents after serializing.
static void serialize_game_info(Game_info* game_info, Data_string* response_string)
{
  // Set up index at beginning of response string.
  unsigned char* response_string_index = response_string->data;

  // Add error type to response string.
  *response_string_index = (unsigned char)game_info->error_type;
  response_string->length++;
  response_string_index += sizeof(unsigned char);

  // We'll only add the remaining data if there was no error when processing the command.  If there was an error then
  // the command will have been aborted, nothing in the game will have changed, and the remaining information would be
  // redundant.
  if(game_info->error_type % ERROR_GROUP_WIDTH == 0)
  {
    // Add game status to the response string.
    *response_string_index = game_info->game_status;
    response_string->length++;
    response_string_index += sizeof(unsigned char);

    // Add unflagged mines count to the response string.
    transfer_value((unsigned char*)&game_info->mines_not_flagged, machine_endian(), response_string_index,
                   ENDIAN_BIG, sizeof(signed short));
    response_string->length += sizeof(signed short);
    response_string_index += sizeof(signed short);

    // Add height to the response string.
    *response_string_index = game_info->height;
    response_string->length++;
    response_string_index += sizeof(unsigned char);

    // Add width to the response string.
    *response_string_index = game_info->width;
    response_string->length++;
    response_string_index += sizeof(unsigned char);

    // Add copied playing field to the response string.
    if(game_info->copy_field_begin != NULL)
    {
      unsigned short length = game_info->height * game_info->width;
      response_string->length += length;
      for(unsigned char* copy_field_index = game_info->copy_field_begin; copy_field_index <
          game_info->copy_field_begin + length * sizeof(unsigned char); copy_field_index++)
      {
        *response_string_index = *copy_field_index;
        response_string_index += sizeof(unsigned char);
      }

      // Delete the copied playing field now that we've serialized it.
      free(game_info->copy_field_begin);
    }
  }

  // Delete game info structure since it's now fully serialized.
  free(game_info);
}

// Command handling functions.
static void call_shut_down(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(SHUT_DOWN_EXPECTED_LENGTH, argument_string, response_string))
  {
    // Since we're trying to shut down the whole program we don't care if there's no game in progress.  We'll remove
    // the error message for this before responding to the client.
    Action_info* action_info = quit_game();
    if(action_info->error_type == QUIT_GAME_NO_ERROR || action_info->error_type == QUIT_GAME_NOT_IN_PROGRESS)
    {
      action_info->error_type = SHUT_DOWN_NO_ERROR;
    }
    else if(action_info->error_type == QUIT_GAME_OUT_OF_MEMORY)
    {
      action_info->error_type = SHUT_DOWN_OUT_OF_MEMORY;
    }
    serialize_action_info(action_info, response_string);
  }
}
static void call_start_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(START_GAME_EXPECTED_LENGTH, argument_string, response_string))
  {
    unsigned char height = *(argument_string->data + START_GAME_HEIGHT_OFFSET);
    unsigned char width = *(argument_string->data + START_GAME_WIDTH_OFFSET);
    unsigned short mines = 0;
    transfer_value(argument_string->data + START_GAME_MINES_OFFSET, ENDIAN_BIG, (unsigned char*)&mines,
                   machine_endian(), sizeof(unsigned short));
    serialize_game_info(start_game(height, width, mines), response_string);
  }
}
static void call_sync_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(SYNC_GAME_EXPECTED_LENGTH, argument_string, response_string))
  {
    serialize_game_info(sync_game(), response_string);
  }
}
static void call_reveal(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(REVEAL_EXPECTED_LENGTH, argument_string, response_string))
  {
    unsigned short x = *(argument_string->data + REVEAL_X_OFFSET);
    unsigned short y = *(argument_string->data + REVEAL_Y_OFFSET);
    serialize_action_info(reveal(x, y), response_string);
  }
}
static void call_toggle_flag(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(TOGGLE_FLAG_EXPECTED_LENGTH, argument_string, response_string))
  {
    unsigned short x = *(argument_string->data + TOGGLE_FLAG_X_OFFSET);
    unsigned short y = *(argument_string->data + TOGGLE_FLAG_Y_OFFSET);
    serialize_action_info(toggle_flag(x, y), response_string);
  }
}
static void call_quit_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(QUIT_GAME_EXPECTED_LENGTH, argument_string, response_string))
  {
    serialize_action_info(quit_game(), response_string);
  }
}

// Program entry point.
int main()
{
  // Initialize command handlers.
  void (*command_handlers[NUMBER_OF_SUPPORTED_COMMANDS])(Data_string*, Data_string*);
  command_handlers[COMMAND_SHUT_DOWN] = &call_shut_down;
  command_handlers[COMMAND_START_GAME] = &call_start_game;
  command_handlers[COMMAND_SYNC_GAME] = &call_sync_game;
  command_handlers[COMMAND_REVEAL] = &call_reveal;
  command_handlers[COMMAND_TOGGLE_FLAG] = &call_toggle_flag;
  command_handlers[COMMAND_QUIT_GAME] = &call_quit_game;

  // Initialize the command and response string structures.
  Data_string* command_string = (Data_string*)malloc(sizeof(Data_string));
  Data_string* response_string = (Data_string*)malloc(sizeof(Data_string));

  // Stay in the main loop until a shut down command is explicitly received and the game has been successfully shut
  // down.
  bool shut_down_requested = false;
  while(!shut_down_requested)
  {
    // Get input from the client.
    command_string->data = (unsigned char*)malloc(MAXIMUM_POSSIBLE_COMMAND_LENGTH);
    command_string->length = 0;
    obtain_command(command_string);

    // TODO: Debug.
    printf("Server received binary command: ");
    print_bits(command_string->data, command_string->length);
    printf("\n");

    // Initialize response string.
    response_string->data = (unsigned char*)malloc(MAXIMUM_POSSIBLE_RESPONSE_LENGTH);
    response_string->length = 0;

    // Make sure the command string isn't empty.
    if(command_string->length == 0)
    {
      *response_string->data = COMMAND_NO_INPUT;
      response_string->length++;
    }
    else
    {
      // Retrieve the command code from the command string.
      unsigned char command_type = *command_string->data;

      // Make sure the command code represents a valid command.
      if(command_type > (sizeof(command_handlers)/sizeof(void(*)(Data_string*, Data_string*))) - 1)
      {
        *response_string->data = COMMAND_CODE_NOT_VALID;
        response_string->length++;
      }
      else
      {
        // Call the command handler function.  It will update the response string internally after calling the command.
        (*command_handlers[command_type])(command_string, response_string);

        // If we've received a shut-down command and the game quit successfully then we're ready to exit the program.
        if(command_type == COMMAND_SHUT_DOWN && *response_string->data == SHUT_DOWN_NO_ERROR) shut_down_requested =
                                                                                              true;
      }
    }

    // TODO: Debug.
    printf("Server sending binary response: ");
    print_bits(response_string->data, response_string->length);
    printf("\n");

    // Send the response string back to the client interface.
    handle_response(response_string);

    // Delete the input/output strings from this command/response cycle.
    free(command_string->data);
    command_string->length = 0;
    free(response_string->data);
    response_string->length = 0;
  }

  // Delete the command and response string structures.
  free(command_string);
  free(response_string);
  return GENERAL_NO_ERROR;
}

