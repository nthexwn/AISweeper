#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "client_interface.h"
#include "common_constants.h"
#include "common_enums.h"
#include "common_functions.h"

static bool client_connected = false;
static bool command_sent = false;
static bool response_received = false;
static bool disconnect_requested = false;
static int client_socket_descriptor = 0;

static int connect_client()
{
  // Prepare socket for connections
  fputs("Creating client socket... ", stdout);
  client_socket_descriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
  if(!validate_condition(client_socket_descriptor > -1, client_socket_descriptor))
  {
    return CLIENT_SOCKET_CREATION_FAILED;
  }
  fputs("Converting server address information... ", stdout);
  struct sockaddr_in server_socket_address = {0};
  server_socket_address.sin_family = AF_INET;
  server_socket_address.sin_port = htons(SERVER_PORT_NUMBER);
  int convert_result = inet_pton(AF_INET, SERVER_ADDRESS, &server_socket_address.sin_addr);
  if(!validate_condition(convert_result > -1, convert_result))
  {
    return CLIENT_SERVER_ADDRESS_CONVERSION_FAILED;
  }
  fputs("Connecting client to server... ", stdout);
  int connect_result = connect(client_socket_descriptor, (struct sockaddr*)&server_socket_address,
                               sizeof(server_socket_address));
  if(!validate_condition(connect_result > -1, connect_result))
  {
    return CLIENT_CONNECTION_TO_SERVER_FAILED;
  }
  client_connected = true;
  return GENERAL_NO_ERROR;
}

static void send_command_to_server(Data_string* command_string)
{
  fputs("Sending... ", stdout);
  int send_result = send(client_socket_descriptor, command_string->data, command_string->length,
                         CLIENT_SEND_OPTIONS);
  if(validate_condition(send_result > - 1, send_result))
  {
    command_sent = true;
  }
  if(command_string->length > 0 && (*command_string->data == COMMAND_DISCONNECT_CLIENT ||
                                    *command_string->data == COMMAND_SHUT_DOWN))
  {
    disconnect_requested = true;
  }
}

static void receive_response_from_server(Data_string* response_string)
{
  fputs("Receiving... ", stdout);
  int receive_result =  recv(client_socket_descriptor, response_string->data, RESPONSE_BUFFER_LENGTH,
                             CLIENT_RECEIVE_OPTIONS);
  if(validate_condition(receive_result > - 1, receive_result))
  {
    response_received = true;
  }
  if(disconnect_requested)
  {
    fputs("Closing connection to server... ", stdout);
    int close_result = close(client_socket_descriptor);
    validate_condition(close_result > -1, close_result);
  }
}

void send_command(Data_string* command_string)
{
  while(!client_connected)
  {
    connect_client();
  }
  command_sent = false;
  while(!command_sent)
  {
    send_command_to_server(command_string);
  }
}

void receive_response(Data_string* response_string)
{
  response_received = false;
  while(!response_received)
  {
    receive_response_from_server(response_string);
  }
}

