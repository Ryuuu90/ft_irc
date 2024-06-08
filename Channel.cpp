#include"Channel.hpp"

unsigned int	Channel::get_limit()const
{
    return(limits);
}

void	Channel::set_limit(unsigned int l)
{
    limits = l;
}

std::string	Channel::get_topic()const
{
    return(topic);
}

void	Channel::set_topic(std::string Topic)
{
    topic = Topic;
}

Channel::Channel(){}

Channel::Channel(std::string Name)
{
	invite = false;
	opCount = 0;
	limit = false;
	restrictionsTOPIC =false;
	keyPass =false;
	password = "";
    name = Name;
}
Channel::~Channel(){}

int check_modes(std::string mode)
{
    size_t i = 1;
    std::string str = "ioltk";
    while(i < mode.size())
    {
        if(str.find(mode[i]) == std::string::npos)
            return(0);
        i++;
    }
    return(1);
}

std::vector<std::vector<std::string> > &split_input(std::string input ,std::vector<std::vector<std::string> > &vect)
{
    std::stringstream ss(input);
    std::vector<std::string> modes; 
    std::vector<std::string> modesParameters;
    vect.push_back(modes);
    vect.push_back(modesParameters);
    while(ss >> input)
    {
        if(input[0] == '+' || input[0] == '-')
        {
            if(!check_modes(input))
            {
                vect.clear();
                return (vect);
            }
            vect[0].push_back(input);
        }
        else
            vect[1].push_back(input);
    }
    return(vect);
}



