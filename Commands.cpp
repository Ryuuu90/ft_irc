#include "Channel.hpp"
#include "Server.hpp"

void Server::commands(std::string msg,std::vector<struct pollfd> fds, int index)
{
    std::vector<std::string> vec;
    std::stringstream ss2(buff);
    std::stringstream ss(buff);
    std::string str; 
    getline(ss2, str);
    if (str.find("PING") != std::string::npos)
    {
        std::cout << YELLOW << "here" << RESET << std::endl; 
        msg = "PONG\r\n";
        send(fds[index].fd, msg.c_str(), msg.size(), 0);
    }
    ss >> str;
    if(str == "JOIN")
    {
        {
            std::vector<std::vector<std::string> > params;
            std::string params_str;
            std::getline(ss,params_str);
            params = split_Channels(params_str,params);
            size_t j= 0;
            std::cout<<"channels :"<<std::endl;
            while(j < params.size())
            {
                size_t l = 0;
                while(l < params[j].size())
                {
                    std::cout<<params[j][l]<<std::endl;
                    l++;
                }
                j++;
                if(j == 1)
                    std::cout<<"params :"<<std::endl;
            }
            if(params[0].empty())
            {
                send(fds[index].fd,"bad join input !! \r\n",21,0);
                return;
            }
            else
            {
                size_t i = 0;
                size_t k = 0;
                while(i < params[0].size())
                {
                    std::map<std::string, Channel>::iterator it = Channels.find(params[0][i]);
                    if(it == Channels.end())
                    {
                        try
                        {
                            std::cout<<"test TEST"<<std::endl;
                            Channels[params[0][i]]= Channel(params[0][i]);
                        }
                        catch(std::exception &e)
                        {
                            std::cout<<e.what()<<std::endl;
                            return;
                        }
                    }
                    // std::map<int, Client>::iterator it2 = Clients.find(fds[index].fd);
                    std::string input;
                    std::getline(ss, input);
                    try
                    {
                        Channels[params[0][i]].join(Clients[fds[index].fd], fds[index].fd, params[1], k);
                        if(Channels[params[0][i]].keyPass)
                            k++;
                    }
                    catch(std::exception &e)
                    {
                        std::cout<<e.what()<<std::endl;
                    }
                    i++;
                }
            }
            params.clear();
        }
            
    }
    else if(str == "MODE")
    {
        ss>>str;
        std::map<std::string, Channel>::iterator it2 = Channels.find(str);
        if (it2 == Channels.end()){}
        else
        {
            std::string channelName = str;
            try
            {
                if(Channels.find(channelName) == Channels.end())
                    throw(std::invalid_argument(Clients[fds[index].fd].nickNameGetter() + " " + channelName + " :No such channel\r\n"));
            }
            catch(std::invalid_argument &e)
            {
                std::stringstream sserr;
                sserr << ERR_NOSUCHCHANNEL<<" "<< e.what();
                std::string errmsg = sserr.str();
                send(fds[index].fd, errmsg.c_str(),errmsg.size(),0);
            }
            std::string input;
            std::getline(ss,input);
            try
            {
                Channels[channelName].mode(input, fds[index].fd);
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            std::cout<<"bool invite "<<Channels[channelName].invite<<" keypass "<<Channels[channelName].password<<" limits "<<Channels[channelName].limits<<std::endl;
        }
    }
        else if (str == "KICK")
    {
        ss>>str;
        if(str[0] == '#')
        {
            if(Channels.find(str) != Channels.end())
            {

                if(Channels[str].operators.find(fds[index].fd) == Channels[str].operators.end())
                {
                    send(fds[index].fd, ":WEBSERV 482 userNick #channelName :You're not channel operator\r\n", 55, 0);
                    return;
                }

                int flag = 0;
                std::string str2;
                ss >> str2;

                std::map<int, Client>::iterator itClient;
                for(itClient = Channels[str].Clients.begin(); itClient != Channels[str].Clients.end(); itClient++)
                {
                    if(itClient->second.nickNameGetter() == str2)
                    {
                        flag = 1;
                        Channels[str].Clients.erase(itClient);
                        return;
                    }
                }
                if (!flag)
                {
                    std::string errMsg = ":WEBSERV 441 " + Clients[fds[index].fd].nickNameGetter() + " " + str2 + " " + str + " :They aren't on that channel\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);

                    return;
                }
                else
                {
                    std::string confirmMsg = ":" + Clients[fds[index].fd].nickNameGetter() + " KICK " + str + " " + str2 + " :User kicked\r\n";
                    send(fds[index].fd, confirmMsg.c_str(), confirmMsg.length(), 0);
                    return;
                }
            }
            else
            {
                std::string errMsg = ":WEBSERV 403 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No such channel\r\n";
                send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                return;
            }
        }
        else
        {
            send(fds[index].fd,"Not the right syntax of the command : KICK <#channel> <user> \r\n",64,0);
            return;
        }
    }
    else if (str == "TOPIC")
    {
        ss >> str;
        if(str[0] == '#')
        {
            if(Channels.find(str) != Channels.end())
            {
                if (Channels[str].operators.find(fds[index].fd) == Channels[str].operators.end() && Channels[str].restrictionsTOPIC)
                {
                    std::ostringstream errorResponse;

                    errorResponse << ":WEBSERV 482 " << Clients[index].nickNameGetter() << " " << Channels[str].name << " :You're not channel operator";
                    send(fds[index].fd, errorResponse.str().c_str(), errorResponse.str().size(), 0);

                    return;
                }
                if(Channels[str].Clients.find(fds[index].fd) == Channels[str].Clients.end())
                {
                    std::ostringstream errorResponse;

                    errorResponse << ":WEBSERV 442 " << Clients[index].nickNameGetter() << " " << Channels[str].name  << " :You're not on that channel";
                    send(fds[index].fd, errorResponse.str().c_str(), errorResponse.str().size(), 0);

                    return;
                }
                std::string topic;
                std::getline(ss, topic);
                if (topic.empty() && Channels[str].topic.empty())
                {
                    std::string errMsg = ":WEBSERV 331 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No topic is set\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                    return;
                }
                else if (topic.empty())
                {
                    std::string msg = ":WEBSERV 332 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :" + Channels[str].topic + "\r\n";
                    send(fds[index].fd, msg.c_str(), msg.size(), 0);
                    return;
                }
                else
                {
                    Channels[str].topic = topic.substr(1, topic.size() - 1);
                    std::cout << "Topic: " << Channels[str].topic << std::endl;
                }
            }
            else
            {
                std::string errMsg = ":WEBSERV 403 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No such channel\r\n";
                send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                return;
            }
        }
        else
        {
            std::string errMsg = ":WEBSERV 461 " + Clients[fds[index].fd].nickNameGetter() + " TOPIC :Not the right syntax of the command : TOPIC <#channel> <topic> \r\n";
            send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
            return;
        }

    }
    else if (str == "INVITE")
    {
        ss >> str;
        std::string str2;
        if (!(ss >> str2))
        {
            std::cout << "--------INVITE bool : " <<  Channels[str2].invite << std::endl;
            std::string errMsg = ":WEBSERV 461 " + Clients[fds[index].fd].nickNameGetter() + " INVITE :Not enough parameters\r\n";
            send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
            return;
        }
        if(str2[0] == '#')
        {
            if(Channels.find(str2) != Channels.end())
            {
                if (Channels[str2].operators.find(fds[index].fd) == Channels[str2].operators.end())
                {
                    std::string errMsg = ":WEBSERV 482 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :You're not channel operator\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
                    return;
                }
                // if (Channels[str2].invite)
                // {
                //     send(fds[index].fd,"You can't invite in this channel\r\n",34,0);
                //     return;
                // }
                
                if(Channels[str2].Clients.find(fds[index].fd) == Channels[str2].Clients.end())
                {
                    std::string errMsg = ":WEBSERV 442 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :You're not on that channel\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
                    return;
                }

                std::map<int, Client>::iterator itClient;
                for(itClient = Clients.begin(); itClient != Clients.end(); itClient++)
                {
                    if(itClient->second.nickNameGetter() == str)
                    {
                        Channels[str2].inviteClients[itClient->first] = itClient->second;

                        std::map<int, Client>::iterator it;
                        for(it = Channels[str2].inviteClients.begin(); it != Channels[str2].inviteClients.end(); it++)
                        {
                            std::cout << it->first << "*-*-* " << it->second.nickNameGetter() << std::endl;
                        }       
                        return;
                    }
                }
                std::string errMsg = ":WEBSERV 401 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No such nick/channel\r\n";
                send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                return;
            }
            else
            {
                std::string errMsg = ":WEBSERV 403 " + Clients[fds[index].fd].nickNameGetter() + " " + str2 + " :No such channel\r\n";
                send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                return;
            }
        }
        else
        {
            std::string errMsg = ":WEBSERV 461 " + Clients[fds[index].fd].nickNameGetter() + " INVITE :Not the right syntax of the command : INVITE <user> <#channel> \r\n";
            send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
            return;
        }

    }
    
    else if(str == "PRIVMSG")
    {
        ss>>str;
        std::cout<<"--->"<<str<<std::endl;
        if(str[0] == '#')
        {
            if(Channels.find(str) != Channels.end())
            {
                if(Channels[str].Clients.find(fds[index].fd) == Channels[str].Clients.end())
                {
                    send(fds[index].fd,"You're not part of this channel\r\n",34,0);
                    return;
                }
                std::cout<<"--->"<<"lqaha"<<std::endl;
                std::string line;
                std::getline(ss,line);
                std::cout<<"--->"<<str<<std::endl;
                std::string msg = line + "\r\n";
                std::map<int, Client>::iterator IT1;
                std::cout<<"--->"<<Channels[str].Clients.begin()->second.nickNameGetter()<<std::endl;
                for(IT1 = Channels[str].Clients.begin(); IT1 != Channels[str].Clients.end(); IT1++)
                {
                    std::cout<<"--->"<<IT1->first<<std::endl;
                    std::ostringstream response;
                    if(IT1->first != fds[index].fd)
                    {
                        response << ":" <<Clients[fds[index].fd].nickNameGetter() << " PRIVMSG " << str << " :" << msg << "\r\n";
                        send(IT1->first, response.str().c_str(), response.str().size(), 0);
                    }
                }
            }
        }
        else
        {
            std::map<int ,Client>::iterator it8;
            for(it8 = Clients.begin(); it8 != Clients.end(); it8++)
            {
                std::cout<<it8->second.nickNameGetter()<<" ";
                if (!it8->second.nickNameGetter().compare(str))
                {
                    std::string input;
                    std::getline(ss, str);
                    // ss >> str;
                    // std::cout<<str<<" haaaa "<<it8->first<<std::endl;
                    input = str.substr(1,str.size() - 1);
                    std::string mm = input + "\r\n";
                    send(it8->first, mm.c_str(), mm.size(), 0);
                    break;
                }
                
            }
            if(it8 == Clients.end())
                send(fds[index].fd,"destinataire unknown!!\r\n",25,0);
        }
    }
    else if (str == "JDM")
    {
        Bot bot;
        msg = "\033[0;35mJDMbot:\033[0;36m " + bot.getRandomFact() + "\033[0m\r\n";
        send(fds[index].fd, msg.c_str(), msg.size(), 0);
    }
}