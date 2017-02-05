#ifndef CONSTANTS_H
#define CONSTANTS_H

// Bit flags for position scheme
extern const unsigned char BIT_ADJACENTS;
extern const unsigned char BIT_MINED;
extern const unsigned char BIT_FLAGGED;
extern const unsigned char BIT_REVEALED;
extern const unsigned char BIT_UNUSED;

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
extern const unsigned char START_GAME_HEIGHT_TOO_SMALL;
extern const unsigned char START_GAME_HEIGHT_TOO_LARGE;
extern const unsigned char START_GAME_WIDTH_TOO_SMALL;
extern const unsigned char START_GAME_WIDTH_TOO_LARGE;
extern const unsigned char START_GAME_NOT_ENOUGH_MINES;
extern const unsigned char START_GAME_TOO_MANY_MINES;
extern const unsigned char START_GAME_UNEXPECTED_ERROR;

// Error codes for 'reveal' calls
extern const unsigned char REVEAL_GAME_NOT_IN_PROGRESS;
extern const unsigned char REVEAL_GAME_ALREADY_FINISHED;
extern const unsigned char REVEAL_X_COORDINATE_TOO_LOW;
extern const unsigned char REVEAL_X_COORDINATE_TOO_HIGH;
extern const unsigned char REVEAL_Y_COORDINATE_TOO_LOW;
extern const unsigned char REVEAL_Y_COORDINATE_TOO_HIGH;
extern const unsigned char REVEAL_ALREADY_REVEALED;
extern const unsigned char REVEAL_MUST_UNFLAG_FIRST;
extern const unsigned char REVEAL_UNEXPECTED_ERROR;

// Error codes for 'flag' calls
extern const unsigned char FLAG_GAME_NOT_IN_PROGRESS;
extern const unsigned char FLAG_GAME_ALREADY_FINISHED;
extern const unsigned char FLAG_X_COORDINATE_TOO_LOW;
extern const unsigned char FLAG_X_COORDINATE_TOO_HIGH;
extern const unsigned char FLAG_Y_COORDINATE_TOO_LOW;
extern const unsigned char FLAG_Y_COORDINATE_TOO_HIGH;
extern const unsigned char FLAG_REVEALED_CANNOT_FLAG;
extern const unsigned char FLAG_ALREADY_FLAGGED;
extern const unsigned char FLAG_UNEXPECTED_ERROR;

// Error codes for 'unflag' calls
extern const unsigned char UNFLAG_GAME_NOT_IN_PROGRESS;
extern const unsigned char UNFLAG_GAME_ALREADY_FINISHED;
extern const unsigned char UNFLAG_X_COORDINATE_TOO_LOW;
extern const unsigned char UNFLAG_X_COORDINATE_TOO_HIGH;
extern const unsigned char UNFLAG_Y_COORDINATE_TOO_LOW;
extern const unsigned char UNFLAG_Y_COORDINATE_TOO_HIGH;
extern const unsigned char UNFLAG_REVEALED_CANNOT_UNFLAG;
extern const unsigned char UNFLAG_NOT_FLAGGED;
extern const unsigned char UNFLAG_UNEXPECTED_ERROR;

// Error codes for 'quit' calls
extern const unsigned char QUIT_GAME_NOT_IN_PROGRESS;
extern const unsigned char QUIT_UNEXPECTED_ERROR;

#endif // CONSTANTS_H