void Channel::mode(std::string input, int index)
{

    std::map<int,Client>::iterator IT;
	std::ostringstream response;
    if(input.empty())
	{
		// :<server_name> 324 <nick> <channel> <modes> <parameters>
		response<<":WEBSERV 324 "<<Clients[index].nickNameGetter()<<" ";
		response<<this->name<<" ";
        if (invite || limit || keyPass || restrictionsTOPIC || operators.find(index)!= operators.end())
            response <<"+";
		if(invite)
			response<<"i";
		if(keyPass)
			response<<"k";
		if(restrictionsTOPIC)
			response <<"t";
		if(operators.find(index)!= operators.end())
		{
			response<< "o";
		}
		if(limit)
			response<<"l "<<limits;
		response<<"\r\n";
		send(index,response.str().c_str(),response.str().size(), 0);
        return;
	}
    if(operators.find(index) == operators.end())
    {
        response.clear();
        // :<server> 482 <client> <channel> :You're not channel operator
        response<<":WEBSERV 482"<<Clients[index].nickNameGetter()<<" ";
        response<<this->name<<" :You're not channel operator\r\n";
        send(index,response.str().c_str(),response.str().size(), 0);
        return;
    }
    std::vector<std::vector<std::string> > vect;
    vect = split_input(input,vect);
    if (vect.empty())
    {
        response.clear();
        response << ":WEBSERV 472 " << Clients[index].nickNameGetter() << " " << input << " :is unknown mode char to me\r\n";
        return;
    }
    size_t i = 0;
    size_t k = 0;
    while(i < vect[0].size())
    {
        size_t j = 0;
        if(vect[0][i][j] == '+')
        {
            j++;
            while(j < vect[0][i].size() && (vect[0][i][j] != '-' && vect[0][i][j] != '+'))
            {
                if(vect[0][i][j] == 'i')
                {
                    std::cout<<vect[0][i][j]<<std::endl;
					invite = true;
					std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" +i"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
                }
                else if(vect[0][i][j] == 't')
				{
					restrictionsTOPIC = true;
					std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" +t"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
				}
                else if(vect[0][i][j] == 'k')
                {
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        password = vect[1][k];
                    	keyPass = true;
						std::ostringstream response;
						response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    					response<<" MODE "<<this->name;
						response<<" +k "<<"\r\n";
						for(IT = Clients.begin(); IT != Clients.end(); IT ++)
						{
                            send(IT->first,response.str().c_str(),response.str().size(), 0);
						}
                        k++;
                    }
					else
					{
						std::ostringstream response;
						response<<":WEBSERV 696 "<<Clients[index].nickNameGetter()<<" "<<name<<" k undifined key :Invalid mode parameter\r\n";
						send(index,response.str().c_str(),response.str().size(), 0);
						return;
					}
                }
                else if(vect[0][i][j] == 'o')
                {
                    std::map<int, Client>::iterator it;
                    for (it = Clients.begin(); it != Clients.end() ; it++)
                    {
                        if (k < vect[1].size() && (it->second.nickNameGetter() == vect[1][k]))
                        {
							std::ostringstream response;
							response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    						response<<" MODE "<<this->name;
							response<<" +o "<<it->second.nickNameGetter()<<"\r\n";
							for(IT = Clients.begin(); IT != Clients.end(); IT ++)
							{
                               send(IT->first,response.str().c_str(),response.str().size(), 0);
							}
                            operators[it->first] = it->second;
                            opCount++;
                            k++;
                            // break;
                        }
                    }
                    if (it == Clients.end())
					{
						std::ostringstream response;
						response<<":WEBSERV 696 "<<Clients[index].nickNameGetter()<<" "<<name<<" o undifined user :Invalid mode parameter\r\n";
						send(index,response.str().c_str(),response.str().size(), 0);
						return;
					}
                }
                else if(vect[0][i][j] == 'l')
                {
                    //liberachat
                    if(!vect[1].empty() && k < vect[1].size())
                    {
						std::stringstream digit(vect[1][k]);
						digit >>limits;
                        if (limits <= 0 || digit.fail())
                        {
						    std::ostringstream response;
						    response<<":WEBSERV 696 "<<Clients[index].nickNameGetter()<<" "<<name<<" l bad limit :Invalid mode parameter\r\n";
						    send(index,response.str().c_str(),response.str().size(), 0);
						    return;
					    }
                    	limit = true;
						std::ostringstream response;
						response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    					response<<" MODE "<<this->name;
						response<<" +l "<<limits<<"\r\n";
						for(IT = Clients.begin(); IT != Clients.end(); IT ++)
						{
                            send(IT->first,response.str().c_str(),response.str().size(), 0);
						}
                        k++;
                    }
                    else
                    {
						std::ostringstream response;
						response<<":WEBSERV 696 "<<Clients[index].nickNameGetter()<<" "<<name<<" l undifined limit :Invalid mode parameter\r\n";
						send(index,response.str().c_str(),response.str().size(), 0);
						return;
					}
                }
                j++;
            }
        }
        else if(vect[0][i][j] == '-')
        {
            j++;
            while(j < vect[0][i].size() && (vect[0][i][j] != '-' && vect[0][i][j] != '+'))
            {
                if(vect[0][i][j] == 'i')
				{
					invite = false;
					std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -i"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
				}
                else if(vect[0][i][j] == 't')
				{
					std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -t"<<"\r\n";
					restrictionsTOPIC = false;
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
				}
                else if(vect[0][i][j] == 'k')
                {
                    keyPass = false;
                    password = "";
                    std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -k"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
                }
                else if(vect[0][i][j] == 'o')
                {
                    std::map<int, Client>::iterator it;
                    for (it = operators.begin(); it != operators.end() ; it++)
                    {
                        if (k < vect[1].size() && (it->second.nickNameGetter() == vect[1][k]) && operators.size() > 1)
                        {
                            operators.erase(it);
							std::ostringstream response;
							response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    						response<<" MODE "<<this->name;
							response<<" -o "<<it->second.nickNameGetter()<<"\r\n";
							for(IT = Clients.begin(); IT != Clients.end(); IT ++)
							{
                                send(IT->first,response.str().c_str(),response.str().size(), 0);
							}
                            opCount--;
                            k++;
                            break;
                        }
                    }
                    if (it == Clients.end())
                    {
						std::ostringstream response;
						response<<":WEBSERV 696 "<<Clients[index].nickNameGetter()<<" "<<name<<" o undifined user :Invalid mode parameter\r\n";
						send(index,response.str().c_str(),response.str().size(), 0);
						return;
					}
                }
                else if(vect[0][i][j] == 'l')
                {
                    limit = false;
                    limits = -1;
					std::ostringstream response;
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -l"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
                        send(IT->first,response.str().c_str(),response.str().size(), 0);
					}
                }
                j++;
            }

        }
        i++;
    }

}

int check_join_params(std::string params)
{
    size_t j;
        j = 0;
        while(j < params.size())
        {
            if (std::isspace(params[j]))
                return (0);
            j++;
        }
  
    return(1);
}

