#include <ftpclient/client.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

namespace ftp {

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

  return status_connect;
}

void print_server_response(int socket) {
  char buff[1024];
  memset(buff, 0, sizeof(buff));
  recv(socket, &buff, 1024, 0);
  std::cout << buff;
}

} // namespace ftp