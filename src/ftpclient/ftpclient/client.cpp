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

void print_server_response(int socket_) {
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(socket_, &buff, BUFFER_SIZE, 0);
  std::cout << buff;
}

int connect_to_server(int socket_, const char *server_ip, int port) {
  sockaddr_in server_address{};
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = inet_addr(server_ip);
  server_address.sin_port = htons(port);

  int status_connect = connect(
      socket_,
      reinterpret_cast<sockaddr *>(&server_address),
      sizeof(server_address));

  if (status_connect < 0) {
    std::cerr << "Error connecting to server!" << std::endl;
    close(socket_);
    return -1;
  }
  print_server_response(socket_);
  return status_connect;
}

void login(int socket_) {
  std::string username;
  std::cout << "Введите имя пользователя > ";
  std::cin >> username;
  char message[BUFFER_SIZE];
  sprintf(message, "USER %s\r\n", username.c_str());
  send(socket_, message, strlen(message), 0);
  print_server_response(socket_);
}

void password(int socket_) {
  std::string passw;
  std::cout << "Введите пароль > ";
  std::cin >> passw;
  char message[BUFFER_SIZE];
  sprintf(message, "PASS %s\r\n", passw.c_str());
  send(socket_, message, strlen(message), 0);
  print_server_response(socket_);
}

void quit(int socket_) {
  send(socket_, "QUIT\r\n", strlen("QUIT\r\n"), 0);
  print_server_response(socket_);
}

} // namespace ftp