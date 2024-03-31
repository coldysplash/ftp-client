#include <ftpclient/client.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 1024

namespace {
void parse_addr_port(const char *pars_str, std::string &ip, int &port) {
  std::string response = pars_str;
  size_t start = response.find('(') + 1;
  size_t end = response.find(')');
  response = response.substr(start, end - start);

  std::vector<std::string> tokens;
  start = end = 0;
  while ((start = response.find_first_not_of(',', end)) != std::string::npos) {
    end = response.find(',', start);
    tokens.push_back(response.substr(start, end - start));
  }
  ip += tokens[0] + '.' + tokens[1] + '.' + tokens[2] + '.' + tokens[3];
  port = std::stoi(tokens[4]) * 256 + std::stoi(tokens[5]);
}
} // namespace

namespace ftp {

void print_server_response(int comm_socket) {
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(comm_socket, &buff, BUFFER_SIZE, 0);
  std::cout << buff;
}

int connect_to_server(int comm_socket, const char *server_ip, int port) {
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(server_ip);
  address.sin_port = htons(port);

  int status_connect = connect(
      comm_socket, reinterpret_cast<sockaddr *>(&address), sizeof(address));

  if (status_connect < 0) {
    std::cerr << "Error connecting to server!" << std::endl;
    close(comm_socket);
    return -1;
  }
  return status_connect;
}

void login(int comm_socket) {
  std::string username;
  std::cout << "Введите имя пользователя > ";
  std::cin >> username;
  char message[BUFFER_SIZE];
  sprintf(message, "USER %s\r\n", username.c_str());
  send(comm_socket, message, strlen(message), 0);
  print_server_response(comm_socket);
}

void password(int comm_socket) {
  std::string passw;
  std::cout << "Введите пароль > ";
  std::cin >> passw;
  char message[BUFFER_SIZE];
  sprintf(message, "PASS %s\r\n", passw.c_str());
  send(comm_socket, message, strlen(message), 0);
  print_server_response(comm_socket);
}

void quit(int comm_socket) {
  send(comm_socket, "QUIT\r\n", strlen("QUIT\r\n"), 0);
  print_server_response(comm_socket);
}

int passive_mode(int comm_socket) {
  send(comm_socket, "PASV\r\n", strlen("PASV\r\n"), 0);
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(comm_socket, &buff, BUFFER_SIZE, 0);
  std::cout << buff;

  std::string ip_addr;
  int port = 0;
  parse_addr_port(buff, ip_addr, port);

  int data_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (data_socket < 0) {
    std::cerr << "Error creating socket!" << std::endl;
    return -1;
  }

  connect_to_server(data_socket, ip_addr.c_str(), port);

  return data_socket;
}

void list(int comm_socket, int data_socket) {
  send(comm_socket, "LIST\r\n", strlen("LIST\r\n"), 0);
  print_server_response(comm_socket);
  print_server_response(data_socket);
  print_server_response(comm_socket);
}

void pwd(int comm_socket) {
  send(comm_socket, "PWD\r\n", strlen("PWD\r\n"), 0);
  print_server_response(comm_socket);
}

void help() {
  std::cout << "\tPASV - войти в пассивный режим\n"
            << "\tLIST - просмотр содержимого каталога\n"
            << "\tPWD  - путь к текущему каталог\n"
            // << "RETR - передать файл с сервера на клиент\n"
            // << "STOR - передать файл с клиента на сервер\n"
            << "\tQUIT - Выход и разрыв соединения\n"
            << "\tHELP - список доступных команд сервера\n";
}

} // namespace ftp