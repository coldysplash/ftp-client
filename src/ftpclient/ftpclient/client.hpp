#pragma once

#include <string>

namespace ftp {

class Client final {
private:
  bool pass_mode_ = false;
  int control_socket_ = 0;
  int data_socket_ = 0;

public:
  Client(const char *server_ip, int port);
  ~Client();
  void connect_to_server(int _socket, const char *server_ip, int port) const;
  std::string execute_command(const std::string &command) const;
  std::string print_server_response(int _socket) const;

  int login() const;
  void password() const;

  void passive_mode();
  // void active_mode();

  void list();
  void pwd() const;
  void change_dir(const std::string &command) const;

  int upload_file(const std::string &command);
  int download_file(const std::string &command);

  void help() const;
  void quit() const;
};

} // namespace ftp