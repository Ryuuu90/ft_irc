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
            vect[0].push_back(input);
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
					if(!vect[1].empty() && k < vect[1].size())
					{
                        topic=vect[1][k];
                        k++;
                    }
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
                else
                    ;
                j++;
            }
        }
        else
            ;
        i++;
        // else if(modes[i][j] == '-')
        // {

        // }
    }

 std::cout<<"bool invite "<<invite<<" keypass "<<password<<" bool key "<<keyPass<<" limits "<<limits<<std::endl;
}


    
void Channel::join(Client &client, int index, std::vector<std::string> params, size_t &paramsIndex)
{
    if(invite == true)
    {
		std::cout<<"**"<<inviteCLients.find(index)->first<<std::endl;
		if(inviteCLients.find(index) == inviteCLients.end())
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
        Clients[index] = client;
	std::cout<<"hahowa fost channel "<<Clients[index].nickNameGetter()<<std::endl;
}