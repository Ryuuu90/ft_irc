#include "Client.hpp"

Client::Client()
{
    nickName = "aaaqwq";
    realName = "aaaqwq";
    userName = "aaaqwq";
    IpAddress = "qqqf";
}

Client::~Client()
{
}

// void Client::join(Channel &channel, int index)
// {
//     if(channel.Clients.empty())
//     {
//         channel.Clients[index] = *this;
//         channel.addOperand(*this, index);
//     }
//     else
//         channel.Clients[index] = *this;
    
// }
// Client& Client::operator=(Client &client)
// {
//     this->realNameSetter(client.nickNameGetter());
//     return(*this);
// }

void Client::nickNameSetter(std::string nName)
{
    this->nickName = nName;
}

void Client::realNameSetter(std::string rName)
{
    this->realName = rName;
}

void Client::userNameSetter(std::string uName)
{
    this->userName = uName;
}

void Client::IpAddressSetter(std::string IPadd)
{
    this->IpAddress = IPadd;
}

std::string Client::IpAddressGetter() const
{
    return(this->IpAddress);
}

std::string Client::userNameGetter() const
{
    return(this->userName);
}

std::string Client::realNameGetter() const
{
    return(this->realName);
}

std::string Client::nickNameGetter() const
{
    return(this->nickName);
}