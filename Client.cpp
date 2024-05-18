#include "Client.hpp"

Client::Client()
{

}

Client::~Client()
{
}

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