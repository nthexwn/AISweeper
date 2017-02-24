#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_game.h"
#include "constants.h"
#include "interface.h"
#include "utilities.h"

static void deserialize_action_info(Action_info* action_info, Data_string* response_string)
{
  // TODO: Debug.
  printf("Client de-serializing binary response_string into action_info: ");
  print_bits(response_string->data, response_string->length);

  // Error handling.
  if(response_string->length < 6)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  unsigned short mbla_count = 0;
  transfer_value(response_string->data + 4, ENDIAN_BIG, (unsigned char*)&mbla_count, machine_endian(),
                 sizeof(unsigned short));
  if(response_string->length < 6 + mbla_count)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  if(response_string->length > 6 + mbla_count * 3)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_EXCESSIVE_ARGUMENT_DATA]);
    return;
  }

  // Begin deserialization.
  action_info = (Action_info*)malloc(sizeof(Action_info));
  action_info->error_type = *response_string->data;
  action_info->game_status = *(response_string->data + 1);
  signed short mines_not_flagged = 0;
  transfer_value(response_string->data + 2, ENDIAN_BIG, (unsigned char*)&mines_not_flagged, machine_endian(),
                 sizeof(signed short));
  Copy_node* mbla_head = (Copy_node*)malloc(sizeof(Copy_node));
  mbla_head->next = NULL;
  Copy_node* mbla_index = mbla_head;
  for(unsigned short index = 0; index < mbla_count * 3; index += 3)
  {
    Copy_node* mbla_new = (Copy_node*)malloc(sizeof(Copy_node));
    mbla_new->x = *(response_string->data + index + 6);
    mbla_new->y = *(response_string->data + index + 7);
    mbla_new->position = *(response_string->data + index + 8);
    mbla_index->next = mbla_new;
    mbla_index = mbla_index->next;
  }
  Copy_node* mbla_free = mbla_head;
  mbla_head = mbla_head->next;
  free(mbla_free);

  // Send deserialized action info to client.
  client_action_update(action_info);
}

static void deserialize_game_info(Game_info* game_info, Data_string* response_string)
{
  // TODO: Debug.
  printf("Client de-serializing binary response_string into game_info: ");
  print_bits(response_string->data, response_string->length);

  // Error handling.
  if(response_string->length < 14)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  unsigned char height = *(response_string->data + 2);
  unsigned char width = *(response_string->data + 3);
  unsigned short field_length = height * width;
  if(response_string->length > 14  && response_string->length < 14 + field_length)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  if(response_string->length > 14 + field_length)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_EXCESSIVE_ARGUMENT_DATA]);
    return;
  }

  // Begin deserialization.
  game_info = (Game_info*)malloc(sizeof(Game_info));
  game_info->error_type = *response_string->data;
  game_info->game_status = *(response_string->data + 1);
  game_info->height = height;
  game_info->width = width;
  signed short mines_not_flagged = 0;
  transfer_value(response_string->data + 4, ENDIAN_BIG, (unsigned char*)&mines_not_flagged, machine_endian(),
                 sizeof(signed short));
  unsigned long seconds_elapsed = 0;
  transfer_value(response_string->data + 6, ENDIAN_BIG, (unsigned char*)&seconds_elapsed, machine_endian(),
                 sizeof(unsigned long));
  game_info->copy_field_begin = (unsigned char*)calloc(field_length, sizeof(unsigned char));
  if(response_string->length == 14) return;
  for(unsigned short index = 0; index < field_length; index++)
  {
    *(game_info->copy_field_begin + index) = *(response_string->data + 14 + index);
  }

  // Send deserialized game info to client.
  client_game_update(game_info);
}

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

  // TODO: Debug.
  printf("Client sending binary command: ");
  print_bits(command_string->data, command_string->length);
}

void handle_response(Data_string* response_string)
{
  // TODO: Debug.
  printf("Client received binary response: ");
  print_bits(response_string->data, response_string->length);

  // Make sure the response string isn't empty.
  if(response_string->length == 0)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_NO_INPUT]);
    return;
  }

  // If the response indicates that there was an error processing the command then print the error message and return.
  Error_type error_type = *response_string->data;
  if(error_type % 10 != 0)
  {
    printf("Client error: %s\n", error_messages[error_type]);
    return;
  }

  // Determine the type of response.
  if(error_type / 10 - 3 > NUMBER_OF_SUPPORTED_COMMANDS - 1)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_CODE_NOT_VALID]);
    return;
  }
  Command_type command_type = error_type / 10 - 3;

  // Deserialize response and update client.
  Action_info* action_info = NULL;
  Game_info* game_info = NULL;
  switch(command_type)
  {
    case COMMAND_SHUT_DOWN:
      deserialize_action_info(action_info, response_string);
      break;
    case COMMAND_START_GAME:
      deserialize_game_info(game_info, response_string);
      break;
    case COMMAND_SYNC_GAME:
      deserialize_game_info(game_info, response_string);
      break;
    case COMMAND_REVEAL:
      deserialize_action_info(action_info, response_string);
      break;
    case COMMAND_TOGGLE_FLAG:
      deserialize_action_info(action_info, response_string);
      break;
    case COMMAND_QUIT_GAME:
      deserialize_action_info(action_info, response_string);
      break;
  }
}

