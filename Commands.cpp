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
                        std::cout<<""<<std::endl;
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
            input.erase(input.end()-1);
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
        ss >> str;
        if (str[0] == '#')
        {
            if (Channels.find(str) != Channels.end())
            {
                if (Channels[str].operators.find(fds[index].fd) == Channels[str].operators.end())
                {
                    std::ostringstream errorResponse;
                    errorResponse << ":WEBSERV 482 " << Clients[fds[index].fd].nickNameGetter() << " " << str << " :You're not channel operator\r\n";
                    send(fds[index].fd, errorResponse.str().c_str(), errorResponse.str().size(), 0);
                    return;
                }

                std::string targetUser;
                ss >> targetUser;

                bool userFound = false;
                std::map<int, Client>::iterator itClient;
                for (itClient = Channels[str].Clients.begin(); itClient != Channels[str].Clients.end(); itClient++)
                {
                    if (itClient->second.nickNameGetter() == targetUser)
                    {
                        userFound = true;

                        // Get the comment if any
                        std::string comment;
                        std::string commentSub;
                        std::getline(ss, comment);
                        commentSub = comment.substr(0, comment.size() - 1);
                        std::cout << "Comment: [" << comment << "]" << std::endl;
                        if (commentSub.empty())
                            comment = "No reason specified";
                        else if (commentSub[0] == ' ')
                                comment = commentSub.substr(1, comment.size() - 1);

                        // Notify the kicked user
                        std::ostringstream kickNotice;
                        kickNotice << ":" << Clients[fds[index].fd].nickNameGetter() << " KICK " << str << " " << targetUser << " :" << comment << "\r\n";
                        send(itClient->first, kickNotice.str().c_str(), kickNotice.str().size(), 0);

                        // Notify the channel about the kicked user
                        std::ostringstream channelNotice;
                        channelNotice << ":" << Clients[fds[index].fd].nickNameGetter() << " KICK " << str << " " << targetUser << " :" << comment << "\r\n";
                        std::map<int, Client>::iterator itNotify;
                        for (itNotify = Channels[str].Clients.begin(); itNotify != Channels[str].Clients.end(); itNotify++)
                        {
                            send(itNotify->first, channelNotice.str().c_str(), channelNotice.str().size(), 0);
                        }

                        Channels[str].Clients.erase(itClient);
                        break;
                    }
                }
                if (!userFound)
                {
                    std::ostringstream errMsg;
                    errMsg << ":WEBSERV 441 " << Clients[fds[index].fd].nickNameGetter() << " " << targetUser << " " << str << " :They aren't on that channel\r\n";
                    send(fds[index].fd, errMsg.str().c_str(), errMsg.str().size(), 0);
                    return;
                }
            }
            else
            {
                std::ostringstream errMsg;
                errMsg << ":WEBSERV 403 " << Clients[fds[index].fd].nickNameGetter() << " " << str << " :No such channel\r\n";
                send(fds[index].fd, errMsg.str().c_str(), errMsg.str().size(), 0);
                return;
            }
        }
        else
        {
            std::ostringstream errMsg;
            errMsg << ":WEBSERV 461 " << Clients[fds[index].fd].nickNameGetter() << " KICK :Not the right syntax of the command : KICK <#channel> <user> [:comment]\r\n";
            send(fds[index].fd, errMsg.str().c_str(), errMsg.str().size(), 0);
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
                std::string topicSub;
                std::getline(ss, topic);
                topicSub = topic.substr(0, topic.size() - 1);
                std::cout << "[" << topicSub << "]"<< std::endl;
                if (topicSub.empty() && Channels[str].topic.empty())
                {
                    std::string errMsg = ":WEBSERV 331 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No topic is set\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                    return;
                }
                else if (topicSub.empty())
                {
                    std::cout << "Topic: 1" << topicSub <<"1" <<std::endl;
                    std::cout << "1Topic: " << Channels[str].topic << std::endl;
                    std::string msg = ":WEBSERV 332 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :" + Channels[str].topic + "\r\n";
                    send(fds[index].fd, msg.c_str(), msg.size(), 0);

                    // Sending RPL_TOPIC to all users in the channel
                    std::ostringstream topicResponseAll;
                    topicResponseAll << ":WEBSERV 332 " << Clients[index].nickNameGetter() << " " << str << " :" << Channels[str].topic << "\r\n";
                    std::map<int, Client>::iterator it;
                    for (it = Channels[str].Clients.begin(); it != Channels[str].Clients.end(); ++it) {
                        send(it->first, topicResponseAll.str().c_str(), topicResponseAll.str().size(), 0);
                    }

                    return;
                }
                else
                {
                     std::cout << "--->Topic: 2" << topicSub <<"12" <<std::endl;

                    Channels[str].topic = topic.substr(1, topic.size() - 1);
                    std::cout << "2Topic:" << Channels[str].topic << std::endl;

                    // Sending RPL_TOPIC to all users in the channel
                    std::ostringstream topicResponseAll;
                    topicResponseAll << ":WEBSERV 332 " << Clients[index].nickNameGetter() << " " << str << " :" << Channels[str].topic << "\r\n";
                    std::map<int, Client>::iterator it;
                    for (it = Channels[str].Clients.begin(); it != Channels[str].Clients.end(); ++it) {
                        send(it->first, topicResponseAll.str().c_str(), topicResponseAll.str().size(), 0);
                    }

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
            std::string errMsg = ":WEBSERV 461 " + Clients[fds[index].fd].nickNameGetter() + " TOPIC :Not the right syntax of the command : TOPIC <#channel> <topic> \r\n";
            send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
            return;
        }
    }
    else if (str == "INVITE")
    {
        ss >> str;
        std::string targetChannel;
        if (!(ss >> targetChannel))
        {
            std::string errMsg = ":WEBSERV 461 " + Clients[fds[index].fd].nickNameGetter() + " INVITE :Not enough parameters\r\n";
            send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
            return;
        }
        if (targetChannel[0] == '#')
        {
            if (Channels.find(targetChannel) != Channels.end())
            {
                if ((Channels[targetChannel].operators.find(fds[index].fd) == Channels[targetChannel].operators.end()) && Channels[targetChannel].invite)
                {
                    std::string errMsg = ":WEBSERV 482 " + Clients[fds[index].fd].nickNameGetter() + " " + targetChannel + " :You're not channel operator\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
                    return;
                }

                if (Channels[targetChannel].Clients.find(fds[index].fd) == Channels[targetChannel].Clients.end())
                {
                    std::string errMsg = ":WEBSERV 442 " + Clients[fds[index].fd].nickNameGetter() + " " + targetChannel + " :You're not on that channel\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
                    return;
                }

                std::map<int, Client>::iterator it;
                for (it = Channels[targetChannel].inviteClients.begin(); it != Channels[targetChannel].inviteClients.end(); it++)
                {
                    if (it->second.nickNameGetter() == str)
                        return;
                }
                std::map<int, Client>::iterator itClient;
                bool userFound = false;
                for (itClient = Clients.begin(); itClient != Clients.end(); itClient++)
                {
                    if (itClient->second.nickNameGetter() == str)
                    {
                        userFound = true;
                        Channels[targetChannel].inviteClients[itClient->first] = itClient->second;

                        // Notify the invited user
                        std::ostringstream inviteNotice;
                        inviteNotice << ":" << Clients[fds[index].fd].nickNameGetter() << " INVITE " << str << " :" << targetChannel << "\r\n";
                        send(itClient->first, inviteNotice.str().c_str(), inviteNotice.str().size(), 0);

                        // Confirm the invitation to the inviter
                        std::ostringstream confirmInvite;
                        confirmInvite << ":WEBSERV 341 " << Clients[fds[index].fd].nickNameGetter() << " " << str << " " << targetChannel << "\r\n";
                        send(fds[index].fd, confirmInvite.str().c_str(), confirmInvite.str().size(), 0);

                        std::map<int, Client>::iterator it;
                        for (it = Channels[targetChannel].inviteClients.begin(); it != Channels[targetChannel].inviteClients.end(); it++)
                        {
                            std::cout << it->first << "*-*-* " << it->second.nickNameGetter() << std::endl;
                        }
                        return;
                    }
                }
                if (!userFound)
                {
                    std::string errMsg = ":WEBSERV 401 " + Clients[fds[index].fd].nickNameGetter() + " " + str + " :No such nick/channel\r\n";
                    send(fds[index].fd, errMsg.c_str(), errMsg.length(), 0);
                    return;
                }
            }
            else
            {
                std::string errMsg = ":WEBSERV 403 " + Clients[fds[index].fd].nickNameGetter() + " " + targetChannel + " :No such channel\r\n";
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
                std::string msg1;
                ss>>msg1;
                std::string msg;
                std::string line;
                // ss >> str;
                // std::cout<<str<<" haaaa "<<it8->first<<std::endl;
                if(msg1[0] == ':')
                {
                    msg1.erase(msg1.begin());
                    std::getline(ss, line);
                    line.erase(line.end() - 1);
                    msg = msg1 + line;
                    std::cout<<"here channel ;"<<msg<<std::endl;
                }
                else
                    msg = msg1;
                std::map<int, Client>::iterator IT1;
                std::cout<<"--->"<<Channels[str].Clients.begin()->second.nickNameGetter()<<std::endl;
                std::ostringstream response;
                response << ":" <<Clients[fds[index].fd].nickNameGetter() << " PRIVMSG " << str << " :" << msg << "\r\n";
                for(IT1 = Channels[str].Clients.begin(); IT1 != Channels[str].Clients.end(); IT1++)
                {
                    std::cout<<"--->"<<IT1->first<<std::endl;
                    if (IT1->first != fds[index].fd)
                        send(IT1->first, response.str().c_str(), response.str().size(), 0);
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
                    std::string recipient = str;
                    ss>>str;
                    std::string msg;
                    std::string line;
                    if(str[0] == ':')
                    {
                        str.erase(str.begin());
                        std::getline(ss, line);
                        line.erase(line.end() - 1);
                        msg = str + line;
                        std::cout<<"here user; "<<msg<<std::endl;
                    }
                    else
                        msg = str;        
                    std::ostringstream response;
                    response << ":" << Clients[fds[index].fd].nickNameGetter()<< " PRIVMSG " << recipient << " :" << msg << "\r\n";
                    send(it8->first, response.str().c_str(), response.str().size(), 0);
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
        std::string fact = bot.getRandomFact();
        std::string channel;
        ss >> channel;

        if (channel[0] == '#')
        {
            if (Channels.find(channel) != Channels.end())
            {
                std::string msg = "\033[0;35mJDMbot:\033[0;36m " + fact + "\033[0m\r\n";
                std::map<int, Client>::iterator it;
                for (it = Channels[channel].Clients.begin(); it != Channels[channel].Clients.end(); ++it)
                {
                    send(it->first, msg.c_str(), msg.size(), 0);
                }
            }
            else
            {
                std::string errMsg = ":WEBSERV 403 " + Clients[fds[index].fd].nickNameGetter() + " " + channel + " :No such channel\r\n";
                send(fds[index].fd, errMsg.c_str(), errMsg.size(), 0);
            }
        }
        else
        {
            std::string msg = "\033[0;35mJDMbot:\033[0;36m " + fact + "\033[0m\r\n";
            send(fds[index].fd, msg.c_str(), msg.size(), 0);
        }
    }
}