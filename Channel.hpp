#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Client;
class Channel
{
	private:
		std::map<int , Client> superClients; 
        bool invite;
        bool keyPass;
        std::string password;
		std::string topic;
        unsigned int limits;

	public:
        std::map<int , Client> operators;
        std::map<int , Client> Clients;
        std::string name;
		Channel();
		Channel(std::string);
		~Channel();
		void join(Client &client, int index);
		unsigned int	get_limit()const;
        void	set_limit(unsigned int l);
		std::string	get_topic()const;
        void	set_topic(std::string Topic);
		void addOperand(Client &client, int index);
};
#endif