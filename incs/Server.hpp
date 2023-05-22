/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 15:43:43 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/22 15:34:20 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

extern bool running;

size_t getMsgFromFd(int fd, std::string * message);

class Server {

	private :

		const std::string								_port;
		const std::string								_password;
		int 										_sockfd;
		int										epoll_fd;
		std::map<std::string, void (Server::*)(ClientInfo *, std::vector<std::string>)>	_cmdsMap;
		std::map<int, ClientInfo *>							_clientsMap;
		std::map<std::string, Channel *>						_channelsMap;

		void generate_socket( void );

		ClientInfo* getClientByNick(const std::string &nickname);

		void newClientConnect (sockaddr_in connect_sock, int connect_fd);
		void clientMessage(int fd, char *tmp, size_t r);
		void execMsg(ClientInfo *client, std::string message);
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
		void CmdNotice(ClientInfo *client, std::vector<std::string> arg);
		void CmdWho(ClientInfo *client, std::vector<std::string> arg);
		void CmdPart(ClientInfo *client, std::vector<std::string> arg);
		void CmdKick(ClientInfo *client, std::vector<std::string> arg);
		void CmdInvite(ClientInfo *client, std::vector<std::string> arg);
		void CmdTopic(ClientInfo *client, std::vector<std::string> arg);
		void CmdMode(ClientInfo *client, std::vector<std::string> arg);

void debugPrints();

	public :

		Server( const std::string & port, const std::string & password );
		~Server( void );

		void launch( void );

};

#endif
