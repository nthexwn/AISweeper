#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

#include "common_structs.h"

void receive_command(Data_string* command_string);
void send_response(Data_string* response_string);

#endif // SERVER_INTERFACE_H

