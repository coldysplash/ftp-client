#include <ftpclient/client.hpp>

#include <iostream>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <server_ip> <server_port>\n";
    return -1;
  }
  try {
    const char *server_ip = argv[1];
    int server_port = atoi(argv[2]);

    ftp::Client client(server_ip, server_port);

    client.login();
    client.password();

    while (1) {
      std::string command;
      std::cout << "> ";
      std::getline(std::cin, command);

      if (command == "PASV") {
        client.passive_mode();
      } else if (command == "LIST") {
        client.list();
      } else if (command == "PWD") {
        client.pwd();
      } else if (command.substr(0, 3) == "CWD") {
        client.change_dir(command);
      } else if (command.substr(0, 4) == "STOR") {
        client.upload_file(command);
      } else if (command.substr(0, 4) == "RETR") {
        client.download_file(command);
      } else if (command == "HELP") {
        client.help();
      } else if (command == "QUIT") {
        client.quit();
        break;
      }
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
  }
}