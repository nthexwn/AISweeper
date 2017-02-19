#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "server_interface.h"
#include "utilities.h"

// TODO: Enable human friendly input.
Data_string obtain_command()
{
  // Prompt
  printf("Command: ");

  // Capture data as characters.
  char* input_buffer = NULL;
  size_t buffer_length = MAXIMUM_POSSIBLE_COMMAND_LENGTH;
  unsigned short command_length = getline(&input_buffer, &buffer_length, stdin);

  // Convert characters to raw numeric values.
  for(unsigned char* input_buffer_index = (unsigned char*)input_buffer; input_buffer_index <
      (unsigned char*)input_buffer + MAXIMUM_POSSIBLE_COMMAND_LENGTH; input_buffer_index++)
  {
    *input_buffer_index -= DATA_TO_CHARACTER_OFFSET;
  }

  // Return prepared data string.
  Data_string command_string = (Data_string){command_length, (unsigned char*)input_buffer};
  return command_string;
}

// TODO: Enable human friendly output.
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

