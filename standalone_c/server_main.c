#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "server_game.h"
#include "server_interface.h"

// Command handling functions.
static void call_shut_down(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}
static void call_start_game(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}
static void call_sync_game(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}
static void call_reveal(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}
static void call_toggle_flag(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}
static void call_quit_game(unsigned char* argument_string, unsigned char** response_string)
{
  //TODO
}

int main()
{
  // Initialize command handlers.
  void (*command_handlers[6])(unsigned char*, unsigned char**);
  command_handlers[0] = &call_shut_down;
  command_handlers[1] = &call_start_game;
  command_handlers[2] = &call_sync_game;
  command_handlers[3] = &call_reveal;
  command_handlers[4] = &call_toggle_flag;
  command_handlers[5] = &call_quit_game;

  // Stay in the main loop until a shut_down command is explicitly declared by the client.
  bool shut_down_requested = false;
  while(!shut_down_requested)
  {
    // Get input from the client.
    unsigned char* command_string = obtain_command();
    unsigned char* command_string_index = command_string;

    // Initialize response string
    unsigned char* response_string = (unsigned char*)malloc(MAXIMUM_POSSIBLE_RESPONSE_LENGTH);
    unsigned char* response_string_index = response_string;

    // Make sure the input string isn't null.
    if(command_string_index == NULL)
    {
      *response_string_index = COMMAND_NO_INPUT;
      response_string_index++;
    }
    else
    {
      // Retrieve the command code from the input string.
      unsigned char command_code = *command_string_index;
      command_string_index++;

      // Make sure the command code represents a valid command.
      if(command_code > (sizeof(command_handlers)/sizeof(void(*)(unsigned char*, unsigned char**))) - 1)
      {
        *response_string_index = COMMAND_DOES_NOT_EXIST;
        response_string_index++;
      }
      else
      {
        // Call the command handler function.  It will update the response string internally after calling the command.
        (*command_handlers[command_code])(command_string_index, &response_string_index);
      }
    }

    // Ensure that the response string is null terminated.
    *response_string_index = CHAR_NULL;

    // Send the response string back to the client interface.
    handle_response(response_string);

    // Erase the input/output strings from this command/response cycle.
    free(command_string);
    free(response_string);
  }
  return 0;
}

