#include "constants.h"

const unsigned char BITS_ADJACENT = 0x0fu;  // 00001111
const unsigned char BIT_MINED = 0x10u;      // 00010000
const unsigned char BITS_SENSITIVE = 0x1fu; // 00011111
const unsigned char BIT_FLAGGED = 0x20u;    // 00100000
const unsigned char BIT_REVEALED = 0x40u;   // 01000000
const unsigned char BIT_UNUSED = 0x80u;     // 10000000

// Parameter constraints for new game
const unsigned char NEW_GAME_MIN_HEIGHT = 9;
const unsigned char NEW_GAME_MIN_WIDTH = 9;
const unsigned char NEW_GAME_MAX_HEIGHT = 24;
const unsigned char NEW_GAME_MAX_WIDTH = 30;
const unsigned char NEW_GAME_MIN_MINES = 10;

// Game status codes
const unsigned char GAME_STATUS_NOT_IN_PROGRESS = 0;
const unsigned char GAME_STATUS_IN_PROGRESS = 2;
const unsigned char GAME_STATUS_LOST = 3;
const unsigned char GAME_STATUS_WON = 4;

// Error codes for general program execution
const unsigned char GENERAL_NO_ERROR = 0;

// Error codes for 'start_game' calls
const unsigned char START_GAME_ALREADY_IN_PROGRESS = 21;
const unsigned char START_GAME_HEIGHT_TOO_SMALL = 22;
const unsigned char START_GAME_HEIGHT_TOO_LARGE = 23;
const unsigned char START_GAME_WIDTH_TOO_SMALL = 24;
const unsigned char START_GAME_WIDTH_TOO_LARGE = 25;
const unsigned char START_GAME_NOT_ENOUGH_MINES = 26;
const unsigned char START_GAME_TOO_MANY_MINES = 27;
const unsigned char START_GAME_UNEXPECTED_ERROR = 29;

// Error codes for 'sync_game' calls
const unsigned char SYNC_GAME_NOT_IN_PROGRESS = 31;
const unsigned char SYNC_GAME_UNEXPECTED_ERROR = 39;

// Error codes for 'reveal' calls
const unsigned char REVEAL_GAME_NOT_IN_PROGRESS = 41;
const unsigned char REVEAL_GAME_ALREADY_FINISHED = 42;
const unsigned char REVEAL_X_COORDINATE_TOO_LOW = 43;
const unsigned char REVEAL_X_COORDINATE_TOO_HIGH = 44;
const unsigned char REVEAL_Y_COORDINATE_TOO_LOW = 45;
const unsigned char REVEAL_Y_COORDINATE_TOO_HIGH = 46;
const unsigned char REVEAL_MUST_BE_UNREVEALED = 47;
const unsigned char REVEAL_MUST_UNFLAG_FIRST = 48;
const unsigned char REVEAL_UNEXPECTED_ERROR = 49;

// Error codes for 'toggle_flag' calls
const unsigned char TOGGLE_FLAG_GAME_NOT_IN_PROGRESS = 51;
const unsigned char TOGGLE_FLAG_GAME_ALREADY_FINISHED = 52;
const unsigned char TOGGLE_FLAG_X_COORDINATE_TOO_LOW = 53;
const unsigned char TOGGLE_FLAG_X_COORDINATE_TOO_HIGH = 54;
const unsigned char TOGGLE_FLAG_Y_COORDINATE_TOO_LOW = 55;
const unsigned char TOGGLE_FLAG_Y_COORDINATE_TOO_HIGH = 56;
const unsigned char TOGGLE_FLAG_MUST_BE_UNREVEALED = 57;
const unsigned char TOGGLE_FLAG_UNEXPECTED_ERROR = 59;

// Error codes for 'quit' calls
const unsigned char QUIT_GAME_NOT_IN_PROGRESS = 61;
const unsigned char QUIT_UNEXPECTED_ERROR = 69;

