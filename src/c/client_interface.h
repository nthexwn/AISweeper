#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "common_structs.h"

void send_command(Data_string* command_string);
void receive_response(Data_string* response_string);

#endif // CLIENT_INTERFACE_H

