#pragma once

#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <unistd.h>

#define BUFFER_SIZE 1024

# define GREEN "\033[1;32m"
# define GREEN2 "\033[0;32m"
# define YELLOW "\033[0;33m"
# define MAGENTA "\033[1;35m"
# define RED "\033[1;31m"
# define RESET "\033[0m"
# define CYAN "\033[1;36m"
# define WHITE "\033[1;16m"

class Server
{
    private:
        int port;
        int passFlag;
        std::string password;
        int sockserv, sockcli;
        struct  sockaddr_in seraddress, cliaddress;
        std::vector<struct pollfd> fds;
        struct pollfd npollfd;
        char buff[BUFFER_SIZE];
        Server();
    public:
        Server(int port, std::string password);
        ~Server();
        void serverSocket();
        void acceptClients();
        void receiveData(int index);
        
};
