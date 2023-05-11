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
#include <signal.h>

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

extern bool running;

size_t getMsgFromFd(int fd, std::string * message);

class Server {

	public :

		Server( const std::string & port, const std::string & password );
		~Server( void );

		void launch( void );


	private :

		const std::string _port;
		const std::string _password;
		int _sockfd;
		int epoll_fd;
		std::map<std::string, void (Server::*)(ClientInfo *, std::vector<std::string>)> _cmdsMap;
		std::map<int, ClientInfo *> _clientsMap;

		void execMsg(ClientInfo *client, std::string message);
		ClientInfo*	getClientByNick(const std::string &nickname);


		void generate_socket( void );

		void newClientConnect (sockaddr_in connect_sock, int connect_fd);
		void clientMessage(int fd, char *tmp, size_t r);
		void clientDisconnect(int fd, int epoll_fd);


		void CmdCap(ClientInfo *client, std::vector<std::string> arg);
		void CmdNick(ClientInfo *client, std::vector<std::string> arg);
		void CmdUser(ClientInfo *client, std::vector<std::string> arg);
		void CmdQuit(ClientInfo *client, std::vector<std::string> arg);
		void CmdPassword(ClientInfo *client, std::vector<std::string> arg);

};

#endif
