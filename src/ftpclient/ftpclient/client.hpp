#pragma once

namespace ftp {

int connect_to_server(int socket, const char *server_ip, int port);
void print_server_response(int socket);

} // namespace ftp
