#ifndef INTERFACE_H
#define INTERFACE_H

#include "constants.h"

// Loops through input from stdin until a valid command is entered.  Commands will start with a string which is checked
// against a trie of known commands.  The final character node of each valid command in the trie will list the expected
// argument types for that command.  The remainder of the input string will then be parsed for data to match those
// arguments.  Each argument in the string will be separated by a space character.  Receiving an invalid command string
// will prompt the user to try again.  Invalid commands will not be sent back to the main server layer.  This is the
// same mechanism that will be used in the networked version of this program in order to prevent known bad commands
// from being sent by the client.  Valid commands will be serialized into raw bytes in big endian order and handed off
// to the main server layer.  The server layer will then perform its own set of validation on the serialized command
// data.
void obtain_command(Data_string* command_string);

// Deserializes the response string and translates it back into a human readable format which is then sent to stdout.
// In the networked version of this program the serialized response will be transmitted to a remote client which will
// handle the deserialization and display on its own.
void handle_response(Data_string* response_string);

#endif // SERVER_INTERFACE_H

