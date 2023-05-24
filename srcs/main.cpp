#include "Server.hpp"
#include <iostream>
#include <stdexcept>

int main(int ac, char** av)
{
	try
	{
		if (ac != 3)
			throw std::invalid_argument("Usage: ./ircserver <port> <password>");
		if (!av[2][0])
			throw std::invalid_argument("Invalid password\n");
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
