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

#include <netdb.h>
#include <arpa/inet.h>

std::string getClientHostname(const std::string& ipAddress) {
    struct sockaddr_in sa;
    char host[1024];
    char service[20];

    sa.sin_family = AF_INET;
    inet_pton(AF_INET, ipAddress.c_str(), &sa.sin_addr);

    int result = getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), service, sizeof(service), 0);
    if (result != 0) {
        std::cerr << "Error getting client hostname: " << gai_strerror(result) << std::endl;
        return ipAddress; // Fallback to IP address if hostname cannot be resolved
    }

    return std::string(host);
}

void Channel::join(Client &client, int index, std::vector<std::string> params, size_t &paramsIndex) {
    if (invite == true) {
        if (inviteClients.find(index) == inviteClients.end()) {
            throw(std::invalid_argument("not invited\r\n"));
        }
    }
    if (limit == true) {
        if (limits == Clients.size()) {
            throw(std::invalid_argument("limits \r\n"));
        }
    }
    if (keyPass == true) {
        if (paramsIndex >= params.size()) {
            throw(std::invalid_argument("invalid password\r\n"));
        } else if (password.compare(params[paramsIndex])) {
            throw(std::invalid_argument("invalid password\r\n"));
        }
    }
    if (Clients.empty()) {
        operators[index] = client;
    }
    if (Clients.find(index) == Clients.end()) {
        Clients[index] = client;

        // Get and set the client's hostname
        client.hostname = getClientHostname(client.IpAddressGetter());

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
            topicResponse << ":WEBSERV 332 " << client.nickNameGetter() << " " << name << " :" << topic << "\r\n";
            send(index, topicResponse.str().c_str(), topicResponse.str().size(), 0);
        } else {
            // RPL_NOTOPIC (331) - No topic is set
            std::ostringstream noTopicResponse;
            noTopicResponse << ":WEBSERV 331 " << client.nickNameGetter() << " " << this->name << " :No topic is set\r\n";
            send(index, noTopicResponse.str().c_str(), noTopicResponse.str().size(), 0);
        }

        // RPL_NAMREPLY (353) - List of users in the channel
        std::ostringstream namesResponse;
        namesResponse << ":WEBSERV 353 " << client.nickNameGetter() << " = " << this->name << " :";
        for (it = Clients.begin(); it != Clients.end(); ++it) {
            namesResponse << it->second.nickNameGetter() << " ";
        }
        namesResponse << "\r\n";
        send(index, namesResponse.str().c_str(), namesResponse.str().size(), 0);

        // RPL_ENDOFNAMES (366) - End of the list of users
        std::ostringstream endNamesResponse;
        endNamesResponse << ":WEBSERV 366 " << client.nickNameGetter() << " " << this->name << " :End of /NAMES list\r\n";
        send(index, endNamesResponse.str().c_str(), endNamesResponse.str().size(), 0);
    }
}

    
