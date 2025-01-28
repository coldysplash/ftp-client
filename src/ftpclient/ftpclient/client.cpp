#include <ftpclient/client.hpp>

#include <algorithm>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#define BUFFER_SIZE 1024

namespace {
void parse_addr_port(std::string &response, std::string &ip, int &port) {
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

int parse_file_size(std::string pars_str) {
  size_t start = pars_str.find('(') + 1;
  size_t end = pars_str.find(')');
  pars_str = pars_str.substr(start, end - start);

  return std::stoi(pars_str.substr(0, pars_str.find(' ')));
}

void parse_file_path(std::string &file_path) {
  size_t pos = file_path.find_last_of('/');
  if (pos != std::string::npos) {
    file_path = file_path.substr(pos + 1);
  }
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

void Client::connect_to_server(
    int _socket, const char *server_ip, int port) const {
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

std::string Client::execute_command(const std::string &command) const {
  char message[BUFFER_SIZE];
  sprintf(message, "%s\r\n", command.c_str());
  send(control_socket_, message, strlen(message), 0);
  return print_server_response(control_socket_);
}

std::string Client::print_server_response(int _socket) const {
  char buff[BUFFER_SIZE];
  memset(buff, 0, sizeof(buff));
  recv(_socket, &buff, BUFFER_SIZE, 0);
  std::cout << buff;
  return buff;
}

int Client::login() const {
  std::string username;
  std::cout << "Enter Username > ";
  std::cin >> username;
  if (execute_command("USER " + username).substr(0, 3) == "530") {
    return -1;
  };
  return 0;
}

void Client::password() const {
  std::string passw;
  std::cout << "Enter Password > ";
  std::cin >> passw;
  execute_command("PASS " + passw);
}

void Client::passive_mode() {
  if (!pass_mode_) {
    std::string reply = execute_command("PASV");
    if (reply.substr(0, 3) != "530") {
      std::string ip_addr;
      int port = 0;
      parse_addr_port(reply, ip_addr, port);

      data_socket_ = socket(AF_INET, SOCK_STREAM, 0);
      if (data_socket_ < 0) {
        throw std::runtime_error("Error creating socket!");
      }

      connect_to_server(data_socket_, ip_addr.c_str(), port);
      pass_mode_ = true;
    }
  }
}

void Client::list() {
  execute_command("LIST");
  if (pass_mode_) {
    print_server_response(data_socket_);
    print_server_response(control_socket_);
    close(data_socket_);
    pass_mode_ = false;
  }
}

void Client::pwd() const { execute_command("PWD"); }

void Client::change_dir(const std::string &command) const {
  execute_command(command);
}

int Client::upload_file(const std::string &command) {
  std::string file_path;
  size_t it = command.find_first_of(' ');
  if (it != std::string::npos) {
    file_path = command.substr(it + 1);
  }
  FILE *file = fopen(file_path.c_str(), "rb");
  if (!file) {
    std::cerr << "Error open file!\n";
    return -1;
  }
  parse_file_path(file_path);
  std::string reply = execute_command("STOR " + file_path);
  if (reply.substr(0, 3) == "150") {
    int count;
    char databuf[BUFFER_SIZE];
    while (!feof(file)) {
      count = fread(databuf, 1, BUFFER_SIZE, file);
      send(data_socket_, databuf, count, 0);
    }
    send(data_socket_, "\r\n", strlen("\r\n"), 0);
    fclose(file);
    close(data_socket_);
    pass_mode_ = false;
    print_server_response(control_socket_);
  } else {
    return -1;
  }
  return 0;
}

int Client::download_file(const std::string &command) {
  std::string file_path;
  size_t it = command.find_first_of(' ');
  if (it != std::string::npos) {
    file_path = command.substr(it + 1);
  }
  std::string reply = execute_command("RETR " + file_path);
  if (reply.substr(0, 3) == "150") {
    int file_size = parse_file_size(reply);
    FILE *file = fopen(file_path.c_str(), "wb");
    int read = 0;
    while (read < file_size) {
      char databuff[BUFFER_SIZE];
      int readed = recv(data_socket_, databuff, sizeof(databuff), 0);
      fwrite(databuff, 1, readed, file);
      read += readed;
    }
    fclose(file);
    close(data_socket_);
    pass_mode_ = false;
    print_server_response(control_socket_);
  } else {
    return -1;
  }

  return 0;
}

void Client::help() const {
  std::cout
      << "\tPASV - enter passive mode\n"
      << "\tLIST - view directory contents\n"
      << "\tPWD  - path to the current directory\n"
      << "\tCWD <path> - change to another directory\n"
      << "\tRETR <file path> - download a file from the server to the client\n"
      << "\tSTOR <file path> - upload a file from the client to the server\n"
      << "\tQUIT - Exit and disconnect\n"
      << "\tHELP - list of available server commands\n";
}

void Client::quit() const { execute_command("QUIT"); }

} // namespace ftp