#include <ftpclient/client.hpp>

#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdexcept>
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

Client::Client(const char *server_ip, int port) {
  control_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (control_socket_ < 0) {
    throw std::runtime_error("Error creating socket!");
  }
  connect_to_server(control_socket_, server_ip, port);
  print_server_response(control_socket_);
}

Client::~Client() {
  close(control_socket_);
  close(data_socket_);
}

void Client::connect_to_server(int _socket, const char *server_ip, int port) {
  sockaddr_in address{};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(server_ip);
  address.sin_port = htons(port);

  if (connect(
          _socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) <
      0) {
    throw std::runtime_error("Error connecting to server!");
    close(_socket);
  } else {
    std::cout << "Connecting complete!\n";
  }
}

void Client::execute_command(const std::string &command) {
  char message[BUFFER_SIZE];
  sprintf(message, "%s\r\n", command.c_str());
  send(control_socket_, message, strlen(message), 0);
  print_server_response(control_socket_);
}

void Client::login() {
  std::string username;
  std::cout << "Введите имя пользователя > ";
  std::cin >> username;
  execute_command("USER " + username);
}

void Client::password() {
  std::string passw;
  std::cout << "Введите пароль > ";
  std::cin >> passw;
  execute_command("PASS " + passw);
}

void Client::passive_mode() {
  send(control_socket_, "PASV\r\n", strlen("PASV\r\n"), 0);
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(control_socket_, &buff, BUFFER_SIZE, 0);
  std::cout << buff;

  std::string ip_addr;
  int port = 0;
  parse_addr_port(buff, ip_addr, port);

  data_socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (data_socket_ < 0) {
    throw std::runtime_error("Error creating socket!");
  }

  connect_to_server(data_socket_, ip_addr.c_str(), port);
  pass_mode_ = true;
}

void Client::list() {
  execute_command("LIST");
  if (pass_mode_ || active_mode_) {
    print_server_response(data_socket_);
    print_server_response(control_socket_);
  }
}

void Client::pwd() { execute_command("PWD"); }

void Client::print_server_response(int _socket) {
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(_socket, &buff, BUFFER_SIZE, 0);
  std::cout << buff;
}

void Client::help() {
  std::cout << "\tPASV - войти в пассивный режим\n"
            << "\tLIST - просмотр содержимого каталога\n"
            << "\tPWD  - путь к текущему каталог\n"
            // << "RETR - передать файл с сервера на клиент\n"
            // << "STOR - передать файл с клиента на сервер\n"
            << "\tQUIT - Выход и разрыв соединения\n"
            << "\tHELP - список доступных команд сервера\n";
}

void Client::quit() { execute_command("QUIT"); }

} // namespace ftp