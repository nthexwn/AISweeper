#include "constants.h"

const unsigned char BIT_ADJACENTS = 0x0fu;
const unsigned char BIT_CHECKED = 0x10u;
const unsigned char BIT_MINED = 0x20u;
const unsigned char BIT_FLAGGED = 0x40u;
const unsigned char BIT_REVEALED = 0x80u;

// Parameter constraints for new game
const unsigned char NEW_GAME_MIN_HEIGHT = 9;
const unsigned char NEW_GAME_MIN_WIDTH = 9;
const unsigned char NEW_GAME_MAX_HEIGHT = 24;
const unsigned char NEW_GAME_MAX_WIDTH = 30;
const unsigned char NEW_GAME_MIN_MINES = 10;

// Error codes for general program execution
const unsigned char GENERAL_NO_ERROR = 0;

// Error codes for 'start_game' calls
const unsigned char START_GAME_HEIGHT_TOO_SMALL = 21;
const unsigned char START_GAME_HEIGHT_TOO_LARGE = 22;
const unsigned char START_GAME_WIDTH_TOO_SMALL = 23;
const unsigned char START_GAME_WIDTH_TOO_LARGE = 24;
const unsigned char START_GAME_NOT_ENOUGH_MINES = 25;
const unsigned char START_GAME_TOO_MANY_MINES = 26;
const unsigned char START_GAME_UNEXPECTED_ERROR = 27;

// Error codes for 'reveal' calls
const unsigned char REVEAL_GAME_NOT_IN_PROGRESS = 31;
const unsigned char REVEAL_GAME_ALREADY_FINISHED = 32;
const unsigned char REVEAL_X_COORDINATE_TOO_LOW = 33;
const unsigned char REVEAL_X_COORDINATE_TOO_HIGH = 34;
const unsigned char REVEAL_Y_COORDINATE_TOO_LOW = 35;
const unsigned char REVEAL_Y_COORDINATE_TOO_HIGH = 36;
const unsigned char REVEAL_ALREADY_REVEALED = 37;
const unsigned char REVEAL_MUST_UNFLAG_FIRST = 38;
const unsigned char REVEAL_UNEXPECTED_ERROR = 39;

// Error codes for 'flag' calls
const unsigned char FLAG_GAME_NOT_IN_PROGRESS = 41;
const unsigned char FLAG_GAME_ALREADY_FINISHED = 42;
const unsigned char FLAG_X_COORDINATE_TOO_LOW = 43;
const unsigned char FLAG_X_COORDINATE_TOO_HIGH = 44;
const unsigned char FLAG_Y_COORDINATE_TOO_LOW = 45;
const unsigned char FLAG_Y_COORDINATE_TOO_HIGH = 45;
const unsigned char FLAG_ALREADY_REVEALED = 46;
const unsigned char FLAG_ALREADY_FLAGGED = 47;

// Error codes for 'unflag' calls
const unsigned char UNFLAG_GAME_NOT_IN_PROGRESS = 51;
const unsigned char UNFLAG_GAME_ALREADY_FINISHED = 52;
const unsigned char UNFLAG_X_COORDINATE_TOO_LOW = 53;
const unsigned char UNFLAG_X_COORDINATE_TOO_HIGH = 54;
const unsigned char UNFLAG_Y_COORDINATE_TOO_LOW = 55;
const unsigned char UNFLAG_Y_COORDINATE_TOO_HIGH = 56;
const unsigned char UNFLAG_REVEALED_CANNOT_FLAG = 57;
const unsigned char UNFLAG_NOT_FLAGGED = 58;
const unsigned char UNFLAG_UNEXPECTED_ERROR = 59;

// Error codes for 'quit' calls
const unsigned char QUIT_GAME_NOT_IN_PROGRESS = 61;
const unsigned char QUIT_UNEXPECTED_ERROR = 62;

