#include "Server.hpp"

Server::Server(int port, std::string password)
{
    this->port = port;
    this->password = password;
    this->passFlag = 0;
    time_t currentTime = time(NULL);
    this->timeStr = ctime(&currentTime);
    this->timeStr.erase(this->timeStr.end() - 1);
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
void Server::msgToClient(int fd, std::string msg)
{
    std::stringstream ss;
    ss << RED ": <" << fd << ">" << msg << RESET;
    msg = ss.str();
    send(fd, msg.c_str(), msg.size(), 0);
}
void Server::ParamMsgClient(int fd, std::string command , std::string msg)
{
    std::stringstream ss;
    ss << RED << ": <" << fd << ">" << " <" << command << ">" << msg << RESET;
    msg = ss.str();
    send(fd, msg.c_str(), msg.size(), 0);
}
bool Server::NickNameInUse(std::string nName, int fd)
{
    std::map<int , Client>::iterator it;

    for(it = Clients.begin(); it != Clients.end(); it++)
    {
        if(it->second.nickNameGetter() == nName && it->first != fd)
            return true;
    }
    return false;
}
bool Server::checkNickName(std::string nName)
{
    if (std::isdigit(nName[0]) || nName[0] == '#' || nName[0] == '&' || nName[0] == ':')
        return false;
    const char* validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]{}#:\\|";
    for (size_t i = 0; i < nName.length(); ++i) 
    {
        if (strchr(validChars, nName[i]) == NULL) 
            return false;
    }
    return true;
}
#include <stdio.h>
void Server::UserCommand(int fd, std::vector<std::string> &vec)
{
    std::string msg;
    if(vec[0] != "USER")
    {
        msg = "\033[1;35mPlease enter USER command before the username.\033[0m\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    if(vec[0] == "USER" && vec.size() < 5)
        ParamMsgClient(fd, vec[0], " 461 :Not enough parameters\r\n");
    if(vec[0] == "USER" && vec.size() > 6)
        ParamMsgClient(fd, vec[0], " :Too much parameters\r\n");
    if((vec.size() == 5 || vec.size() == 6) && vec[0] == "USER" && Clients.find(fd) != Clients.end() && Clients.size() != 1)
        msgToClient(fd, " 462 :You may not register\r\n");
    if(vec[0] == "USER" && (vec.size() == 5 || vec.size() == 6))
    {
        if(vec[2] != "0" || vec[3] != "*")
            ParamMsgClient(fd, vec[0], " :The third paramter should be '0' and the fourth parameter should be '*'.\r\n");
        else if(vec[4][0] != ':')
            ParamMsgClient(fd, vec[0], " :The real should prefixed with a colon(:).\r\n");
        else
        {
            Clients[fd].userNameSetter(vec[1]);
            vec[4].erase(vec[4].begin());
            if(vec[4].empty())
            {
                ParamMsgClient(fd, vec[0], " :Enter your realname.\r\n");
                return;
            }
            else if(vec[5].empty())
                Clients[fd].realNameSetter(vec[4]);
            else
                Clients[fd].realNameSetter(vec[4] + vec[5]);
            this->authenFlag++;
            msg = "\033[1;32mUser gets registered with username '" + Clients[fd].userNameGetter() + "' and realname '" + Clients[fd].realNameGetter() + "'\033[0m\r\n";
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
    vec.clear();
}
void Server::NickCommand(int fd, std::vector<std::string> &vec)
{
    std::string msg;
    Client cli;
    if(vec[0] != "NICK")
    {
        msg = "\033[1;35mPlease enter NICK command before the nickname.\033[0m\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    if(vec[0] == "NICK" && vec.size() == 1)
        msgToClient(fd, "431 :No nickname given\r\n");
    if(vec[0] == "NICK" && vec.size() > 2)
        ParamMsgClient(fd, vec[0], " :Too much parameters\r\n");
    if(vec[0] == "NICK" && vec.size() == 2)
    {
        if(NickNameInUse(vec[1], fd))
        {
            ParamMsgClient(fd, vec[1], " 433 :Nickname is already in use.\r\n");
        }
        else
        {
            if(!checkNickName(vec[1]))
                ParamMsgClient(fd, vec[1], " 432 :Erroneus nickname.\r\n");
            else
            {
                this->authenFlag++;
                cli.nickNameSetter(vec[1]);
                std::cout<<"--> NICK "<<vec[1]<<std::endl;
                std::cout<<"--> NICK "<<cli.nickNameGetter()<<std::endl;
                Clients[fd] = cli;
                msg = "\033[1;32mRequesting the new nick " + Clients[fd].nickNameGetter() + "\r\n\033[0m";
                send(fd, msg.c_str(), msg.size(), 0);
            }
        }
    }
    if(this->authenFlag == 2)
    {
        msg = "\033[1;35mPlease enter your username and your realname:\033[0m\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    vec.clear();
}
void Server::PassCommand(int fd, std::vector<std::string> &vec)
{
    std::string msg;
    if(vec[0] != "PASS" && vec[0] != "CAP")
    {
        msg = "\033[1;35mplease enter PASS command before the password.\033[0m\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    if(vec.size() != 2 && vec[0] == "PASS")
        ParamMsgClient(fd, vec[0], " 461 :Not enough parameters\r\n");
    if(vec.size() == 2 && vec[0] == "PASS" && Clients.find(fd) != Clients.end() && Clients.size() != 1)
        msgToClient(fd, " 462 :You may not reregister\r\n");
    if(vec.size() == 2 && vec[0] == "PASS")
    {
        if(vec[1] == this->password)
        {
            this->authenFlag++;
            std::cout << GREEN2 << "Client (" << fd << ") Connected." << RESET << std::endl;
        }
        else
            msgToClient(fd, " 464 :Password incorrect\r\n");
    }
    if(this->authenFlag == 1)
    {
        msg = "\033[1;35mPlease enter your nickname:\033[0m\r\n";
        send(fd, msg.c_str(), msg.size(), 0);
    }
    vec.clear();
}
void Server::authentication(std::string buff, int index)
{
    std::istringstream ss(buff);
    std::istringstream ss2;
    std::string str;
    std::vector<std::string> vec;
    vec.clear();
    while(ss >> str)
        vec.push_back(str);
    if(vec.empty())
        return;
    if(this->authenFlag == 0)
        PassCommand(fds[index].fd, vec);
    else if(this->authenFlag == 1)
        NickCommand(fds[index].fd, vec);
    else if(this->authenFlag == 2)
        UserCommand(fds[index].fd, vec);
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
    this->authenFlag = 0;
    fds.push_back(this->npollfd);
    Clients[npollfd.fd].IpAddressSetter(inet_ntoa(cliaddress.sin_addr));
    std::string msg = "\033[1;35mPlease enter your password :\033[0m\r\n";
    send(npollfd.fd, msg.c_str(), msg.size(), 0);
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
        std::string msg;
        this->buff[rec] = '\0';
        std::cout << buff << std::endl;
        if(this->authenFlag < 3)
            authentication(this->buff, index);
        if(this->authenFlag == 3)
        {

            char hostname[256];
            if (gethostname(hostname, sizeof(hostname)) == -1) {
                std::cerr << "Error getting hostname\n";
            }
            msg = ":WEBSERV 001 " + Clients[fds[index].fd].nickNameGetter() + " :\033[0;33mWelcome to the WEBSERV Network, " + Clients[fds[index].fd].nickNameGetter() + "[!" + Clients[fds[index].fd].userNameGetter()+ "@" + hostname +"]\033[0m\r\n";
            send(fds[index].fd, msg.c_str(), msg.size(), 0);
            msg = ":WEBSERV 002 " + Clients[fds[index].fd].nickNameGetter() + " :\033[0;33mYour host is WEBSERV, running version 1.0\033[0m\r\n";
            send(fds[index].fd, msg.c_str(), msg.size(), 0);
            msg = ":WEBSERV 003 " + Clients[fds[index].fd].nickNameGetter() + " :\033[0;33mThis server was created " + this->timeStr + "\033[0m\r\n";
            send(fds[index].fd, msg.c_str(), msg.size(), 0);
            msg = ":WEBSERV 004 " + Clients[fds[index].fd].nickNameGetter() + " :\033[0;33mWEBSERV 1.0\033[0m\r\n";
            send(fds[index].fd, msg.c_str(), msg.size(), 0);
            this->authenFlag++;
        }
        if(this->authenFlag == 4)
        {
            std::vector<std::string> vec;
            std::stringstream ss(buff);
            std::string str;
            while(ss >> str)
            {
                if(str == "JOIN")
                {
                    while(ss>>str)
                    {
                        // std::cout<<str<<std::endl;
                        // std::map<std::string, Channel>::iterator it = Channels.find(str);
                        // // if(it == Channels.end())
                        Channels[str]= Channel(str);
                        std::map<int, Client>::iterator it2 = Clients.find(fds[index].fd);
                        std::cout<<"index 1: "<<it2->first<<std::endl;
                        std::cout<<"index  :"<<fds[index].fd<<" name: "<<Clients[fds[index].fd].nickNameGetter();
                        std::cout<<" "<<Clients[fds[index].fd].realNameGetter();
                        std::cout<<" "<<Clients[fds[index].fd].userNameGetter()<<std::endl;

                        Channels[str].join(it2->second, fds[index].fd);

                        // std::map<std::string, Channel>::iterator it3 ;
                        // for(it3= Channels.begin(); it3 != Channels.end(); it3++)
                        // {
                        //     std::map<int, Client>::iterator it5 = it3->second.Clients.find(fds[index].fd);
                        //     std::cout<<it3->first<<" ------ index inside channel clients "<<it5->first<<std::endl;
                        //     // std::map<int, Client>::iterator it5oper = it3->second.operators.find(fds[index].fd);
                        //     std::map<int, Client>::iterator it6 ;
                        //     for(it6 = it3->second.operators.begin(); it6 != it3->second.operators.end(); it6++)
                        //     {
                        //         std::map<int, Client>::iterator it5 = it3->second.operators.find(fds[index].fd);
                        //         std::cout<<it6->first<<" 888888888888 index inside channel  "<<it5->first<<std::endl;
                        //     }
                        // }
                        // std::map<int, Client>::iterator it4 ;
                        // for(it4= Clients.begin(); it4 != Clients.end(); it4++)
                        // {
                        //     std::cout<<it4->first<<" ***** "<<it4->second.nickName<<std::endl;
                        // }
                    }
                        vec.push_back(str);
                }
                else if (str == "JDM")
                {
                    Bot bot;
                    msg = "\033[0;35mJDMbot:\033[0;36m " + bot.getRandomFact() + "\033[0m\r\n";
                    send(fds[index].fd, msg.c_str(), msg.size(), 0);
                }
            }
            if(vec.empty())
                return;
            if(vec[0] == "PING" && vec.size() == 2)
            {
                msg = "PONG\r\n";
                send(fds[index].fd, msg.c_str(), msg.size(), 0);
            }
        }
        
    }
}
