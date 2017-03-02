#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "client_game.h"
#include "constants.h"
#include "interface.h"
#include "utilities.h"

static void deserialize_action_info(Action_info* action_info, Data_string* response_string)
{
  // Error handling.
  if(response_string->length < ACTION_INFO_MBLA_HEAD_OFFSET)
  {
    // If there's insufficient data present to generate an action info structure then we'll abort now.
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }

  // We now know there's at least enough information present to extract the mbla list count.
  unsigned short mbla_count = 0;
  transfer_value(response_string->data + ACTION_INFO_MBLA_COUNT_OFFSET, ENDIAN_BIG, (unsigned char*)&mbla_count,
                 machine_endian(), sizeof(unsigned short));

  // We'll use the transfered mbla count to determine if the remaining length of the response string matches the
  // length indicated by the mbla count.  If it does not then we'll abort instead of attempting to read or write
  // outside of the expected bounds.

  if(response_string->length < ACTION_INFO_MBLA_HEAD_OFFSET + mbla_count)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  if(response_string->length > ACTION_INFO_MBLA_HEAD_OFFSET + mbla_count * POSITION_DATA_SIZE)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_EXCESSIVE_ARGUMENT_DATA]);
    return;
  }

  // Begin deserialization.
  action_info = (Action_info*)malloc(sizeof(Action_info));
  action_info->error_type = *response_string->data + ACTION_INFO_ERROR_TYPE_OFFSET;
  action_info->game_status = *(response_string->data + ACTION_INFO_GAME_STATUS_OFFSET);
  transfer_value(response_string->data + ACTION_INFO_MINES_NOT_FLAGGED_OFFSET, ENDIAN_BIG,
                 (unsigned char*)&action_info->mines_not_flagged, machine_endian(), sizeof(signed short));

  // Generate modified by last action list.  May simply be null.
  action_info->mbla_head = (Copy_node*)malloc(sizeof(Copy_node));
  action_info->mbla_head->next = NULL;
  Copy_node* mbla_index = action_info->mbla_head;
  for(unsigned char* index = response_string->data + ACTION_INFO_MBLA_HEAD_OFFSET; index < response_string->data +
      ACTION_INFO_MBLA_HEAD_OFFSET + mbla_count * POSITION_DATA_SIZE; index += POSITION_DATA_SIZE)
  {
    Copy_node* mbla_new = (Copy_node*)malloc(sizeof(Copy_node));
    mbla_new->x = *(index + COPY_NODE_X_OFFSET);
    mbla_new->y = *(index + COPY_NODE_Y_OFFSET);
    mbla_new->position = *(index + COPY_NODE_POSITION_OFFSET);
    mbla_new->next = NULL;
    mbla_index->next = mbla_new;
    mbla_index = mbla_index->next;
  }
  Copy_node* mbla_free = action_info->mbla_head;
  action_info->mbla_head = action_info->mbla_head->next;
  free(mbla_free);

  // Send deserialized action info to client.
  client_action_update(action_info);
}

static void deserialize_game_info(Game_info* game_info, Data_string* response_string)
{
  // Error handling.
  if(response_string->length < GAME_INFO_COPY_FIELD_BEGIN_OFFSET)
  {
    // If there's insufficient data present to generate a game info structure then we'll abort now.
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }

  // We now know there's at least enough information present to extract the height and width of the playing field.
  unsigned char height = *(response_string->data + GAME_INFO_HEIGHT_OFFSET);
  unsigned char width = *(response_string->data + GAME_INFO_WIDTH_OFFSET);
  unsigned short field_length = height * width;

  // We'll use the calculated field_length to determine if the provided copy of the field data matches the expected
  // length.  For this to be true there must be an exact match of the length or no data provided at all (this is valid
  // in some cases).  If it does not then we'll abort instead of attempting to read or write outside of the expected
  // bounds.
  if(response_string->length > GAME_INFO_COPY_FIELD_BEGIN_OFFSET && response_string->length <
                                                                  GAME_INFO_COPY_FIELD_BEGIN_OFFSET + field_length)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_INSUFFICIENT_ARGUMENT_DATA]);
    return;
  }
  if(response_string->length > GAME_INFO_COPY_FIELD_BEGIN_OFFSET + field_length)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_EXCESSIVE_ARGUMENT_DATA]);
    return;
  }

  // Begin deserialization.
  game_info = (Game_info*)malloc(sizeof(Game_info));
  game_info->error_type = *response_string->data + GAME_INFO_ERROR_TYPE_OFFSET;
  game_info->game_status = *(response_string->data + GAME_INFO_GAME_STATUS_OFFSET);
  signed short mines_not_flagged = 0;
  transfer_value(response_string->data + GAME_INFO_MINES_NOT_FLAGGED_OFFSET, ENDIAN_BIG,
                 (unsigned char*)&mines_not_flagged, machine_endian(), sizeof(signed short));
  game_info->height = height;
  game_info->width = width;
  if(response_string->length == GAME_INFO_COPY_FIELD_BEGIN_OFFSET)
  {
    // No copy of the playing field was provided, so we'll initialize it ourselves and fill it with empty data.  This
    // is done when a new game was just started and we want the client to initialize on its own instead of wasting
    // bandwidth by transmitting a bunch of zeros.
    game_info->copy_field_begin = (unsigned char*)calloc(field_length, sizeof(unsigned char));
  }
  else
  {
    // A copy of the playing field was provided, so we'll initialize space for it prior to copying the data over.  Note
    // that we don't need to zero initialize the data (calloc) here because the data is guaranteed to be overwritten
    // from the copied playing field.
    game_info->copy_field_begin = (unsigned char*)malloc(field_length);
    for(unsigned char index = 0; index < field_length; index++)
    {
      *(game_info->copy_field_begin + index * sizeof(unsigned char)) = *(response_string->data +
      GAME_INFO_COPY_FIELD_BEGIN_OFFSET + index * sizeof(unsigned char));
    }
  }

  // Send deserialized game info to client.
  client_game_update(game_info);
}

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
  printf("\n");
}

void handle_response(Data_string* response_string)
{
  // TODO: Debug.
  printf("Client received binary response: ");
  print_bits(response_string->data, response_string->length);
  printf("\n");

  // Make sure the response string isn't empty.
  if(response_string->length == 0)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_NO_INPUT]);
    return;
  }

  // If the response indicates that there was an error processing the command then print the error message and return.
  Error_type error_type = *response_string->data;
  if(error_type % ERROR_GROUP_WIDTH != 0)
  {
    printf("Client error: %s\n", error_messages[error_type]);
    return;
  }

  // Determine the type of response.
  if(error_type / ERROR_GROUP_WIDTH - ERROR_COMMAND_GROUP_OFFSET > NUMBER_OF_SUPPORTED_COMMANDS - 1)
  {
    printf("Client error: %s\n", error_messages[RESPONSE_CODE_NOT_VALID]);
    return;
  }
  Command_type command_type = error_type / ERROR_GROUP_WIDTH - ERROR_COMMAND_GROUP_OFFSET;

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

