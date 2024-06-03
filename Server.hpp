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
 
#define SERVER_NAME "\033[1;36m                                                                                          \n" \
                    "                               888                                                   888            \n" \
                    "                               888                                                   888            \n" \
                    "        888  888  888  .d88b.  888  .d8888b .d88b.  88888b d88b.   .d88b.            888888 .d88b.  \n" \
                    "        888  888  888 d8P  Y8b 888 d88P'   d88'88b 888 '888 '88b d8P  Y8b            888    d88'88b \n" \
                    "        888  888  888 88888888 888 888     888  888 888  888  888 88888888           888   888  888 \n" \
                    "        Y88b 888 d88P Y8b.     888 Y88b.   Y88..88P 888  888  888 Y8b.               Y88b. Y88..88P \n" \
                    "        'Y8888888P'   'Y8888  888  'Y8888P 'Y88P'  888  888  888  'Y8888             'Y888 'Y88P'   \n" \
                    "            888       888 8888888888 888888b     .d8888b.  8888888888 88888888   888     888        \n" \
                    "            888   o   888 888        888  '88b  d88P  Y88b 888        888   Y88b 888     888        \n" \
                    "            888  d8b  888 888        888  .88P  Y88b.      888        888    888 888     888        \n" \
                    "            888 d888b 888 8888888    8888888K.   'Y888b.   8888888    888   d88P Y88b   d88P        \n" \
                    "            888d88888b888 888        888  'Y88b     'Y88b. 888        8888888P    Y88b d88P         \n" \
                    "            88888P Y88888 888        888    888       '888 888        888 T88b     Y88o88P          \n" \
                    "            8888P   Y8888 888        888   d88P Y88b  d88P 888        888  T88b     Y888P           \n" \
                    "            888P     Y888 8888888888 8888888P'   'Y8888P'  8888888888 888   T88b     Y8P            \n" \
                    "                                                                                            \033[0m \n"                                                                                                                                             
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
        int joinFlag;
        std::string join;
        std::string password;
        int sockserv, sockcli;
        struct  sockaddr_in seraddress, cliaddress;
        std::vector<struct pollfd>fds; 
        struct pollfd npollfd;
        char buff[BUFFER_SIZE];
        std::map<int, Client> Clients;
        std::map<std::string, Channel> Channels;
        std::map<int, int> authenFlag;
        std::string timeStr;
        Server();
    public:
        Server(int port, std::string password);
        ~Server();
        void serverSocket();
        void acceptClients();
        void receiveData(int index);
        void authentication(std::string buff, int index);
        void ParamMsgClient(int fd, std::string command, std::string msg, int errorNum);
        void msgToClient(int fd, std::string msg, int errorNum);
        void PassCommand(int fd, std::vector<std::string> &vec);
        void NickCommand(int fd, std::vector<std::string> &vec);
        bool NickNameInUse(std::string nName, int fd);
        bool checkNickName(std::string nName);
        void UserCommand(int fd, std::vector<std::string> &vec);
        bool checkControlD(int rec);
        void commands(std::string msg,std::vector<struct pollfd>fds, int index);
};
