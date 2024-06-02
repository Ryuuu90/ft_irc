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
    if(input.size() < 2)
        throw(std::invalid_argument("Not valide Argument!!\r\n"));
    if(operators.find(index) == operators.end())
        throw(std::invalid_argument("Not valide operator!!\r\n"));
    std::vector<std::vector<std::string> > vect;
    vect = split_input(input,vect);
    if(vect[0].empty())
        throw(std::logic_error(""));
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
                    invite = true;
                else if(vect[0][i][j] == 't')
				{
					restrictionsTOPIC = true;
					// if(!vect[1].empty() && k < vect[1].size())
					// {
                    //     topic=vect[1][k];
                    //     k++;
                    // }
				}
                else if(vect[0][i][j] == 'k')
                {
                    keyPass = true;
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        password = vect[1][k];
                        k++;
                    }
                }
                else if(vect[0][i][j] == 'o')
                {
                    std::map<int, Client>::iterator it;
                    for (it = Clients.begin(); it != Clients.end() ; it++)
                    {
                        if (k < vect[1].size() && (it->second.nickNameGetter() == vect[1][k]))
                        {
                            operators[it->first] = it->second;
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
                    limit = true;
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        std::cout<<vect[1][k];
                        limits = std::atoi(vect[1][k].c_str());
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
                    invite = false;
                else if(vect[0][i][j] == 't')
				{
					restrictionsTOPIC = false;
					// if(!vect[1].empty() && k < vect[1].size())
					// {
                    //     topic=vect[1][k];
                    //     k++;
                    // }
				}
                else if(vect[0][i][j] == 'k')
                {
                    keyPass = false;
                    password = "";
                    if(!vect[1].empty() && k < vect[1].size())
                    {
                        password = vect[1][k];
                        k++;
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
                            k++;
                            break;
                        }
                    }
                    if (it == Clients.end())
                        throw(std::logic_error(""));
                }
                else if(vect[0][i][j] == 'l')
                {
                    limit = false;
                    limits = -1;
                }
                j++;
            }

        }
        i++;
    }

 std::cout<<"bool invite "<<invite<<" keypass "<<password<<" bool key "<<keyPass<<" limits "<<limits<<std::endl;
}


    
void Channel::join(Client &client, int index, std::vector<std::string> params, size_t &paramsIndex)
{
    if(invite == true)
    {
        std::cout<<"index "<<index<<std::endl;
		// std::cout<<"**"<<inviteClients.find(index)->second.nickNameGetter()<<std::endl;
		if(inviteClients.find(index) == inviteClients.end())
		{
			throw(std::invalid_argument("not invited\r\n"));
		}
	}
	if(limit == true)
	{	if(limits == Clients.size())
			throw(std::invalid_argument("limits \r\n"));
    }
    if(keyPass == true)
    {
		std::cout<<"check password"<<std::endl;
        if(paramsIndex >= params.size())
        {
            throw(std::invalid_argument("anvalide pass word\r\n"));
			return;
        }
		else if(password.compare(params[paramsIndex]))
		{
			std::cout<<"---> invalide password"<<std::endl;
            throw(std::invalid_argument("anvalide pass word\r\n"));
			return;
		}
		std::cout<<"hada"<<std::endl;
	}
	if(Clients.empty())
        operators[index] = client;
    if(Clients.find(index) == Clients.end())
    {
        Clients[index] = client;
	    std::cout<<"hahowa fost channel "<<Clients[index].nickNameGetter()<<std::endl;
        std::ostringstream response;
        response << ":" << Clients[index].nickNameGetter() << " JOIN " << name<<"\r\n";
        send(index, response.str().c_str(), response.str().size(), 0);
        if(!topic.empty())
        {
            std::cout<<"salam"<<std::endl; 

            std::ostringstream s;
            s<<":WEBSERVE 332 "<<Clients[index].nickNameGetter()<< " "<<name<<" :"<<topic<<"\r\n";
            send(index, s.str().c_str(), s.str().size(), 0);
        }

    }
}