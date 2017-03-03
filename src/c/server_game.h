#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "common_structs.h"

// TODO: Comments describing these functions.

Data_string* server_start_game(Data_string* command_string);
Data_string* server_sync_game(Data_string* command_string);
Data_string* server_reveal_position(Data_string* command_string);
Data_string* server_toggle_flag(Data_string* command_string);
Data_string* server_quit_game(Data_string* command_string);

#endif // SERVER_GAME_H

