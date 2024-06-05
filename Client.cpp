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