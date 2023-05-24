/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientInfo.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 15:48:41 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/24 14:47:34 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ClientInfo.hpp"

ClientInfo::ClientInfo(const std::string &hostname, int fd, sockaddr_in connect_sock)//int port)
	: _hostname(hostname), _fd(fd), _port(ntohs(connect_sock.sin_port)), _realname(""), _username(""), _nickname(""), _password(""), _isregistered(0), _msg("") 
{
	if (_hostname.size() > 63)
	{
		std::stringstream ss;
		ss << inet_ntoa(connect_sock.sin_addr);
		_hostname = ss.str();
	}

	return;
}

ClientInfo::~ClientInfo() {}

const std::string&			ClientInfo::getHostname() const	{return(_hostname);}
const int&				ClientInfo::getFd() const {return(_fd);}
const std::string&			ClientInfo::getRealname() const {return(this->_realname);}
void					ClientInfo::setRealname(const std::string &realname) {_realname = realname;}
const std::string&			ClientInfo::getUsername() const {return(this->_username);}
void					ClientInfo::setUsername(const std::string &username) {_username = username;}
const std::string&			ClientInfo::getNickname() const	{return(this->_nickname);}
void					ClientInfo::setNickname(const std::string &nickname) {_nickname = nickname;}
const std::string&			ClientInfo::getPassword() const	{return(this->_password);}
void					ClientInfo::setPassword(const std::string &password) {_password = password;}
const bool&				ClientInfo::getRegistered() const {return(this->_isregistered);}
void					ClientInfo::setRegistered(const bool &isregistered) {_isregistered = isregistered;}
std::map<std::string, Channel*>&	ClientInfo::getChannelsMap() {return(this->_channelsMap);}
std::string&				ClientInfo::getMsg() {return(this->_msg);}

void ClientInfo::sendMsg(const std::string &message) const 
{
	std::cout << "---> " << message << "\n" << std::endl;

	std::string buffer = message + "\r\n";
	if (send(_fd, buffer.c_str(), buffer.length(), 0) < 0)
		throw std::runtime_error("Error while sending message to client.");
}

const std::string ClientInfo::getPrefix() const 
{
	return (_nickname + "!" + _username + "@" + _hostname);
}

void ClientInfo::reply(const std::string &reply)
{
	sendMsg(":" + getPrefix() + " " + reply);
}
