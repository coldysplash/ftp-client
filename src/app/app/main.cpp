#include <ftpclient/client.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>\n";
    return -1;
  }

  const char *server_ip = argv[1];
  int server_port = atoi(argv[2]);

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    std::cerr << "Error creating socket!" << std::endl;
    return -1;
  }

  int status_connect =
      ftp::connect_to_server(server_socket, server_ip, server_port);

  ftp::print_server_response(server_socket);
  ftp::login(server_socket);
  ftp::password(server_socket);

  int data_socket = 0;
  while (status_connect == 0) {
    std::string command;
    std::cout << "> ";
    std::cin >> command;

    if (command == "PASV") {
      data_socket = ftp::passive_mode(server_socket);
    } else if (command == "LIST") {
      ftp::list(server_socket, data_socket);
    } else if (command == "PWD") {
      ftp::pwd(server_socket);
    } else if (command == "HELP") {
      ftp::help();
    } else if (command == "QUIT") {
      ftp::quit(server_socket);
      status_connect = -1;
    }
  }
  close(server_socket);
}
