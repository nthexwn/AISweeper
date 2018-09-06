#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"
#include "common_structs.h"
#include "server_game.h"
#include "server_interface.h"

// Program entry point.
int main()
{
  // Initialize command handlers.
  void (*command_handlers[SUPPORTED_COMMAND_COUNT])(Data_string*, Data_string*);
  command_handlers[COMMAND_SHUT_DOWN] = &server_shut_down;
  command_handlers[COMMAND_START_GAME] = &server_start_game;
  command_handlers[COMMAND_SYNC_GAME] = &server_sync_game;
  command_handlers[COMMAND_REVEAL_POSITION] = &server_reveal_position;
  command_handlers[COMMAND_TOGGLE_FLAG] = &server_toggle_flag;
  command_handlers[COMMAND_QUIT_GAME] = &server_quit_game;

  // Initialize the command and response string structures.
  Data_string* command_string = (Data_string*)malloc(sizeof(Data_string));
  command_string->data = (unsigned char*)calloc(COMMAND_BUFFER_LENGTH, sizeof(unsigned char));
  Data_string* response_string = (Data_string*)malloc(sizeof(Data_string));
  response_string->data = (unsigned char*)calloc(RESPONSE_BUFFER_LENGTH, sizeof(unsigned char));

  // Stay in the main loop until a shut down command is explicitly received and the game has been successfully shut
  // down.
  bool shut_down_requested = false;
  while(!shut_down_requested)
  {
    // Get input from the client.
    command_string->length = 0;
    receive_command(command_string);

    // TODO: Debug.
    fputs("Server received binary command: ", stdout);
    print_bits(command_string->data, command_string->length);
    fputc('\n', stdout);

    // Reset response string.
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
      unsigned char command_code = *command_string->data;

      // Make sure the command code represents a valid command.
      if(command_code > (sizeof(command_handlers)/sizeof(void(*)(Data_string*, Data_string*))) - 1)
      {
        *response_string->data = COMMAND_CODE_NOT_VALID;
        response_string->length++;
      }
      else
      {
        // Call the command handler function.  It will update the response string internally after calling the command.
        (*command_handlers[command_code])(command_string, response_string);

        // If we've received a shut-down command and the game quit successfully then we're ready to exit the program.
        if(command_code == COMMAND_SHUT_DOWN && *response_string->data == SHUT_DOWN_NO_ERROR) shut_down_requested =
                                                                                              true;
      }
    }

    // TODO: Debug.
    fputs("Server sending binary response: ", stdout);
    print_bits(response_string->data, response_string->length);
    fputc('\n', stdout);

    // Send the response string back to the client interface.
    send_response(response_string);
  }
  fputs("Goodbye!", stdout);

  // Delete the command and response string structures.
  free(command_string->data);
  free(command_string);
  free(response_string->data);
  free(response_string);
  return SHUT_DOWN_NO_ERROR;
}

