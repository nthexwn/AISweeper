#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "server_interface.h"
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"

static bool server_started = false;
static bool connected_to_client = false;
static bool command_received = false;
static bool response_sent = false;
static bool disconnect_requested = false;
static bool shutdown_requested = false;
static int server_socket_descriptor = 0;
static int client_connection_descriptor = 0;

/*
TODO:
* Convert first nibble of command code byte into byte count indicator for messages.

* Continuously read from recv into a moving pointer into the response buffer until the specified number of bytes for a message has been reached (max 16)

* Implement flush mechanism for desyncs:
 * 1.) Client suspects desync after receiving an error message.
 * 2.) Client sends buffer length * 2 bytes filled with 1's as "flush" sequence to server.
 * 3.) Server responds to next desync'd command(s) which include parts of the flush sequence normally with appropriate error message.
 * 4.) Client ignores error message(s) since it already suspects a desync and is working to remedy it.
 * 5.) Once server detects that it's been receiving all 1's for more than 1 full command length, it agrees to resync and throws out all remaining incoming bytes until it sees one that's all 0's.
 * 6.) Client sends all 0's reset byte to server to indicate end of flush sequence.
 * 7.) Both client and server begin again with next valid command, both sending/receiving the next message starting with the next byte following the 0's byte (done this way in case next valid byte is all 1's which would otherwise blend into flush sequence).

* Add keepalives/timeouts/slient_termination_detection

* Add automatic disconnect for clients that repeatedly induce errors without attempting to resync.

*/

static int start_server()
{
  // Prepare socket for connections
  fputs("Creating server socket... ", stdout);
  server_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if(!validate_condition(server_socket_descriptor > -1, server_socket_descriptor))
  {
    return SERVER_SOCKET_CREATION_FAILED;
  }
  fputs("Binding address info to server socket... ", stdout);
  struct sockaddr_in server_socket_address = {0};
  server_socket_address.sin_family = AF_INET;
  server_socket_address.sin_port = htons(SERVER_PORT_NUMBER);
  server_socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
  int bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_socket_address,
                         sizeof(server_socket_address));
  if(!validate_condition(bind_result > -1, bind_result))
  {
    return SERVER_SOCKET_BIND_FAILED;
  }
  fputs("Opening server socket for connections... ", stdout);
  int listen_result = listen(server_socket_descriptor, SERVER_MAX_CONNECTIONS);
  if(!validate_condition(listen_result > -1, listen_result))
  {
    return SERVER_SOCKET_LISTEN_FAILED;
  }
  server_started = true;
  return GENERAL_NO_ERROR;
}

static void connect_to_client()
{
  fputs("Waiting for connection... ", stdout);
  client_connection_descriptor = accept(server_socket_descriptor, (struct sockaddr*)NULL, NULL);
  if(validate_condition(client_connection_descriptor > -1, client_connection_descriptor))
  {
    connected_to_client = true;
  }
}

static void receive_command_from_client(Data_string* command_string)
{
  fputs("Receiving... ", stdout);
  int receive_result =  recv(client_connection_descriptor, command_string->data, COMMAND_BUFFER_LENGTH,
                             SERVER_RECEIVE_OPTIONS);
  if(validate_condition(receive_result > - 1, receive_result))
  {
    command_received = true;
  }
}

static void send_response_to_client(Data_string* response_string)
{
  fputs("Sending... ", stdout);
  int send_result = send(client_connection_descriptor, response_string->data, response_string->length,
                         SERVER_SEND_OPTIONS);
  if(validate_condition(send_result > - 1, send_result))
  {
    response_sent = true;
  }
}

void receive_command(Data_string* command_string)
{
  while(!server_started)
  {
    start_server();
  }
  while(!connected_to_client)
  {
    connect_to_client();
  }
  command_received = false;
  while(!command_received)
  {
    receive_command_from_client(command_string);
  }
}

void send_response(Data_string* response_string)
{
  response_sent = false;
  while(!response_sent)
  {
    send_response_to_client(response_string);
  }
  if(disconnect_requested)
  {
    fputs("Closing connection to client...  ", stdout);
    int close_result = close(client_connection_descriptor);
    validate_condition(close_result > -1, close_result);
  }
  if(shutdown_requested)
  {
    fputs("Closing server socket...  ", stdout);
    int close_result = close(server_socket_descriptor);
    validate_condition(close_result > -1, close_result);
  }
}

