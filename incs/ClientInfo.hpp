/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 15:25:44 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/23 15:44:28 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENTINFO_HPP
# define CLIENTINFO_HPP

class ClientInfo;

#include <iostream>
#include <string>
#include <sstream>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Server.hpp"
#include "Channel.hpp"

class ClientInfo
{

	private:

		std::string			_hostname;
		const int			_fd;
		const int			_port;
		std::string			_realname;
		std::string			_username;
		std::string			_nickname;
		std::string			_password;
		bool				_isregistered;
		std::map<std::string, Channel*>	_channelsMap;
		std::string			_msg;

	public:

		ClientInfo(const std::string &hostname, int fd, sockaddr_in connect_sock);
		~ClientInfo();

		const std::string&		getHostname() const;
		const int&			getFd() const;
		const int&			getPort() const;
		const std::string&		getRealname() const;
		void				setRealname(const std::string &realname);
		const std::string&		getUsername() const;
		void				setUsername(const std::string &username);
		const std::string&		getNickname() const;
		void				setNickname(const std::string &nickname);
		const std::string&		getPassword() const;
		void				setPassword(const std::string &password);
		const bool&			getRegistered() const;
		void				setRegistered(const bool &isregistered);
		const std::string		getPrefix() const;
		std::string&			getMsg();

		std::map<std::string, Channel*> &getChannelsMap();

		void				reply(const std::string &reply);
		void 				sendMsg(const std::string &message) const;

};

#endif
