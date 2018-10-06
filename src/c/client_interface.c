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
  fputs("Sending command... ", stdout);
  int command_length_net = 0;
  transfer_value((unsigned char*)&command_string->length, detect_machine_byte_order(),
                 (unsigned char*)&command_length_net, ENDIAN_BIG, sizeof(int));
  int send_result = send(client_socket_descriptor, &command_length_net, sizeof(int), CLIENT_SEND_OPTIONS);
  if(send_result > -1)
  {
    send_result = send(client_socket_descriptor, command_string->data, command_string->length,
                       CLIENT_SEND_OPTIONS);
  }
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
  fputs("Receiving response... ", stdout);
  int receive_result = -1;
  int bytes_received = 0;
  int response_size_net = 0;
  int response_size_local = 0;
  while(bytes_received < sizeof(int))
  {
    receive_result = recv(client_socket_descriptor, &response_size_net + bytes_received, sizeof(int),
                          CLIENT_RECEIVE_OPTIONS);
    if(receive_result < 0)
    {
      break;
    }
    bytes_received += receive_result;
  }
  if(bytes_received == sizeof(int))
  {
    transfer_value((unsigned char*)&response_size_net, ENDIAN_BIG, (unsigned char*)&response_size_local,
                   detect_machine_byte_order(), sizeof(int));
  }
  bytes_received = 0;
  while(bytes_received < response_size_local)
  {
    receive_result = recv(client_socket_descriptor, response_string->data + bytes_received, response_size_local,
                          CLIENT_RECEIVE_OPTIONS);
    if(receive_result < 0)
    {
      break;
    }
    bytes_received += receive_result;
  }
  if(bytes_received == response_size_local)
  {
    response_received = true;
    response_string->length = bytes_received;
  }
  validate_condition(response_received, bytes_received);
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

