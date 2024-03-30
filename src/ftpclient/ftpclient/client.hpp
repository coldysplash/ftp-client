#pragma once

#include <string>

namespace ftp {

void print_server_response(int socket);
int connect_to_server(int socket, const char *server_ip, int port);
void login(int socket, const std::string &username);
void password(int socket, const std::string &pass);

} // namespace ftp
