#pragma once

#include <string>

namespace ftp {

class Client {
private:
  bool pass_mode_ = false;
  bool active_mode_ = false;
  int control_socket_;
  int data_socket_;

public:
  Client(const char *server_ip, int port);
  ~Client();
  void connect_to_server(int _socket, const char *server_ip, int port);
  void execute_command(const std::string &command);

  void login();
  void password();

  void passive_mode();

  void list();
  void pwd();
  void cwd();

  void upload_file(std::string filename);

  std::string print_server_response(int _socket);
  void help();
  void quit();
};

} // namespace ftp