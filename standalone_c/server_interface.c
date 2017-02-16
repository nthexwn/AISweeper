#define _GNU_SOURCE

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"
#include "server_interface.h"

unsigned char* obtain_command()
{
  char* input_buffer = NULL;
  size_t buffer_length = MAXIMUM_POSSIBLE_COMMAND_LENGTH;
  getline(&input_buffer, &buffer_length, stdin);
  return (unsigned char*)input_buffer;
}

void handle_response(unsigned char* response_string)
{
  // TODO: Parse the response and print it in a human readable format.
  printf("%s", response_string);
}

