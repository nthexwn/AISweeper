#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#include "common_structs.h"

void client_start_game(Data_string* command_string, Data_string* response_string);
void client_sync_game(Data_string* command_string, Data_string* response_string);
void client_reveal_position(Data_string* command_string, Data_string* response_string);
void client_toggle_flag(Data_string* command_string, Data_string* response_string);
void client_quit_game(Data_string* command_string, Data_string* response_string);
void client_disconnect_client(Data_string* command_string, Data_string* response_string);
void client_shut_down(Data_string* command_string, Data_string* response_string);
void client_render();

#endif // CLIENT_GAME_H

