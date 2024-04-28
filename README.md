# FTP-client
## Introduction

FTP-client is a simple console network application working with FTP protocol in Linux OS. 
The client functionality is capable of connecting to an ftp server, browsing directories, transferring and downloading files.
My application implements only passive mode for data transfer between client and server.

## Usage

1. Clone the repository to your local machine and build project:

        git clone https://github.com/coldysplash/ftp-client.git
        cd ftp-client
        cmake --preset release
        cmake --build --preset release

3. When running the application, enter the server ip and port as arguments:

        ./build/release/bin/app "server-ip" "server-port"

## Example of usage:

![Снимок экрана от 2024-04-28 15-53-07](https://github.com/coldysplash/ftp-client/assets/99001708/4197d1a8-2639-4e4c-932c-06e099303c22)