void Channel::join(Client &client, int index, std::vector<std::string> params, size_t &paramsIndex)
{
	std::ostringstream joinError;
    if (invite == true) {
        if (inviteClients.find(index) == inviteClients.end()) {     
        joinError.clear();
        //:irc.example.com 473 Alice #privatechat :Cannot join channel (+i)
        joinError<<":WEBSERV 473 "<<client.nickNameGetter()<<" "<<name<<" :Cannot join channel (+i)\r\n";
        send(index, joinError.str().c_str(), joinError.str().size(), 0);
        return;
        }
    }
    if (limit == true) {
        if (limits == Clients.size()) {
            //:server_name 471 Bob #popular :Cannot join channel (+l)
            joinError.clear();
            joinError<<":WEBSERV 471 "<<client.nickNameGetter()<<" "<<name<<" :Cannot join channel (+l)\r\n";
            send(index, joinError.str().c_str(), joinError.str().size(), 0);
            return;
        }
    }
    if (keyPass == true)
    {
        if (paramsIndex >= params.size() || password.compare(params[paramsIndex])) {
            joinError.clear();
            joinError<<":WEBSERV 475 "<<client.nickNameGetter()<<" "<<name<<" :Cannot join channel (+k) - bad key\r\n";
            send(index, joinError.str().c_str(), joinError.str().size(), 0);
            return;
        }
    }
    
    if (Clients.find(index) == Clients.end())
    {
        if (Clients.empty()) 
	    {
            operators[index] = client;
	    	opCount++;
        }
        Clients[index] = client;
        // Notify all users in the channel about the new join
        std::ostringstream joinNotif;
        joinNotif << ":" << client.nickNameGetter() << "!" << client.userNameGetter() << "@" << client.hostname << " JOIN :" << name << "\r\n";
        std::map<int, Client>::iterator it;
        for (it = Clients.begin(); it != Clients.end(); ++it) {
            send(it->first, joinNotif.str().c_str(), joinNotif.str().size(), 0);
        }

        // If a topic is set, send RPL_TOPIC (332)
        if (!topic.empty()) {
            std::ostringstream topicResponse;
            topicResponse << ":WEBSERV 332 " << client.nickNameGetter() << " " << name << " :" << topic << "\r\n";
            send(index, topicResponse.str().c_str(), topicResponse.str().size(), 0);
        } 
        // RPL_NAMREPLY (353) - List of users in the channel
        std::ostringstream namesResponse;
        namesResponse << ":WEBSERV 353 " << client.nickNameGetter() << " = " << this->name << " :";
        std::map<int,Client>::iterator next;
        for (it = Clients.begin(); it != Clients.end(); ++it)
        {
            if (operators.find(it->first) != operators.end()) {
                namesResponse << "@" << it->second.nickNameGetter();
            } else {
                namesResponse << it->second.nickNameGetter();
            }
            next = it;
            next++;
            if (next != Clients.end()) {
                namesResponse << " ";
            }
        }
        namesResponse << "\r\n";

        // RPL_ENDOFNAMES (366) - End of the list of users
        std::ostringstream endNamesResponse;
        endNamesResponse << ":WEBSERV 366 " << client.nickNameGetter() << " " << this->name << " :End of /NAMES list\r\n";
            send(index, namesResponse.str().c_str(), namesResponse.str().size(), 0);
            send(index, endNamesResponse.str().c_str(), endNamesResponse.str().size(), 0);
		// If the client is an operator, send the mode change notification
        if (operators.find(index) != operators.end()) {
            std::ostringstream modeChangeResponse;
            modeChangeResponse << ":" << client.nickNameGetter() << "!" << client.userNameGetter() << "@" << client.hostname << " MODE " << name << " +o " << client.nickNameGetter() << "\r\n";
            for (it = Clients.begin(); it != Clients.end(); ++it) {
                send(it->first, modeChangeResponse.str().c_str(), modeChangeResponse.str().size(), 0);
            }
        }

		// Notify all users about the operator count update
        updateUserCount();

    }

}

    
void Channel::updateUserCount() {
    std::ostringstream userCountResponse;
    userCountResponse << ":WEBSERV NOTICE " << name << " :Total of " << Clients.size() << " nicks ["
                      << opCount << " ops, 0 halfops, 0 voices, "
                      << Clients.size() - opCount << " normal]\r\n";
    std::map<int, Client>::iterator it;
    for (it = Clients.begin(); it != Clients.end(); ++it) {
        send(it->first, userCountResponse.str().c_str(), userCountResponse.str().size(), 0);
    }
}