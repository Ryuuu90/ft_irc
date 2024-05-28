#include "Server.hpp"

bool parsePort(std::string portStr)
{
    std::string::iterator it;
    for(it = portStr.begin() ; it != portStr.end() ; it++)
    {
        if(!std::isdigit(*it))
            return false;
    }
    return true;
}

int main(int ac, char **av)
{
    try
    {
        if(ac != 3)
            throw (std::runtime_error("Error: usage <port> <password>."));
        if(!parsePort(av[1]))
            throw (std::runtime_error("Error: The port is a number."));
        int port = std::atoi(av[1]);
        std::string password = av[2];
        Server server(port, password);
    }
    catch (std::exception &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}