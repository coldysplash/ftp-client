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
  std::string execute_command(const std::string &command);

  void login();
  void password();

  void passive_mode();
  void active_mode();

  void list();
  void pwd();
  void change_dir(const std::string &command);

  int upload_file(const std::string &command);
  int download_file(const std::string &command);

  std::string print_server_response(int _socket);
  void help();
  void quit();
};

} // namespace ftp