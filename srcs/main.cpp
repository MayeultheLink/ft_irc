#include "Server.hpp"
#include <iostream>
#include <stdexcept>

int main(int ac, char** av)
{
	try
	{
		if (ac != 3)
			throw std::runtime_error("Usage: ./ircserver <port> <password>");
		Server server(av[1], av[2]);
		server.launch();
		return 0;
	}
	catch (const std::exception & ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}
}
