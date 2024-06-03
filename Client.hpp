#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp"

class Client
{
    private:
        std::string realName;
        std::string userName;
        std::string IpAddress;
    public:
        std::string hostname;
        bool operat;
        std::string nickName;
        Client();
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
