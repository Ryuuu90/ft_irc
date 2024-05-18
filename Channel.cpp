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
    name = Name;
}
Channel::~Channel(){}


void Channel::join(Client &client, int index)
{
    if(Clients.empty())
        operators[index] = client;
    Clients[index] = client;
}