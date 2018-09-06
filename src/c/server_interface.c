#include <stdbool.h>
#include <stdio.h>
#include "server_interface.h"
#include "common_constants.h"

void receive_command(Data_string* command_string)
{
  fputs("Enter a command using decimal numbers: ", stdout);
  bool command_received = false;
  unsigned char* command_string_index = command_string->data;
  while(!command_received && command_string->length < COMMAND_BUFFER_LENGTH)
  {
    unsigned char next_char = (unsigned char)fgetc(stdin);
    if(next_char == '\n')
    {
      command_received = true;
    }
    else if(next_char > CONTROL_CHARACTER_RANGE)
    {
      *command_string_index = next_char - DATA_TO_CHARACTER_OFFSET;
      command_string_index += sizeof(unsigned char);
      command_string->length++;
    }
  }
}

void send_response(Data_string* response_string)
{
  // TODO
}

