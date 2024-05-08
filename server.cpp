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

int main(int ac, char **av)
{

    int sockserv, sockcli;
    struct  sockaddr_in seraddress, cliaddress;
    std::vector<struct pollfd> fds;
    struct pollfd npollfd;
    char buff[BUFFER_SIZE];
    if(ac != 3)
    {
        std::cerr << "Error: usage <port> <password>." << std::endl;
        exit(1);
    }
    int port = std::atoi(av[1]);
    if((sockserv = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        std::cerr << "Error: Socket creation fails." << std::endl;
        exit(1);
    }
    int en = 1;
    if(setsockopt(sockserv, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
    {
        std::cerr << "Error: fcntl() failed." << std::endl;
        exit(1);
    }
    if(fcntl(sockserv, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error: fcntl() failed." << std::endl;
        exit(1);
    }
    bzero(&seraddress, sizeof(seraddress));
    seraddress.sin_family = AF_INET;
    seraddress.sin_port = htons(port);
    seraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sockserv, (struct sockaddr *) &seraddress, sizeof(seraddress)) == -1)
    {
        std::cerr << "Error: Binding the socket and the address of the server fails." << std::endl;
        exit(1);
    }
    if(listen(sockserv, SOMAXCONN) == -1)
    {
        std::cerr << "Error: Listening to incoming connections fails." << std::endl;
        exit(1);
    }
    bzero(&npollfd, sizeof(npollfd));
    npollfd.fd = sockserv;
    npollfd.events = POLLIN;
    npollfd.revents = 0;
    fds.push_back(npollfd);
    while(true)
    {
        std::cout << "ccc" << std::endl;
        if(poll(&fds[0], fds.size(), -1) == -1)
        {
            std::cerr << "Error: faild to poll." << std::endl;
            exit(1);
        }
        for(int i  = 0; i < fds.size(); i++)
        {
            if(fds[i].revents & POLLIN)
            {
                if(fds[i].fd == sockserv)
                {
                    std::cout << "bbbb" << std::endl;
                    socklen_t len = sizeof(cliaddress);
                    if((sockcli = accept(sockserv, (struct sockaddr *) &cliaddress, &len)) < 0)
                    {
                        std::cerr << "Error: accepting the new client fails." << std::endl;
                        exit(1);
                    }
                    if(fcntl(sockcli, F_SETFL, O_NONBLOCK) == -1)
                    {
                        std::cerr << "Error: fcntl() failed." << std::endl;
                        exit(1);
                    }
                    npollfd.fd = sockcli;
                    npollfd.events = POLLIN;
                    npollfd.revents = 0;
                    fds.push_back(npollfd);
                }
                else
                {
                    bzero(buff, BUFFER_SIZE);
                    int bytes;
                    if((bytes = recv(fds[i].fd, buff, sizeof(buff) - 1, 0)) <= 0)
                    {
                        std::cerr << "Client (" << fds[i].fd << ") Disconnected." << std::endl;
                        close(fds[i].fd);
                    }
                    else
                    {
                        buff[bytes] = '\0';
                        std::cout << "the data :" << buff << std::endl;
                    }
                }
            }
        }
        
    }
    for(int j = 0; j < fds.size(); j++)
        close(fds[j].fd);
}