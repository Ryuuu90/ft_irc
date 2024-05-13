#include "Server.hpp"

int main(int ac, char **av)
{
    try
    {
        if(ac != 3)
            throw (std::runtime_error("Error: usage <port> <password>."));
        int port = std::atoi(av[1]);
        std::string password = av[2];
        Server server(port, password);
    }
    catch (std::exception &e)
    {
        std::cerr << RED << e.what() << RESET << std::endl;
    }
}