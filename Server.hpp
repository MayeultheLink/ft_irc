#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdexcept>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

class Server {

	public :

		Server( const std::string & port, const std::string & password );
		~Server( void );

		void launch( void );
		void generate_socket( void );

	private :

		const std::string _port;
		const std::string _password;
		int _sockfd;

};

#endif
