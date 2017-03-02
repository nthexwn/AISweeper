#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include "common_structs.h"

// Update the client's model of the game state based on the last action.  Finishes with a call to client_render in
// order to display the updated game state.
void client_action_update(Action_info* action_info);

// Update the client's model of the game state based on new game or sync requests.  Finishes with a call to
// client_render in order to display the updated game state.
void client_game_update(Game_info* game_info);

#endif // CLIENT_GAME_H

