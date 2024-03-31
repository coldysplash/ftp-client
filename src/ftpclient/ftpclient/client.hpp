#pragma once

#include <string>

namespace ftp {

void print_server_response(int comm_socket);
int connect_to_server(int comm_socket, const char *server_ip, int port);
void login(int comm_socket);
void password(int comm_socket);
void quit(int comm_socket);
int passive_mode(int comm_socket);
void list(int comm_socket, int data_socket);
void pwd(int comm_socket);
void help();

} // namespace ftp