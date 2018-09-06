#include "common_enums.h"

/*
  // Make sure the response string isn't empty.
  if(response_string->length == 0)
  {
    printf("Client error: %s\n", response_code_strings[COMMAND_NO_INPUT]);
    return;
  }

  // If the response indicates that there was an error processing the command then print the error message and return.
  Response_code response_code = *response_string->data;
  if(response_code % RESPONSE_CODE_GROUP_WIDTH != 0)
  {
    printf("Client error: %s\n", response_code_strings[response_code]);
    return;
  }

  // Determine the type of response.
  if(response_code / RESPONSE_CODE_GROUP_WIDTH > SUPPORTED_COMMAND_COUNT)
  {
    printf("Client error: %s\n", response_code_strings[RESPONSE_CODE_NOT_VALID]);
    return;
  }
  Command_type command_type = response_code / RESPONSE_CODE_GROUP_WIDTH;

  switch(command_type)
  {
    case COMMAND_SHUT_DOWN:
      server_quit_game(command_string, response_string);
      break;
    case COMMAND_START_GAME:
      server_start_game(command_string, response_string);
      break;
    case COMMAND_SYNC_GAME:
      deserialize_game_info(command_string, response_string);
      break;
    case COMMAND_REVEAL:
      deserialize_action_info(command_string, response_string);
      break;
    case COMMAND_TOGGLE_FLAG:
      deserialize_action_info(command_string, response_string);
      break;
    case COMMAND_QUIT_GAME:
      deserialize_action_info(command_string, response_string);
      break;
  }
*/

int main()
{
  // TODO
  return 0;
}