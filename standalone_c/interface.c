#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "interface.h"
#include "utilities.h"

// TODO: Perform user input processing and validation against the detected command type.
void obtain_command(Data_string* command_string)
{
  // Prompt for input.
  printf("Command: ");

  // Continue filling up the buffer until we receive any control character.
  unsigned char* command_string_index = command_string->data;
  unsigned char current_character = CHAR_SPACE;
  while(current_character > CONTROL_CHARACTER_RANGE)
  {
    // Get a character from stdin.
    current_character = (unsigned char)getc(stdin);

    // If the current character is valid (not a control character) and there's still room left in the buffer for more
    // characters then add it to the buffer.  Note that extra characters (beyond the buffer length) will simply be
    // thrown out while we're waiting for the next control character (IE: a new line or carriage return).
    if(current_character > CONTROL_CHARACTER_RANGE && command_string_index < command_string->data +
                                                                             MAXIMUM_POSSIBLE_COMMAND_LENGTH)
    {
      *command_string_index = current_character;
      command_string_index++;
      command_string->length++;
    }
  }

  // Convert characters in buffer to raw numeric values.
  for(command_string_index = command_string->data; command_string_index < command_string->data +
      command_string->length; command_string_index++)
  {
    *command_string_index -= DATA_TO_CHARACTER_OFFSET;
  }

  // Debugging.
  printf("Sending(binary): ");
  print_bits(command_string->data, command_string->length);
}

// TODO: Deserialize the response string into either an action_info or game_info struct and call the appropriate
// handler function on the client.
void handle_response(Data_string* response_string)
{
  // Display raw data output byte by byte.
  printf("Response: ");
  for(unsigned char* index = response_string->data; index < response_string->data + response_string->length; index++)
  {
    printf("%u", *index);
  }
  printf("\n");
}

