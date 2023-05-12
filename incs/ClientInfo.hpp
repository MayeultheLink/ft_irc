/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 15:25:44 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/11 18:45:42 by mde-la-s         ###   ########.fr       */
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

#include "Server.hpp"
#include "Channel.hpp"

class ClientInfo
{
	public:

		ClientInfo(const std::string &hostname, int fd, int port);
		~ClientInfo();

		std::string			getHostname() const;
		int					getFd() const;
		int					getPort() const;
		std::string			getRealname() const;
		std::string			getUsername() const;
		std::string			getNickname() const;
		std::string			getPassword() const;
		// std::string			getModes() const;
		// bool				getOperator() const;
		bool				getRegistered() const;
		// std::string			getChannels() const;
		std::string			getPrefix() const;

		std::map<std::string, Channel*> &getChannelsMap();

		void				setRealname(const std::string &realname);
		void				setUsername(const std::string &username);
		void				setNickname(const std::string &nickname);
		void				setPassword(const std::string &password);
		// void				setModes(const std::string &modes);
		// void				setOperator(const bool &isoperator);
		void				setRegistered(const bool &isregistered);
		// void				setChannel(Channel *channel);

		// bool				findChannel(std::string chan_name);
		void				reply(const std::string &reply);
		void				reply_command(const std::string &reply);
		void 				writetosend(const std::string &message) const;
		// void				join_channel(Channel *channel);
		// void				leave_channel(Channel *channel, std::string message, bool kill);

	private:

		std::string				_hostname;
		int						_fd;
		int						_port;
		std::string				_realname;
		std::string				_username;
		std::string				_nickname;
		std::string				_password;
		// std::string 			_modes;
		// bool					_isoperator;
		bool					_isregistered;
		std::map<std::string, Channel*> _channelsMap;
		// std::vector<Channel *>	_channelsList;

};

#endif
