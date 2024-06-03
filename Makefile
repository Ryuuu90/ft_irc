NAME = Server
SRC = Client.cpp\
	Server.cpp\
	Channel.cpp\
	main.cpp\
	bot.cpp\
	Commands.cpp\

OSRC = $(SRC:.cpp=.o)
%.o : %.cpp
	@c++ -Wall -Wextra -Werror -std=c++98  -c $(SRC)
$(NAME) : $(OSRC)
	@c++ -fsanitize=address -g3 $(OSRC) -o $(NAME)
	@echo "\033[1;32mServer is ready to execute ✅\033[0m"
all : $(NAME)
clean :
	@rm -rf $(OSRC)
	@echo "\033[1;31mObject files deleted successfully 🗑\033[0m"
fclean : clean
	@rm -rf $(NAME)
	@echo "\033[1;31mExecution files deleted successfully 🗑\033[0m"
re : fclean all