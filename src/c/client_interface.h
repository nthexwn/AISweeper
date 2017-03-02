#ifndef CLIENT_INTERFACE_H
#define CLIENT_INTERFACE_H

#include "common_structs.h"

// Grabs up to MAXIMUM_POSSIBLE_COMMAND_LENGTH characters from the user (discarding the rest).  These characters are
// shifted by DATA_TO_CHARACTER_OFFSET as a simple method of allowing the user to enter raw numeric values.  This
// mechanism is mostly used for testing and will be completely replaced when the web-based client is implemented.
void obtain_command(Data_string* command_string);

// Deserializes the response string and translates it back into a human readable format which is then sent to stdout.
// In the networked version of this program the serialized response will be transmitted to a remote client which will
// handle the deserialization and display on its own.
void handle_response(Data_string* response_string);

#endif // CLIENT_INTERFACE_H

