#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

unsigned char* obtain_command();
void handle_response(unsigned char* response_string);

#endif // SERVER_INTERFACE_H

