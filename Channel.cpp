#include"Channel.hpp"

// unsigned int	Channel::get_limit()const{}
// void	Channel::set_limit(unsigned int l){}
// std::string	Channel::get_topic()const{}
// void	Channel::set_topic(std::string Topic){}
Channel::Channel(){}
Channel::Channel(std::string Name)
{
    name = Name;
}
Channel::~Channel(){}
// void Channel::addOperand(Client &client, int index)
// {
//     Operator op;

//     op = client;
//     operators[index] = op;
// }