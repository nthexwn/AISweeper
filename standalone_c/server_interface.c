#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "server_interface.h"
#include "utilities.h"

Data_string obtain_command()
{
  char* input_buffer = NULL;
  size_t buffer_length = MAXIMUM_POSSIBLE_COMMAND_LENGTH;
  unsigned short command_length = getline(&input_buffer, &buffer_length, stdin);
  Data_string command_string = (Data_string){command_length, (unsigned char*)input_buffer};
  return command_string;
}

void handle_response(Data_string* response_string)
{
  // TODO:  Deserialize the response and print it in a human readable format.  Can remove this temporary raw output
  // loop once that is done.
  for(unsigned char* index = response_string->data; index < response_string->data + response_string->length; index++)
  {
    printf("%u", *index);
  }
}

