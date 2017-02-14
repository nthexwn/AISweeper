#ifndef CONSTANTS_H
#define CONSTANTS_H

// Bit flags for position scheme
extern const unsigned char BITS_ADJACENT;  // 00001111
extern const unsigned char BIT_MINED;      // 00010000
extern const unsigned char BITS_SENSITIVE; // 00011111
extern const unsigned char BIT_FLAGGED;    // 00100000
extern const unsigned char BIT_REVEALED;   // 01000000
extern const unsigned char BIT_UNUSED;     // 10000000

// Parameter extern constraints for new game
extern const unsigned char NEW_GAME_MIN_HEIGHT;
extern const unsigned char NEW_GAME_MIN_WIDTH;
extern const unsigned char NEW_GAME_MAX_HEIGHT;
extern const unsigned char NEW_GAME_MAX_WIDTH;
extern const unsigned char NEW_GAME_MIN_MINES;

// Game status codes
extern const unsigned char GAME_STATUS_NOT_IN_PROGRESS;
extern const unsigned char GAME_STATUS_IN_PROGRESS;
extern const unsigned char GAME_STATUS_LOST;
extern const unsigned char GAME_STATUS_WON;

// Error codes for general program execution
extern const unsigned char GENERAL_NO_ERROR;

// Error codes for 'start_game' calls
extern const unsigned char START_GAME_ALREADY_IN_PROGRESS;
extern const unsigned char START_GAME_HEIGHT_TOO_SMALL;
extern const unsigned char START_GAME_HEIGHT_TOO_LARGE;
extern const unsigned char START_GAME_WIDTH_TOO_SMALL;
extern const unsigned char START_GAME_WIDTH_TOO_LARGE;
extern const unsigned char START_GAME_NOT_ENOUGH_MINES;
extern const unsigned char START_GAME_TOO_MANY_MINES;
extern const unsigned char START_GAME_UNEXPECTED_ERROR;

// Error codes for 'sync_game' calls
extern const unsigned char SYNC_GAME_NOT_IN_PROGRESS;
extern const unsigned char SYNC_GAME_UNEXPECTED_ERROR;

// Error codes for 'reveal' calls
extern const unsigned char REVEAL_GAME_NOT_IN_PROGRESS;
extern const unsigned char REVEAL_GAME_ALREADY_FINISHED;
extern const unsigned char REVEAL_X_COORDINATE_TOO_LOW;
extern const unsigned char REVEAL_X_COORDINATE_TOO_HIGH;
extern const unsigned char REVEAL_Y_COORDINATE_TOO_LOW;
extern const unsigned char REVEAL_Y_COORDINATE_TOO_HIGH;
extern const unsigned char REVEAL_MUST_BE_UNREVEALED;
extern const unsigned char REVEAL_MUST_UNFLAG_FIRST;
extern const unsigned char REVEAL_UNEXPECTED_ERROR;

// Error codes for 'toggle_flag' calls
extern const unsigned char TOGGLE_FLAG_GAME_NOT_IN_PROGRESS;
extern const unsigned char TOGGLE_FLAG_GAME_ALREADY_FINISHED;
extern const unsigned char TOGGLE_FLAG_X_COORDINATE_TOO_LOW;
extern const unsigned char TOGGLE_FLAG_X_COORDINATE_TOO_HIGH;
extern const unsigned char TOGGLE_FLAG_Y_COORDINATE_TOO_LOW;
extern const unsigned char TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH;
extern const unsigned char TOGGLE_FLAG_MUST_BE_UNREVEALED;
extern const unsigned char UNFLAG_UNEXPECTED_ERROR;

// Error codes for 'quit' calls
extern const unsigned char QUIT_GAME_NOT_IN_PROGRESS;
extern const unsigned char QUIT_UNEXPECTED_ERROR;

#endif // CONSTANTS_H

