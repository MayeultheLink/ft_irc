#ifndef SERVER_HPP
# define SERVER_HPP

class Server;

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
#include <cstring>

#include "ClientInfo.hpp"
#include "Channel.hpp"
#include "codesDef.hpp"

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
		std::map<std::string, Channel *> _channelsMap;

		void execMsg(ClientInfo *client, std::string message);
		ClientInfo*	getClientByNick(const std::string &nickname);


		void generate_socket( void );

		void newClientConnect (sockaddr_in connect_sock, int connect_fd);
		void clientMessage(int fd, char *tmp, size_t r);
		void clientDisconnect(int fd, int epoll_fd);
		void createChannel(const std::string &name, const std::string &key, ClientInfo *client);

		void CmdCap(ClientInfo *client, std::vector<std::string> arg);
		void CmdNick(ClientInfo *client, std::vector<std::string> arg);
		void CmdUser(ClientInfo *client, std::vector<std::string> arg);
		void CmdQuit(ClientInfo *client, std::vector<std::string> arg);
		void CmdPassword(ClientInfo *client, std::vector<std::string> arg);
		void CmdPing(ClientInfo *client, std::vector<std::string> arg);
		void CmdJoin(ClientInfo *client, std::vector<std::string> arg);
		void CmdPrivmsg(ClientInfo *client, std::vector<std::string> arg);
		void CmdPart(ClientInfo *client, std::vector<std::string> arg);

};

#endif
