#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Operator;
class Client
{
    private:
        std::string nickName;
        std::string realName;
        std::string userName;
        std::string IpAddress;
    public:
        Client();
        // void join(Channel &channel, int index);
        void nickNameSetter(std::string nName);
        void realNameSetter(std::string rName);
        void userNameSetter(std::string uName);
        void IpAddressSetter(std::string IPadd);
        std::string nickNameGetter() const;
        std::string realNameGetter() const;
        std::string userNameGetter() const;
        std::string IpAddressGetter() const;
        // Client& operator=(Client &client);
        virtual ~Client();

};

#endif