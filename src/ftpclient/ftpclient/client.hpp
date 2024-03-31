#pragma once

#include <string>

namespace ftp {

void print_server_response(int socket_);
int connect_to_server(int socket_, const char *server_ip, int port);
void login(int socket_);
void password(int socket_);
void quit(int socket_);

} // namespace ftp
