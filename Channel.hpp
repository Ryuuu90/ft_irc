#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Server.hpp"

class Client;
class Channel
{
	private:

	public:
		std::string topic;
        unsigned int limits;
		bool restrictionsTOPIC;
        std::string password;
		int opCount;
        bool keyPass;
		bool limit;
        bool invite;
		std::map<int , Client> inviteClients;
        std::map<int , Client> operators;
        std::map<int , Client> Clients;
		void updateUserCount();
        std::string name;
		Channel();
		Channel(std::string);
		~Channel();
		void join(Client &client, int index, std::vector<std::string> params, size_t &paramsIndex);
		void mode(std::string, int index);
		unsigned int	get_limit()const;
        void	set_limit(unsigned int l);
		std::string	get_topic()const;
        void	set_topic(std::string Topic);
		void addOperand(Client &client, int index);
};
std::vector<std::vector<std::string> > &split_Channels(std::string input, std::vector<std::vector<std::string> > &vect);
#endif