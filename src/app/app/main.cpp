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

  std::string username;
  std::string password;
  std::cout << "Введите имя пользователя > ";
  std::cin >> username;
  ftp::login(server_socket, username);
  std::cout << "Введите пароль > ";
  std::cin >> password;
  ftp::password(server_socket, password);

  // while (status_connect == 0) {
  //   std::string command;
  //   std::cout << "Список возможных комманд:\n"
  //             << "LIST - просмотр содержимого каталога\n"
  //             << "RETR - передать файл с сервера на клиент\n"
  //             << "STOR - передать файл с клиента на сервер\n"
  //             << "QUIT - Выход и разрыв соединения\n";
  //   std::cout << "> ";
  //   std::cin >> command;
  // }
}
