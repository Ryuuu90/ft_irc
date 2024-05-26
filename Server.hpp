#pragma once

#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <string.h>
#include "Client.hpp"

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <unistd.h>
#include <map>
#include <sstream>
#include "Channel.hpp"

#define BUFFER_SIZE 1024

# define GREEN "\033[1;32m"
# define GREEN2 "\033[0;32m"
# define YELLOW "\033[0;33m"
# define MAGENTA "\033[1;35m"
# define RED "\033[1;31m"
# define RESET "\033[0m"
# define CYAN "\033[1;36m"
# define WHITE "\033[1;16m"
# include "Channel.hpp"
#include "bot.hpp"
#define ERR_NOSUCHCHANNEL 403
class Client;
class Channel;
class Server
{
    private:
        int port;
        int passFlag;
        std::string password;
        int sockserv, sockcli;
        struct  sockaddr_in seraddress, cliaddress;
        std::vector<struct pollfd>fds; 
        struct pollfd npollfd;
        char buff[BUFFER_SIZE];
        std::map<int, Client> Clients;
        std::map<std::string, Channel> Channels;
        int authenFlag;
        std::string timeStr;
        Server();
    public:
        Server(int port, std::string password);
        ~Server();
        void serverSocket();
        void acceptClients();
        void receiveData(int index);
        void authentication(std::string buff, int index);
        void ParamMsgClient(int fd, std::string command, std::string msg);
        void msgToClient(int fd, std::string msg);
        void PassCommand(int fd, std::vector<std::string> &vec);
        void NickCommand(int fd, std::vector<std::string> &vec);
        bool NickNameInUse(std::string nName, int fd);
        bool checkNickName(std::string nName);
        void UserCommand(int fd, std::vector<std::string> &vec);
};
