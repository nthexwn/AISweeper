#ifndef COMMON_CONSTANTS_H
#define COMMON_CONSTANTS_H

// Error messages.
extern const char* error_messages[90];

// Status messages.
extern const char* status_messages[5];

// Action info data offsets.
extern const unsigned char ACTION_INFO_ERROR_TYPE_OFFSET;
extern const unsigned char ACTION_INFO_GAME_STATUS_OFFSET;
extern const unsigned char ACTION_INFO_MINES_NOT_FLAGGED_OFFSET;
extern const unsigned char ACTION_INFO_MBLA_COUNT_OFFSET;
extern const unsigned char ACTION_INFO_MBLA_HEAD_OFFSET;

// Game info data offsets.
extern const unsigned char GAME_INFO_ERROR_TYPE_OFFSET;
extern const unsigned char GAME_INFO_GAME_STATUS_OFFSET;
extern const unsigned char GAME_INFO_MINES_NOT_FLAGGED_OFFSET;
extern const unsigned char GAME_INFO_HEIGHT_OFFSET;
extern const unsigned char GAME_INFO_WIDTH_OFFSET;
extern const unsigned char GAME_INFO_COPY_FIELD_BEGIN_OFFSET;

// Copy node offsets.
extern const unsigned char COPY_NODE_POSITION_OFFSET;
extern const unsigned char COPY_NODE_X_OFFSET;
extern const unsigned char COPY_NODE_Y_OFFSET;

// Argument lengths and offsets.
extern const unsigned char SHUT_DOWN_EXPECTED_LENGTH;
extern const unsigned char START_GAME_EXPECTED_LENGTH;
extern const unsigned char START_GAME_HEIGHT_OFFSET;
extern const unsigned char START_GAME_WIDTH_OFFSET;
extern const unsigned char START_GAME_MINES_OFFSET;
extern const unsigned char SYNC_GAME_EXPECTED_LENGTH;
extern const unsigned char REVEAL_EXPECTED_LENGTH;
extern const unsigned char REVEAL_X_OFFSET;
extern const unsigned char REVEAL_Y_OFFSET;
extern const unsigned char TOGGLE_FLAG_EXPECTED_LENGTH;
extern const unsigned char TOGGLE_FLAG_X_OFFSET;
extern const unsigned char TOGGLE_FLAG_Y_OFFSET;
extern const unsigned char QUIT_GAME_EXPECTED_LENGTH;

// Bit flags for position scheme.
extern const unsigned char BITS_ADJACENT;  // 0b00001111
extern const unsigned char BIT_MINED;      // 0b00010000
extern const unsigned char BITS_SENSITIVE; // 0b00011111
extern const unsigned char BIT_FLAGGED;    // 0b00100000
extern const unsigned char BIT_REVEALED;   // 0b01000000
extern const unsigned char BIT_UNUSED;     // 0b10000000

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
extern const unsigned char ERROR_GROUP_WIDTH;
extern const unsigned char ERROR_COMMAND_GROUP_OFFSET;
extern const unsigned short MAXIMUM_POSSIBLE_COMMAND_LENGTH;
extern const unsigned short MAXIMUM_POSSIBLE_RESPONSE_LENGTH;
extern const unsigned char NUMBER_OF_SUPPORTED_COMMANDS;
extern const unsigned char POSITION_DATA_SIZE;

#endif // COMMON_CONSTANTS_H

