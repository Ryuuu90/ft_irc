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

void	Channel::set_topic(std::string Topic){
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
    if((Name[0] != '#' && Name[0] != '&')|| Name.size() < 2)
        throw(std::invalid_argument("Name does not match the requirement!!!"));
    else
    {
        size_t i = 1;
        while(i < Name.size())
        {
            if(std::isspace(Name[i]) || Name[i] == ',' || Name[i] == '\a')
                throw(std::invalid_argument("Name does not match the requirement!!!"));
            i++;
        }
    }
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
                throw(std::invalid_argument(""));
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
        // else return;
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
		// for(IT = Clients.begin(); IT != Clients.end(); IT ++)
		// {
		send(index,response.str().c_str(),response.str().size(), 0);
			//:mokhalil!mokha@example.com MODE #channel +i
		// }
		// 
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
    size_t i = 0;
    while (i < vect.size())
    {
        size_t l = 0;
        while(l < vect[i].size())
        {
            std::cout<<vect[i][l]<<std::endl;
            l++;
        }
        std::cout<<std::endl;
        i++;
    }
	i = 0;
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
					invite = true;
					response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" +i"<<"\r\n";
					// if();
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
                }
                else if(vect[0][i][j] == 't')
				{
					restrictionsTOPIC = true;
					response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" +t"<<"\r\n";
					// if();
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
				}
                else if(vect[0][i][j] == 'k')
                {
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        password = vect[1][k];
                    	keyPass = true;
						response.clear();
						response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    					response<<" MODE "<<this->name;
						response<<" +k "<<"\r\n";
						// if();
						for(IT = Clients.begin(); IT != Clients.end(); IT ++)
						{
							send(IT->first,response.str().c_str(),response.str().size(), 0);
							//:mokhalil!mokha@example.com MODE #channel +i
						}
                        k++;
                    }
					else
					{
						response.clear();
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
							response.clear();
							response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    						response<<" MODE "<<this->name;
							response<<" +o "<<it->second.nickNameGetter()<<"\r\n";
							// if();
							for(IT = Clients.begin(); IT != Clients.end(); IT ++)
							{
								send(IT->first,response.str().c_str(),response.str().size(), 0);
								//:mokhalil!mokha@example.com MODE #channel +i
							}
                            operators[it->first] = it->second;
                            opCount++;
                            k++;
                            break;
                        }
                    }
                    if (it == Clients.end())
                        exit(0);
                }
                else if(vect[0][i][j] == 'l')
                {
                    //liberachat
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        std::cout<<vect[1][k];
						std::stringstream digit(vect[1][k]);
                        // limits = std::atoi(vect[1][k].c_str());
						digit >>limits;
						if(digit.fail())
							return;
                        // if (limits <= 0);
                    	limit = true;
						response.clear();
						response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    					response<<" MODE "<<this->name;
						response<<" +l "<<limits<<"\r\n";
						// if();
						for(IT = Clients.begin(); IT != Clients.end(); IT ++)
						{
							send(IT->first,response.str().c_str(),response.str().size(), 0);
							//:mokhalil!mokha@example.com MODE #channel +i
						}
                        k++;
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
					response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -i"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
				}
                else if(vect[0][i][j] == 't')
				{
					response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -t"<<"\r\n";
					restrictionsTOPIC = false;
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
				}
                else if(vect[0][i][j] == 'k')
                {
                    keyPass = false;
                    password = "";
                    response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -k"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
                }
                else if(vect[0][i][j] == 'o')
                {
                    std::map<int, Client>::iterator it;
                    for (it = Clients.begin(); it != Clients.end() ; it++)
                    {
                        if (k < vect[1].size() && (it->second.nickNameGetter() == vect[1][k]))
                        {
                            operators.erase(it);
							response.clear();
							response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    						response<<" MODE "<<this->name;
							response<<" -o "<<it->second.nickNameGetter()<<"\r\n";
							for(IT = Clients.begin(); IT != Clients.end(); IT ++)
							{
								send(IT->first,response.str().c_str(),response.str().size(), 0);
								//:mokhalil!mokha@example.com MODE #channel +i
							}
                            opCount--;
                            k++;
                            break;
                        }
                    }
                    if (it == Clients.end())
                        throw(std::logic_error("invalid user name\r\n"));
                }
                else if(vect[0][i][j] == 'l')
                {
                    limit = false;
                    limits = -1;
					response.clear();
					response<<":"<<Clients[index].nickNameGetter()<<"!"<<Clients[index].userNameGetter()<<"@"<<getClientHostname(Clients[index].IpAddressGetter());
    				response<<" MODE "<<this->name;
					response<<" -l"<<"\r\n";
					for(IT = Clients.begin(); IT != Clients.end(); IT ++)
					{
						send(IT->first,response.str().c_str(),response.str().size(), 0);
						//:mokhalil!mokha@example.com MODE #channel +i
					}
                }
                j++;
            }

        }
        i++;
    }

 std::cout<<"bool invite "<<invite<<" keypass "<<password<<" bool key "<<keyPass<<" limits "<<limits<<std::endl;
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
    if (keyPass == true) {
		std::cout << "Index    -->"<<paramsIndex <<"---->"<<params.size()<< std::endl;
        // if (paramsIndex >= params.size()) {
        //     //:irc.example.com 696 Dave #example k very long and improper key with spaces :Invalid mode parameter
        //    joinError.clear();
        //    joinError<<":WEBSERV 696 "<<client.nickNameGetter()<<" "<<name<<" k undifined key :Invalid mode parameter\r\n";
        //    send(index, joinError.str().c_str(), joinError.str().size(), 0);
        //     return;
        // } 
        // if (check_join_params(params[paramsIndex]))
        // {
		// 	std::cout<<"1"<<params[paramsIndex]<<"1\n";
        //     joinError.clear();
        //     joinError<<":WEBSERV 696 "<<client.nickNameGetter()<<" "<<name<<" k key with spaces :Invalid mode parameter\r\n";
        //     send(index, joinError.str().c_str(), joinError.str().size(), 0);
        //     return;    
        // }
        if (paramsIndex >= params.size() || password.compare(params[paramsIndex])) {
			std::cout<<"inside channel "<<client.nickNameGetter()<<std::endl;
            joinError.clear();
            joinError<<":WEBSERV 475 "<<client.nickNameGetter()<<" "<<name<<" :Cannot join channel (+k) - bad key\r\n";
            send(index, joinError.str().c_str(), joinError.str().size(), 0);
            return;
        }
		std::cout<<"hada    -->"<<params[paramsIndex]<<std::endl;
    }
    
    if (Clients.find(index) == Clients.end())
    {
        if (Clients.empty()) 
	    {
            operators[index] = client;
	    	opCount++;
        }
        Clients[index] = client;

        // Get and set the client's hostname
        // client.hostname = getClientHostname(client.IpAddressGetter());

        // Notify all users in the channel about the new join
        std::ostringstream joinNotif;
        joinNotif << ":" << client.nickNameGetter() << "!" << client.userNameGetter() << "@" << client.hostname << " JOIN :" << name << "\r\n";
        std::map<int, Client>::iterator it;
        for (it = Clients.begin(); it != Clients.end(); ++it) {
            send(it->first, joinNotif.str().c_str(), joinNotif.str().size(), 0);
        }
        // Send the JOIN response to the new client
        // send(index, joinNotif.str().c_str(), joinNotif.str().size(), 0);

        // If a topic is set, send RPL_TOPIC (332)
        if (!topic.empty()) {
            std::ostringstream topicResponse;
			std::cout<<"----00 "<<topic<<"1"<<std::endl;
            topicResponse << ":WEBSERV 332 " << client.nickNameGetter() << " " << name << " :" << topic << "\r\n";
            send(index, topicResponse.str().c_str(), topicResponse.str().size(), 0);
        } 
        // RPL_NAMREPLY (353) - List of users in the channel
		// Kayn chi concept hnaya message ki trprinta check top dial terminal f blasst topic
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
            // if (operators.find(it->first) != operators.end())
            //     namesResponse<<"@";
            // namesResponse << it->second.nickNameGetter();
            // next = it;
            // next++;
            // if (next != Clients.end())
            //     namesResponse <<" ";
        }
        namesResponse << "\r\n";

        // RPL_ENDOFNAMES (366) - End of the list of users
        std::ostringstream endNamesResponse;
        endNamesResponse << ":WEBSERV 366 " << client.nickNameGetter() << " " << this->name << " :End of /NAMES list\r\n";
        // for (it = Clients.begin(); it != Clients.end(); ++it)
        // {
            send(index, namesResponse.str().c_str(), namesResponse.str().size(), 0);
            send(index, endNamesResponse.str().c_str(), endNamesResponse.str().size(), 0);
        // }

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