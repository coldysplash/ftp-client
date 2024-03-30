#include <ftpclient/client.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

namespace ftp {

void print_server_response(int socket) {
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(socket, &buff, BUFFER_SIZE, 0);
  std::cout << buff;
}

int connect_to_server(int socket, const char *server_ip, int port) {
  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(server_ip);
  server_address.sin_port = htons(port);

  int status_connect = connect(
      socket,
      reinterpret_cast<sockaddr *>(&server_address),
      sizeof(server_address));

  if (status_connect < 0) {
    std::cerr << "Error connecting to server!" << std::endl;
    close(socket);
    return -1;
  }
  print_server_response(socket);
  return status_connect;
}

void login(int socket, const std::string &username) {
  char message[BUFFER_SIZE];
  sprintf(message, "USER %s\r\n", username.c_str());
  send(socket, message, strlen(message), 0);
  print_server_response(socket);
}

void password(int socket, const std::string &pass) {
  char message[BUFFER_SIZE];
  sprintf(message, "PASS %s\r\n", pass.c_str());
  send(socket, message, strlen(message), 0);
  print_server_response(socket);
}

} // namespace ftp