#pragma once

namespace ftp {

class Client {
private:
  bool pass_mode_ = false;
  bool active_mode_ = false;
  int control_socket_;
  int data_socket_;

public:
  Client(const char *server_ip, int port);
  void connect_to_server(int _socket, const char *server_ip, int port);

  void login();
  void password();

  void passive_mode();

  void list();
  void pwd();

  void stor(int comm_socket, int data_socket);

  void print_server_response(int _socket);
  void help();
  void quit();
};

} // namespace ftp