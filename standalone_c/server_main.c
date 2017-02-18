#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "server_game.h"
#include "server_interface.h"
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
    *(response_string->data + response_string->length) = COMMAND_INSUFFICIENT_ARGUMENT_DATA;
    response_string->length++;
    return false;
  }
  if(argument_string->length > valid_length)
  {
    *(response_string->data + response_string->length) = COMMAND_EXCESSIVE_ARGUMENT_DATA;
    response_string->length++;
    return false;
  }
  return true;
}

// Helper methods for serializing game information prior to returning it to the client.
static void serialize_action_info(Action_info* action_info, Data_string* response_string)
{
  // Set up index at beginning of response string.
  unsigned char* response_string_index = response_string->data;

  // Add error type to response string.
  *response_string_index = (unsigned char)action_info->error_type;
  response_string->length++;
  response_string_index++;

  // We'll only add the remaining data if there was no error when processing the command.  If there was an error then
  // the command will have been aborted, nothing in the game will have changed, and the remaining information would be
  // redundant.
  if(action_info->error_type == GENERAL_NO_ERROR)
  {
    // Add game status to the response string.
    *response_string_index = action_info->game_status;
    response_string->length++;
    response_string_index++;

    // Add unflagged mines count to the response string.
    // TODO
  }

  // TODO: Delete Action_info struct and contents after serializing them.
}
static void serialize_game_info(Game_info* game_info, Data_string* response_string)
{
  // TODO

  // TODO: Delete Game_info struct and contents after serializing them.
}

// Command handling functions.
static void call_start_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(5, argument_string, response_string))
  {
    unsigned char* height = argument_string->data + 1;
    unsigned char* width = argument_string->data + 2;
    unsigned short* mines = (unsigned short*)extract_value(argument_string->data + 3, sizeof(unsigned short),
                                                           machine_endian());
    serialize_game_info(start_game(*height, *width, *mines), response_string);
    free(mines);
  }
}
static void call_sync_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(1, argument_string, response_string))
  {
    serialize_game_info(sync_game(), response_string);
  }
}
static void call_reveal(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(3, argument_string, response_string))
  {
    unsigned short x = *(argument_string->data + 1);
    unsigned short y = *(argument_string->data + 2);
    serialize_action_info(reveal(x, y), response_string);
  }
}
static void call_toggle_flag(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(3, argument_string, response_string))
  {
    unsigned short x = *(argument_string->data + 1);
    unsigned short y = *(argument_string->data + 2);
    serialize_action_info(toggle_flag(x, y), response_string);
  }
}
static void call_quit_game(Data_string* argument_string, Data_string* response_string)
{
  if(ensure_valid_length(1, argument_string, response_string))
  {
    serialize_action_info(quit_game(), response_string);
  }
}

void test_function(signed short test_input)
{
  printf("before (numeric): %d\n", test_input);
  printf(" before (binary): ");
  print_bits((unsigned char*)&test_input, sizeof(test_input) * BITS_PER_CHAR);
  signed short* test_output = (signed short*)extract_value((unsigned char*)&test_input, sizeof(test_input),
                                                           ENDIAN_BIG);
  printf("  after (binary): ");
  print_bits((unsigned char*)test_output, sizeof(test_input) * BITS_PER_CHAR);
  free(test_output);
}

// TODO: Delete this temporary testing stuff.
int main()
{
  test_function(-2);
  test_function(-1);
  test_function(0);
  test_function(1);
  test_function(2);
}

// Program entry point.
int whatevs()
{
  // Initialize command handlers.
  void (*command_handlers[6])(Data_string*, Data_string*);
  command_handlers[0] = &call_quit_game; // shut down.
  command_handlers[1] = &call_start_game;
  command_handlers[2] = &call_sync_game;
  command_handlers[3] = &call_reveal;
  command_handlers[4] = &call_toggle_flag;
  command_handlers[5] = &call_quit_game;

  // Stay in the main loop until a shut down command is explicitly made by the client.
  bool shut_down_requested = false;
  while(!shut_down_requested)
  {
    // Get input from the client.
    Data_string command_string = obtain_command();

    // Initialize response string.
    Data_string response_string = (Data_string){0, (unsigned char*)malloc(MAXIMUM_POSSIBLE_RESPONSE_LENGTH)};

    // Make sure the command string isn't empty.
    if(command_string.data == NULL || command_string.length == 1)
    {
      *response_string.data = COMMAND_NO_INPUT;
      response_string.length++;
    }
    else
    {
      // Retrieve the command code from the command string.
      unsigned char command_code = *command_string.data;

      // Make sure the command code represents a valid command.
      if(command_code > (sizeof(command_handlers)/sizeof(void(*)(Data_string*, Data_string*))) - 1)
      {
        *response_string.data = COMMAND_DOES_NOT_EXIST;
        response_string.length++;;
      }
      else
      {
        // If we've received a shut-down command then set the flag to exit the main loop.
        if(command_code == 0) shut_down_requested = true;

        // Call the command handler function.  It will update the response string internally after calling the command.
        (*command_handlers[command_code])(&command_string, &response_string);
      }
    }

    // Send the response string back to the client interface.
    handle_response(&response_string);

    // Erase the input/output strings from this command/response cycle.
    free(command_string.data);
    free(response_string.data);
  }
  return GENERAL_NO_ERROR;
}

