#ifndef SERVER_GAME_H
#define SERVER_GAME_H

#include "common_structs.h"

void server_shut_down(Data_string* command_string, Data_string* response_string);
void server_start_game(Data_string* command_string, Data_string* response_string);
void server_sync_game(Data_string* command_string, Data_string* response_string);
void server_reveal_position(Data_string* command_string, Data_string* response_string);
void server_toggle_flag(Data_string* command_string, Data_string* response_string);
void server_quit_game(Data_string* command_string, Data_string* response_string);

#endif // SERVER_GAME_H

