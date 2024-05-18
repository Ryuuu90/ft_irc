#ifndef BOT_HPP
#define BOT_HPP

#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>


class Bot
{
	private:
        std::vector<std::string> jdmFacts;
    public:
		std::string getRandomFact() const;
		Bot();
};

#endif