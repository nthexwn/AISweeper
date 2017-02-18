#ifndef SERVER_INTERFACE_H
#define SERVER_INTERFACE_H

// Container for unsigned strings which contain raw data.  Relies on a length variable instead of null termination
// since numerical values are stored directly in these strings instead of being translated through characters.  This
// means that the null character (AKA: "\0", #0, end of string) is frequently used to represent the actual number 0 and
// can't be utilized in this structure.  Without any kind of terminator it's up to the function which is handling one
// of these data strings to never read beyond the provided length of the string.  Likewise, any function which writes
// to one of these strings must accurately update the length variable after doing so.
typedef struct data_string
{
  unsigned short length;
  unsigned char* data;
} Data_string;

// Utilizes the GNU/Linux getline function to read command and argument data from the client into a buffer.  The buffer
// is then cast into a data_string structure and handed back to the main program loop for further analysis/execution.
// The main program loop will erase the buffer after each command/response cycle before calling this function again to
// obtain the next command.
Data_string obtain_command();

// Deserializes the response string and translates it back into a human readable format which is then sent to stdout.
// In the networked version of this program the serialized response will be transmitted to a remote client which will
// handle the deserialization and display on its own.
void handle_response(Data_string* response_string);

#endif // SERVER_INTERFACE_H

