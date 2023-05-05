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
#include <cerrno>
#include <netdb.h>

#include <map>
#include <vector>
#include "ClientInfo.hpp"
#include "codesDef.hpp"
#include <cstring>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

// void CmdCap(Server *server, ClientInfo *client, std::vector<std::string> arg);
// void CmdNick(Server *server, ClientInfo *client, std::vector<std::string> arg);
// void CmdUser(Server *server, ClientInfo *client, std::vector<std::string> arg);

class Server {

	public :

		Server( const std::string & port, const std::string & password );
		~Server( void );

		void launch( void );
		void generate_socket( void );
		void newClientConnect (sockaddr_in connect_sock, int connect_fd);

	private :

		const std::string _port;
		const std::string _password;
		int _sockfd;
		using func = void (Server::*)(ClientInfo *, std::vector<std::string>);
		std::map<std::string, func> _cmdsMap;
		std::map<int, ClientInfo *> _clientsMap;

		void parseMsg(ClientInfo *client, std::string message);
		ClientInfo*	getClient(const std::string &nickname);

		void CmdCap(ClientInfo *client, std::vector<std::string> arg);
		void CmdNick(ClientInfo *client, std::vector<std::string> arg);
		void CmdUser(ClientInfo *client, std::vector<std::string> arg);

};

#endif
