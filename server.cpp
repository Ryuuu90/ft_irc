#include "server.hpp"

Server::Server(int port, std::string password)
{
    this->port = port;
    this->password = password;
    this->passFlag = 0;
    serverSocket();
    std::cout << GREEN << "Server (" << this->sockserv << ") is connected." << RESET << std::endl;
    std::cout << YELLOW << "The server is waiting for new connections..." << RESET << std::endl;
    while(true)
    {
        if(poll(&fds[0], fds.size(), -1) == -1)
            throw(std::runtime_error("Error: Faild to poll."));
        for(size_t i = 0 ; i < this->fds.size(); i++)
        {
            if(this->fds[i].revents & POLLIN)
            {
                if(this->fds[i].fd == this->sockserv)
                    this->acceptClients();
                else
                    this->receiveData(i);

            }
        }
    }
}
Server::~Server()
{
    for(size_t j = 0; j < fds.size(); j++)
        close(fds[j].fd);
}
void Server::serverSocket()
{
    if((this->sockserv = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw(std::runtime_error( "Error: Socket creation failed."));
    int en = 1;
    if(setsockopt(this->sockserv, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1)
        throw(std::runtime_error( "Error: Reuse address failed."));
    if(fcntl(this->sockserv, F_SETFL, O_NONBLOCK) == -1)
        throw(std::runtime_error( "Error: fcntl() failed."));
    bzero(&this->seraddress, sizeof(this->seraddress));
    this->seraddress.sin_family = AF_INET;
    this->seraddress.sin_port = htons(this->port);
    this->seraddress.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(this->sockserv, (struct sockaddr *) &this->seraddress, sizeof(this->seraddress)) == -1)
        throw(std::runtime_error( "Error: Binding the socket and the address of the server failed."));
    if(listen(this->sockserv, SOMAXCONN) == -1)
        throw(std::runtime_error( "Error: Listening to incoming connections failed."));
    bzero(&this->npollfd, sizeof(this->npollfd));
    this->npollfd.fd = this->sockserv;
    this->npollfd.events = POLLIN;
    this->npollfd.revents = 0;
    fds.push_back(this->npollfd);
}
void Server::acceptClients()
{
    socklen_t len = sizeof(this->cliaddress);
    if((this->sockcli = accept(this->sockserv, (struct sockaddr *) &this->cliaddress, &len)) < 0)
    {
        std::cerr << RED << "Error: Accepting the new client failed." << RESET << std::endl;
        return;
    }
    if(fcntl(this->sockcli, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << RED << "Error: fcntl() failed." << RESET << std::endl;
        return;
    }
    this->npollfd.fd = this->sockcli;
    this->npollfd.events = POLLIN;
    this->npollfd.revents = 0;
    fds.push_back(this->npollfd);
}
void Server::receiveData(int index)
{
    bzero(this->buff, BUFFER_SIZE);
    int rec;
    if((rec = recv(this->fds[index].fd, this->buff, sizeof(this->buff) - 1, 0)) <= 0)
    {
        std::cerr << MAGENTA << "Client (" << this->fds[index].fd << ") Disconnected." << RESET << std::endl;
        close(this->fds[index].fd);
        this->fds.erase(this->fds.begin() + index);
    }
    else
    {
        this->buff[rec] = '\0';
        char buffNl[1024];
        int i;
        for( i = 0; buff[i] != '\n'; i++)
        {
            buffNl[i] = buff[i];
        }
        buffNl[i] = '\0';
        if(passFlag == 0 && password == buffNl)
        {
            std::cout << GREEN2 << "Client (" << this->sockcli << ") Connected." << RESET << std::endl;
            passFlag = 1;
        }
        else if(passFlag == 1)
            std::cout << CYAN << "the data :" << this->buff << RESET;
        else
            std::cerr << RED << "Password incorrect." << buffNl << RESET << std::endl;
    }
}
