#ifndef COMMON_CONSTANTS_H
#define COMMON_CONSTANTS_H

// Buffer lengths.
extern const unsigned short COMMAND_BUFFER_LENGTH;
extern const unsigned short RESPONSE_BUFFER_LENGTH;

// Command lengths and offsets.
extern const unsigned char COMMAND_SHUT_DOWN_REQUIRED_LENGTH;
extern const unsigned char COMMAND_START_GAME_REQUIRED_LENGTH;
extern const unsigned char COMMAND_START_GAME_HEIGHT_OFFSET;
extern const unsigned char COMMAND_START_GAME_WIDTH_OFFSET;
extern const unsigned char COMMAND_START_GAME_MINES_OFFSET;
extern const unsigned char COMMAND_SYNC_GAME_REQUIRED_LENGTH;
extern const unsigned char COMMAND_REVEAL_POSITION_REQUIRED_LENGTH;
extern const unsigned char COMMAND_REVEAL_POSITION_X_OFFSET;
extern const unsigned char COMMAND_REVEAL_POSITION_Y_OFFSET;
extern const unsigned char COMMAND_TOGGLE_FLAG_REQUIRED_LENGTH;
extern const unsigned char COMMAND_TOGGLE_FLAG_X_OFFSET;
extern const unsigned char COMMAND_TOGGLE_FLAG_Y_OFFSET:
extern const unsigned char COMMAND_QUIT_GAME_REQUIRED_LENGTH;

// Response lengths and offsets.
extern const unsigned char RESPONSE_SHUT_DOWN_REQUIRED_LENGTH;
extern const unsigned char RESPONSE_START_GAME_REQUIRED_LENGTH;
extern const unsigned char RESPONSE_SYNC_GAME_REQUIRED_LENGTH;
extern const unsigned char RESPONSE_SYNC_GAME_GAME_STATUS_OFFSET;
extern const unsigned char RESPONSE_SYNC_GAME_HEIGHT_OFFSET;
extern const unsigned char RESPONSE_SYNC_GAME_WIDTH_OFFSET;
extern const unsigned char RESPONSE_SYNC_GAME_MINES_OFFSET;
extern const unsigned char RESPONSE_SYNC_GAME_FIELD_COPY_OFFSET;
extern const unsigned char RESPONSE_REVEAL_POSITION_REQUIRED_LENGTH;
extern const unsigned char RESPONSE_REVEAL_POSITION_GAME_STATUS_OFFSET;
extern const unsigned char RESPONSE_REVEAL_POSITION_MBLA_HEAD_OFFSET;
extern const unsigned char RESPONSE_TOGGLE_FLAG_REQUIRED_LENGTH;
extern const unsigned char RESPONSE_QUIT_GAME_REQUIRED_LENGTH;

// Copy node length and offsets.
extern const unsigned char COPY_NODE_REQUIRED_LENGTH;
extern const unsigned char COPY_NODE_X_OFFSET;
extern const unsigned char COPY_NODE_Y_OFFSET;
extern const unsigned char COPY_NODE_POSITION_OFFSET;

// Bit flags for position scheme.
extern const unsigned char BITS_ADJACENT;
extern const unsigned char BIT_MINED;
extern const unsigned char BITS_SENSITIVE;
extern const unsigned char BIT_FLAGGED;
extern const unsigned char BIT_REVEALED;
extern const unsigned char BIT_UNUSED;

// Parameter constraints for new game.
extern const unsigned char NEW_GAME_MIN_HEIGHT;
extern const unsigned char NEW_GAME_MIN_WIDTH;
extern const unsigned char NEW_GAME_MAX_HEIGHT;
extern const unsigned char NEW_GAME_MAX_WIDTH;
extern const unsigned char NEW_GAME_MIN_MINES;

// Miscellaneous.
extern const unsigned char BITS_PER_CHAR;
extern const unsigned char CHAR_SPACE;
extern const unsigned char CONTROL_CHARACTER_RANGE;
extern const unsigned char DATA_TO_CHARACTER_OFFSET;
extern const unsigned char SUPPORTED_COMMAND_COUNT;
extern const unsigned char RESPONSE_CODE_GROUP_WIDTH;
extern const unsigned char RESPONSE_CODE_OUT_OF_MEMORY_INDICATOR;

// Response code strings.
extern const char* response_code_strings[90];

// Game status strings.
extern const char* game_status_strings[5];

#endif // COMMON_CONSTANTS_H

