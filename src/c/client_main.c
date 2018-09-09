#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_interface.h"
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"
#include "client_game.h"
#include "common_structs.h"

void obtain_command(Data_string* command_string)
{
  printf("Command: ");
  unsigned char* command_string_index = command_string->data;
  unsigned char next_character = CHAR_SPACE;
  while(command_string->length < COMMAND_BUFFER_LENGTH)
  {
    next_character = (unsigned char)fgetc(stdin);
    if(next_character == '\n')
    {
      break;
    }
    *command_string_index = next_character;
    command_string_index += sizeof(unsigned char);
    command_string->length += sizeof(unsigned char);
  }
}

int main()
{
  // Initialize command handlers.
  void (*response_handlers[SUPPORTED_COMMAND_COUNT])(Data_string*, Data_string*);
  response_handlers[COMMAND_START_GAME] = &client_start_game;
  response_handlers[COMMAND_SYNC_GAME] = &client_sync_game;
  response_handlers[COMMAND_REVEAL_POSITION] = &client_reveal_position;
  response_handlers[COMMAND_TOGGLE_FLAG] = &client_toggle_flag;
  response_handlers[COMMAND_QUIT_GAME] = &client_quit_game;
  response_handlers[COMMAND_DISCONNECT_CLIENT] = &client_disconnect_client;
  response_handlers[COMMAND_SHUT_DOWN] = &client_shut_down;
  
  // Initialize the command and response string structures.
  Data_string* command_string = (Data_string*)malloc(sizeof(Data_string));
  command_string->data = (unsigned char*)calloc(COMMAND_BUFFER_LENGTH, sizeof(unsigned char));
  Data_string* response_string = (Data_string*)malloc(sizeof(Data_string));
  response_string->data = (unsigned char*)calloc(RESPONSE_BUFFER_LENGTH, sizeof(unsigned char));

  bool exit_requested = false;
  while(!exit_requested)
  {
    // Obtain a new command from the user.
    command_string->length = 0;
    obtain_command(command_string);
    if(command_string->length > 0 && (*command_string->data == COMMAND_DISCONNECT_CLIENT ||
                                      *command_string->data == COMMAND_SHUT_DOWN))
    {
      exit_requested = true;
    }

    // Send command to the server.
    fputs("Sending binary command to server: ", stdout);
    print_bits(command_string->data, command_string->length);
    fputc('\n', stdout);
    send_command(command_string);

    // Receive response from the server.
    response_string->length = 0;
    receive_response(response_string);
    fputs("Client received binary response: ", stdout);
    print_bits(response_string->data, response_string->length);
    fputc('\n', stdout);

    // Make sure the response string isn't empty.
    if(response_string->length == 0)
    {
      fputs("Server response was empty!\n", stdout);
    }
    else
    {
      // Retrieve the command code from the response string.
      unsigned char response_code = *response_string->data;
      if(response_code > RESPONSE_CODE_COUNT)
      {
        fprintf(stdout, "Server response code (%d) is outside of supported range!\n", response_code);
      }
      else if(response_code % RESPONSE_CODE_GROUP_WIDTH != 0)
      {
        fprintf(stdout, "Received error code (%d) from server: %s\n", response_code,
                response_code_strings[response_code]);
      }
      else
      {
        unsigned char response_command_group = (response_code - response_code % RESPONSE_CODE_GROUP_WIDTH) /
                                              RESPONSE_CODE_GROUP_WIDTH;
        if(response_command_group > (sizeof(response_handlers)/sizeof(void(*)(Data_string*, Data_string*))) - 1)
        {
          fprintf(stdout, "Server response command group (%d) does not correspond with any known commands!\n",
                  response_command_group);
        }
        else if(command_string->length > 0 && response_command_group != *command_string->data)
        {
          fprintf(stdout, "Server response command group (%d) does not correspond with original command code (%d)!\n",
                  response_command_group, *command_string->data);
        }
        else
        {
          // Call the response handler function.
          (*response_handlers[response_code])(command_string, response_string);
        }
      }
    }
  }

  // Delete the command and response string structures before exiting the program.
  free(command_string->data);
  free(command_string);
  free(response_string->data);
  free(response_string);
  fputs("Goodbye!", stdout);
  return GENERAL_NO_ERROR;
}