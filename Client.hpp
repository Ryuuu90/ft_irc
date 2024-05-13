#pragma once

#include "Server.hpp"

class Client
{
    private:
        std::string nickName;
        std::string realName;
        std::string userName;
        std::string IpAddress;
    public:
        Client();
        ~Client();
        void nickNameSetter(std::string nName);
        void realNameSetter(std::string rName);
        void userNameSetter(std::string uName);
        void IpAddressSetter(std::string IPadd);
        std::string nickNameGetter() const;
        std::string realNameGetter() const;
        std::string userNameGetter() const;
        std::string IpAddressGetter() const;
};
