/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/19 15:50:43 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/23 17:29:51 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

bool running;

Server::Server( const std::string & port, const std::string & password ) : _port(port), _password(password)
{
	_cmdsMap["PASS"] = & Server::CmdPassword;
	_cmdsMap["CAP"] = & Server::CmdCap;
	_cmdsMap["NICK"] = & Server::CmdNick;
	_cmdsMap["USER"] = & Server::CmdUser;
	_cmdsMap["PING"] = & Server::CmdPing;
	_cmdsMap["JOIN"] = & Server::CmdJoin;
	_cmdsMap["PRIVMSG"] = & Server::CmdPrivmsg;
	_cmdsMap["NOTICE"] = & Server::CmdNotice;
	_cmdsMap["WHO"] = & Server::CmdWho;
	_cmdsMap["PART"] = & Server::CmdPart;
	_cmdsMap["KICK"] = & Server::CmdKick;
	_cmdsMap["INVITE"] = & Server::CmdInvite;
	_cmdsMap["TOPIC"] = & Server::CmdTopic;
	_cmdsMap["MODE"] = & Server::CmdMode;
	_cmdsMap["QUIT"] = & Server::CmdQuit;

	running = true;
}

Server::~Server( void )
{
	if (_clientsMap.size())
	{
		for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); ++it)
		{
			close(it->first);
			delete it->second;
		}
		_clientsMap.clear();
	}
	if (_channelsMap.size())
	{
		for (std::map<std::string, Channel *>::iterator it = _channelsMap.begin(); it != _channelsMap.end(); ++it)
			delete it->second;
	}
	_channelsMap.clear();
}

void handleSignal(int sigint)
{
	std::cout << std::endl;
	std::cout << "Exiting server..." << std::endl;
	if (sigint == SIGINT)
		running = false;
}

void Server::generate_socket( void )
{

	if ((_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
		throw std::runtime_error("Error while generating socket\n");

	if (fcntl(_sockfd, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error while setting socket to non blocking mode\n");

	struct sockaddr_in serv_socket = {AF_INET, 0, {0}, {0}};
	serv_socket.sin_addr.s_addr = htonl(INADDR_ANY);
	int port = std::strtol(_port.c_str(), NULL, 10);
	if (port < 0 || port > 65535)
		throw std::invalid_argument("Invalid port number\n");
	serv_socket.sin_port = htons(port);

	if (bind(_sockfd, (struct sockaddr*) & serv_socket, sizeof(serv_socket)) < 0)
		throw std::runtime_error("Error while binding socket\n");

	if (listen(_sockfd, 1000) < 0)
		throw std::runtime_error("Error while listening on socket\n");

	std::cout << "Server socket has been generated\n";
	
}

void Server::launch( void )
{
std::cout << "START\n";

	signal(SIGINT, &handleSignal);

	generate_socket();
	
	epoll_fd = epoll_create1(0);
	if (epoll_fd < 0)
		throw std::runtime_error("Error while creating epoll file descriptor\n");

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event)) {
		close(epoll_fd);
		throw std::runtime_error("Error while adding file descriptor to epoll\n");
	}

	int event_count;
	struct epoll_event events[MAX_EVENTS];
	
	while (running) 
	{
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (!running)
			break;
		for (int i = 0; i < event_count; i++)
		{
			if (events[i].data.fd == _sockfd)
			{
				int connect_fd;
				sockaddr_in connect_sock = {AF_INET, 0, {0}, {0}};
				socklen_t size = sizeof connect_sock;
				connect_fd = accept(_sockfd, (struct sockaddr*) &connect_sock, &size);
				if (connect_fd < 0)
				{
					if (errno == EINTR)
						continue;
					throw std::runtime_error("Error while accepting a connection\n");
				}
				event.data.fd = connect_fd;
				event.events = EPOLLIN;
				if (!epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect_fd, &event))
				{
std::cout << "ClientInfo n " << event.data.fd << " connected" << std::endl;
					newClientConnect(connect_sock, connect_fd);	
					break ;
				}
			}
			else
			{

				char tmp[100] = {0};
				int r = recv(events[i].data.fd, tmp, 100, 0);
std::cout << "recv = '" << tmp << "'" << std::endl;
				if (r < 0)
				{
					perror("Error while receiving data.");
					break;
				}
				else if (r == 0)
				{
					clientDisconnect(events[i].data.fd, epoll_fd);
					continue;
				}
				_clientsMap[events[i].data.fd]->getMsg().append(tmp, r);
				if (_clientsMap[events[i].data.fd]->getMsg().find("\r\n") == std::string::npos && _clientsMap[events[i].data.fd]->getMsg().find("\n") == std::string::npos)
					continue;
				execMsg(_clientsMap[events[i].data.fd], _clientsMap[events[i].data.fd]->getMsg());
				if (_clientsMap.find(events[i].data.fd) != _clientsMap.end())
					_clientsMap[events[i].data.fd]->getMsg() = "";
			}
		}
	}

	if (close(epoll_fd))
		throw std::runtime_error("Failed to close file descriptor");
	if (close(_sockfd))
		throw std::runtime_error("Failed to close file descriptor");
	
}

void Server::newClientConnect (sockaddr_in connect_sock, int connect_fd)
{
	char hostname[100];
	if (getnameinfo((struct sockaddr *) &connect_sock, sizeof(connect_sock), hostname, 100, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("Error while getting hostname on new client.");

std::cout << "CLIENT CONNECT: hostname = " << hostname << std::endl;

	ClientInfo *client = new ClientInfo(hostname, connect_fd, connect_sock);
	std::cout << hostname << ":" << ntohs(connect_sock.sin_port) << " has connected." << std::endl << std::endl;
	_clientsMap[connect_fd] = client;

	char tmp[100] = {0};
	int r = recv(connect_fd, tmp, 100, 0);
std::cout << "recv = '" << tmp << "'" << std::endl;
//std::cout << "message received = " << client->getMsg();
	if (r < 0)
	{
		perror("Error while receiving data.");
		return;
	}
	else if (r == 0)
		return;
	client->getMsg().append(tmp, r);
	if (client->getMsg().find("\r\n") == std::string::npos && client->getMsg().find("\n") == std::string::npos)
		return;
	execMsg(client, client->getMsg());
	if (_clientsMap.find(connect_fd) != _clientsMap.end())
	{
		client->getMsg() = "";
std::cout << "newco nickname: " << client->getNickname() << std::endl;
std::cout << "newco username: " << client->getUsername() << std::endl;
std::cout << "newco realname: " << client->getRealname() << std::endl;
	}
}

void Server::clientDisconnect(int fd, int epoll_fd)
{
std::cout << "CLIENT DISCONNECT" << std::endl;

	for (std::map<std::string, Channel*>::iterator it = _clientsMap[fd]->getChannelsMap().begin(); it != _clientsMap[fd]->getChannelsMap().end(); it++)
	{
		it->second->removeClient(_clientsMap[fd]);
		if (it->second->getNbClient() == 0)
		{
			_channelsMap.erase(it->second->getName());
			delete it->second;
		}
	}

	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
	delete _clientsMap.at(fd);
	_clientsMap.erase(fd);
	close(fd);
std::cout << "Client n°" << fd << " s'est déconnecté." << std::endl;
}

void Server::execMsg(ClientInfo *client, std::string message)
{
std::cout << "\nEXEC MSG : " << message;

	int fd = client->getFd();

	std::stringstream	ssMsg(message);
	std::string			msg_parse;

	while(_clientsMap.find(fd) != _clientsMap.end() && std::getline(ssMsg, msg_parse))
	{
		int len = msg_parse.length();
		if (msg_parse[len - 1] == '\r')
			msg_parse = msg_parse.substr(0, len - 1);

		std::string cmd_name = msg_parse.substr(0, msg_parse.find(' '));
		try
		{
			std::vector<std::string>	arguments;
			std::string 				buf;
			std::stringstream 			ssArg(msg_parse.substr(cmd_name.length(), msg_parse.length()));

			buf.clear();
			while (ssArg >> buf)
			{
				arguments.push_back(buf);
			}
			if (cmd_name != "CAP" && cmd_name != "PASS" && cmd_name != "USER" && cmd_name != "NICK" && client->getRegistered() != 1)
				client->reply(ERR_NOTREGISTERED(client->getNickname()));
			else if (_cmdsMap.find(cmd_name) != _cmdsMap.end())
				((this->*_cmdsMap[cmd_name]))(client, arguments);
		}
		catch (const std::out_of_range &e)
		{
			client->reply(ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_name));
		}
	}
}

ClientInfo* Server::getClientByNick(const std::string &nickname)
{
	for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); it++)
	{
		if (!nickname.compare(it->second->getNickname()))
			return (it->second);
	}
	return NULL;
}

void Server::createChannel(const std::string &name, const std::string &key, ClientInfo *client)
{
std::cout << "CREATING CHANNEL : " << name << " key = " << key << " by " << client->getNickname() << std::endl;
	Channel *channel = new Channel(name, key, client);
	if (key != "")
		channel->setKMode(true);
	client->getChannelsMap()[name] = channel;
	_channelsMap[name] = channel;
}







/* COMMANDS */







void Server::CmdCap(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : CAP" << std::endl;
	(void)client;
	(void)arg;
	return;
}

bool checkNickInvalid(std::string nickname)
{
	int i = 0;
	int length = nickname.length();

	if (length > 9)
	{
		std::cout << "NICK Error: Nickname is too big (>9)" << std::endl;
		return (false);
	}
	while (i < length)
	{
		if (!std::strchr(NICK_VALID_CHARS, nickname[i]))
		{
			std::cout << "NICK Error: Found special characters" << std::endl;
			return (false);
		}
		i++;
	}
	if (!isalpha(nickname[0]))
	{
		std::cout << "NICK Error: The nickname does not start with an alpha" << std::endl;
		return (false);
	}
	return true;
}

void Server::CmdNick(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : NICK" << std::endl;
	if (arg.size() < 1)
	{
		client->reply(ERR_NONICKNAMEGIVEN(client->getNickname(), "NICK"));
		return;
	}
	if (client->getPassword() != _password)
	{
		client->reply(ERR_PASSWDMISMATCH(client->getNickname()));
		clientDisconnect(client->getFd(), epoll_fd);
		return;
	}

	std::string reply = client->getNickname();
	std::string nickname = arg[0];
	if (checkNickInvalid(nickname) == false)
	{
		client->reply(ERR_ERRONEUSNICKNAME(client->getNickname()));
		return;
	}

	if (getClientByNick(nickname))
	{
		client->sendMsg(ERR_NICKNAMEINUSE(nickname));
		return;
	}
	std::string oldNick = client->getNickname();
	std::string newNick = nickname;
	client->setNickname(nickname);
	client->sendMsg(RPL_NICK(oldNick, newNick));

	if (!client->getRegistered() && client->getUsername() != "")
	{
		client->setRegistered(1);
		client->reply(RPL_WELCOME(client->getNickname()));
	}
}

void Server::CmdUser(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : USER" << std::endl;

	int param_size = arg.size();

	if (param_size < 3)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "USER"));
		return;
	}
	else if (client->getRegistered() == 1)
	{
		client->reply(ERR_ALREADYREGISTERED(client->getNickname()));
		return;
	}
	else if (client->getPassword() != _password)
	{
		client->reply(ERR_PASSWDMISMATCH(client->getNickname()));
		clientDisconnect(client->getFd(), epoll_fd);
		return;
	}
	else
	{
		std::string message;
		std::string realname;
		std::string username = arg[0];
		client->setUsername(username);
		for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it) 
		{
			message.append(" ");
			message.append(*it);
		}
		message.append("\t\n");
		size_t start;
		size_t end;
		start = message.find(":");
		if(message.find(":") == std::string::npos)
			realname = arg[2];
		else
		{
			end = message.find_first_of("\t\n", start);
			realname = message.substr(start + 1, end - start -1);
		}
		client->setRealname(realname);
		if (client->getNickname() != "")
		{
			client->setRegistered(1);
			client->reply(RPL_WELCOME(client->getNickname()));
		}
	}
}

void Server::CmdQuit(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : QUIT" << std::endl;
	std::string	message;
	if (arg.empty())
		message = "leaving";
	else
		message = (arg.at(0)).substr(1);
	client->sendMsg(RPL_QUIT(client->getPrefix(), message));
	clientDisconnect(client->getFd(), epoll_fd);
}

void Server::CmdPassword(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : PASS" << std::endl;
	int param_size = arg.size();
	if (param_size < 1)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Pass"));
		return;
	}
	else if (client->getRegistered() == 1)
	{
		client->reply(ERR_ALREADYREGISTERED(client->getNickname()));
		return;
	}
	else
		client->setPassword(arg[0]);
}

void Server::CmdPing(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : PING" << std::endl;
	std::string message = arg.at(0);
	client->sendMsg(RPL_PING(client->getPrefix(), message));
}

void Server::CmdJoin(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : JOIN" << std::endl;
	if (arg.size() == 1)
		arg.push_back("");
	if (arg[0].size() >= 1 && arg[0][0] != '#')
		arg[0].insert(0, "#");
	if (_channelsMap.find(arg[0]) == _channelsMap.end())
	{
		createChannel(arg[0], arg[1], client);
		client->getChannelsMap()[arg[0]] = _channelsMap[arg[0]];
		std::string users;
		for (std::vector<ClientInfo *>::const_iterator it = _channelsMap[arg[0]]->getClients().begin(); it != _channelsMap[arg[0]]->getClients().end(); ++it) 
		{
			if (_channelsMap[arg[0]]->isOperator(*it))
				users += "@";
			users += (*it)->getNickname() + " ";
		}
		client->reply(RPL_NAMREPLY(client->getNickname(), arg[0], users));
		client->reply(RPL_ENDOFNAMES(client->getNickname(), arg[0]));
		_channelsMap[arg[0]]->sendAll(RPL_JOIN(client->getPrefix(), arg[0]));
		if (_channelsMap[arg[0]]->getTopic() == "")
			client->reply(RPL_NOTOPIC(client->getNickname(), arg[0]));
		else
			client->reply(RPL_TOPIC(client->getPrefix(), arg[0], _channelsMap[arg[0]]->getTopic()));
		return;
	}
	if (client->getChannelsMap().find(arg[0]) != client->getChannelsMap().end())
		return;
	if (_channelsMap[arg[0]]->getIMode() && !_channelsMap[arg[0]]->isInvited(client))
	{
		client->reply(ERR_INVITEONLYCHAN(client->getNickname(), arg[0]));
		return;
	}
	if (_channelsMap[arg[0]]->getKMode() && arg[1] != _channelsMap[arg[0]]->getKey())
	{
		client->reply(ERR_BADCHANNELKEY(client->getNickname(), arg[0]));
		return;
	}
	if (_channelsMap[arg[0]]->getLMode() && _channelsMap[arg[0]]->getNbClient() >= _channelsMap[arg[0]]->getMaxClient())
	{
		client->reply(ERR_CHANNELISFULL(client->getNickname(), arg[0]));
		return;
	}
	_channelsMap[arg[0]]->addClient(client);
	client->getChannelsMap()[arg[0]] = _channelsMap[arg[0]];

	std::string users;
	for (std::vector<ClientInfo *>::const_iterator it = _channelsMap[arg[0]]->getClients().begin(); it != _channelsMap[arg[0]]->getClients().end(); ++it) 
	{
		if (_channelsMap[arg[0]]->isOperator(*it))
			users += "@";
		users += (*it)->getNickname() + " ";
	}
	client->reply(RPL_NAMREPLY(client->getNickname(), arg[0], users));
	client->reply(RPL_ENDOFNAMES(client->getNickname(), arg[0]));
	_channelsMap[arg[0]]->sendAll(RPL_JOIN(client->getPrefix(), arg[0]));
	if (_channelsMap[arg[0]]->getTopic() == "")
		client->reply(RPL_NOTOPIC(client->getNickname(), arg[0]));
	else
		client->reply(RPL_TOPIC(client->getPrefix(), arg[0], _channelsMap[arg[0]]->getTopic()));
}

void Server::CmdPrivmsg(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : PRIVMSG" << std::endl;
	if (arg.size() < 2 || arg[0].empty() || arg[1].empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "PRIVMSG"));
		return;
	}

	std::string	message;
	for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); it++)
		message.append(*it + " ");

	message = message.substr(message.find(':') + 1);

	std::string target = arg.at(0);
	if (target.at(0) == '#')
	{
		if (_channelsMap.find(target) == _channelsMap.end())
		{
			client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), target));
			return;
		}
		else if ((_channelsMap[target]->getKMode() && client->getChannelsMap().find(target) == client->getChannelsMap().end()) || (_channelsMap[target]->getIMode() && !_channelsMap[target]->isInvited(client)))
		{
			client->reply(ERR_CANNOTSENDTOCHAN(client->getNickname(), target));
			return;
		}
		_channelsMap[target]->sendAll(RPL_PRIVMSG(client->getPrefix(), target, message), client);
		return;
	}
	else if (getClientByNick(target))
		getClientByNick(target)->sendMsg(RPL_PRIVMSG(client->getPrefix(), target, message));
	else
		client->reply(ERR_NOSUCHNICK(client->getNickname(), target));
}

void Server::CmdNotice(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : NOTICE" << std::endl;
	if (arg.size() < 2 || arg[0].empty() || arg[1].empty())
		return;
	std::string	message;
	for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); it++)
		message.append(*it + " ");

	message = message.substr(message.find(':') + 1);

	std::string target = arg.at(0);
	if (target.at(0) == '#')
	{
		if (_channelsMap.find(target) == _channelsMap.end())
			return;
		else if ((_channelsMap[target]->getKMode() && client->getChannelsMap().find(target) == client->getChannelsMap().end()) || (_channelsMap[target]->getIMode() && !_channelsMap[target]->isInvited(client)))
			return;
		_channelsMap[target]->sendAll(RPL_NOTICE(client->getPrefix(), target, message), client);
		return;
	}
	else if (getClientByNick(target))
		getClientByNick(target)->sendMsg(RPL_NOTICE(client->getPrefix(), target, message));
}

void Server::CmdWho(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : WHO" << std::endl;
	if (arg.size())
		client->reply(RPL_ENDOFWHO(client->getPrefix(), arg[0]));
}

void Server::CmdPart(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : PART" << std::endl;
	if (arg.empty())
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "PART"));
		return;
	}

	std::vector<std::string>	chanVect;
	std::stringstream		ssChan(arg[0]);
	std::string			chan;

	while (std::getline(ssChan, chan, ','))
		chanVect.push_back(chan);

	for (std::vector<std::string>::iterator it = chanVect.begin(); it != chanVect.end(); it++)
	{
		std::string	reason = "";
		if (arg.size() == 2)
			reason = arg[1];
		if (_channelsMap.find(*it) == _channelsMap.end())
		{
			client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), *it));
			return;
		}
		if (client->getChannelsMap().find(*it) == client->getChannelsMap().end())
		{
			client->reply(ERR_NOTONCHANNEL(client->getNickname(), *it));
			return;
		}

		_channelsMap.find(*it)->second->sendAll(RPL_PART(client->getPrefix(), *it, reason));
		_channelsMap.find(*it)->second->removeClient(client);

		client->getChannelsMap().erase(*it);

		if (_channelsMap[*it]->getNbClient() == 0)
		{
			delete _channelsMap[*it];
			_channelsMap.erase(*it);
		}
	}
}

void Server::CmdKick(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : KICK" << std::endl;
	if (arg.size() < 2)
	{
		 client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Kick"));
		 return;
	}
	std::string chanName = arg.at(0);
	if (_channelsMap.find(chanName) == _channelsMap.end())
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (client->getChannelsMap().find(chanName) == client->getChannelsMap().end())
	{
		client->reply(ERR_NOTONCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (!_channelsMap[chanName]->isOperator(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickname(), chanName));
		return;
	}

	std::vector<std::string>	usersToKick;
	std::stringstream		ssUsersToKick(arg[1]);
	std::string			each_user;
	while (std::getline(ssUsersToKick, each_user, ','))
		usersToKick.push_back(each_user);

	for (std::vector<std::string>::iterator it = usersToKick.begin(); it != usersToKick.end(); it++)
	{
		std::string	comment = "";
		if (arg.size() == 3)
			comment = arg[2];
		ClientInfo*	clientToKick = getClientByNick(*it);

		if (clientToKick->getChannelsMap().find(chanName) == clientToKick->getChannelsMap().end())
			client->reply(ERR_USERNOTINCHANNEL(client->getNickname(), (*it), chanName));
		else
		{
			_channelsMap.find(chanName)->second->sendAll(RPL_KICK(client->getPrefix(), chanName, *it, comment));
			_channelsMap.find(chanName)->second->removeClient(clientToKick);

			clientToKick->getChannelsMap().erase(*it);

			if (_channelsMap[chanName]->getNbClient() == 0)
			{
				delete _channelsMap[chanName];
				_channelsMap.erase(chanName);
			}
		}
	}
}

void Server::CmdInvite(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : INVITE" << std::endl;
	
	if (arg.size() < 2)
	{
		 client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Invite"));
		 return;
	}
	std::string nickname = arg.at(0);
	std::string chanName = arg.at(1);
	if (_channelsMap.find(chanName) == _channelsMap.end())
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (client->getChannelsMap().find(chanName) == client->getChannelsMap().end())
	{
		client->reply(ERR_NOTONCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (_channelsMap[chanName]->getIMode() && !_channelsMap[chanName]->isOperator(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickname(), chanName));
		return;
	}
	if (!getClientByNick(nickname))
	{
		client->reply(ERR_NOSUCHNICK(client->getPrefix(), nickname));
		return;
	}
	_channelsMap[chanName]->getInvited().push_back(getClientByNick(nickname));
	client->reply(RPL_INVITING(client->getPrefix(), nickname, chanName));
	std::string invite = client->getNickname() + " has invited you to " + chanName;
	getClientByNick(nickname)->reply(invite);

}

void Server::CmdTopic(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : TOPIC" << std::endl;

	if (arg.size() < 1)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Topic"));
		return;
	}
	std::string chanName = arg[0];
	if (_channelsMap.find(chanName) == _channelsMap.end())
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (_channelsMap[chanName]->getTMode() && !_channelsMap[chanName]->isOperator(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickname(), chanName));
		return;
	}
	if (client->getChannelsMap().find(chanName) == client->getChannelsMap().end())
	{
		client->reply(ERR_NOTONCHANNEL(client->getPrefix(), chanName));
		return;
	}
	if (arg.size() == 1)
	{
		if (_channelsMap[chanName]->getTopic() == "")
			client->reply(RPL_NOTOPIC(client->getPrefix(), chanName));
		else
			client->reply(RPL_TOPIC(client->getPrefix(), chanName, _channelsMap[chanName]->getTopic()));
		return;
	}
	else
	{
		std::string topic = "";
		for (size_t i = 1; i < arg.size(); i++)
			topic.append(arg[i] + " ");
		topic.erase(--topic.end());
		_channelsMap[chanName]->setTopic(topic);
		_channelsMap[chanName]->sendAll(RPL_TOPIC(client->getPrefix(), chanName, topic));
	}
}

void Server::CmdMode(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : MODE" << std::endl;

	if (arg.size() < 1)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Mode"));
		return;
	}
	std::string chanName = arg[0];
	if (arg.size() == 1)
	{
		if (_channelsMap.find(chanName) == _channelsMap.end())
		{
			client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), chanName));
			return;
		}
		std::string modes = "";
		if (_channelsMap[chanName]->getIMode())
			modes += "i";
		if (_channelsMap[chanName]->getKMode())
			modes += "k";
		if (_channelsMap[chanName]->getLMode())
			modes += "l";
		if (_channelsMap[chanName]->getTMode())
			modes += "t";
		client->reply(RPL_CHANNELMODEIS(client->getPrefix(), chanName, modes, ""));
		return;
	}
	std::string modechar = arg[1];
	if (modechar[1] == 'i' && chanName == client->getNickname())
		return;
	if ((modechar[0] != '+' && modechar[0] != '-') || modechar.size() != 2)
	{
		client->reply("/mode <channel> [[+|-]modechar [parameter]]");
		return;
	}
	if ((modechar[1] == 'k' || modechar[1] == 'o' || (modechar[1] == 'l' && modechar[0] == '+')) && arg.size() < 3)
	{
		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "Mode"));
		return;
	}
	if (modechar[1] == 'i' && chanName == client->getNickname())
		return;
	if (_channelsMap.find(chanName) == _channelsMap.end())
	{
		client->reply(ERR_NOSUCHCHANNEL(client->getNickname(), chanName));
		return;
	}
	if (!_channelsMap[chanName]->isOperator(client))
	{
		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickname(), chanName));
		return;
	}
	if (modechar[1] == 'i')
	{
		if (modechar[0] == '+')
		{
			_channelsMap[chanName]->setIMode(true);
			_channelsMap[chanName]->setAllToInvited();
		}
		else if (modechar[0] == '-')
		{
			_channelsMap[chanName]->setIMode(false);
			_channelsMap[chanName]->getInvited().clear();
		}
	}
	else if (modechar[1] == 'o')
	{
		if (!getClientByNick(arg[2]))
		{
			client->reply(ERR_NOSUCHNICK(client->getPrefix(), arg[2]));
			return;
		}
		if (modechar[0] == '+')
		{
			if (getClientByNick(arg[2])->getChannelsMap().find(chanName) == getClientByNick(arg[2])->getChannelsMap().end())
			{
				client->reply(ERR_USERNOTINCHANNEL(client->getPrefix(), arg[2], chanName));
				return;
			}
			if (!_channelsMap[chanName]->isOperator(getClientByNick(arg[2])))
				_channelsMap[chanName]->getOperators().push_back(getClientByNick(arg[2]));
		}
		else if (modechar[0] == '-' && _channelsMap[chanName]->isOperator(getClientByNick(arg[2])))
			_channelsMap[chanName]->removeOperator(getClientByNick(arg[2]));
	}
	else if (modechar[1] == 'k')
	{
		if (modechar[0] == '+')
		{
			_channelsMap[chanName]->setKMode(true);
			_channelsMap[chanName]->setKey(arg[2]);
		}
		else if (modechar[0] == '-')
			_channelsMap[chanName]->setKMode(false);
	}
	else if (modechar[1] == 'l')
	{
		if (modechar[0] == '+')
		{
			for (std::string::iterator it = arg[2].begin(); it != arg[2].end(); it++)
				if (!std::isdigit(*it))
					return;
			_channelsMap[chanName]->setLMode(true);
			std::stringstream ss(arg[2]);
			size_t maxClient;
			ss >> maxClient;
			_channelsMap[chanName]->setMaxClient(maxClient);
		}
		else if (modechar[0] == '-')
			_channelsMap[chanName]->setLMode(false);
	}
	else if (modechar[1] == 't')
	{
		if (modechar[0] == '+')
			_channelsMap[chanName]->setTMode(true);
		else if (modechar[0] == '-')
			_channelsMap[chanName]->setTMode(false);
	}
	else
		client->reply(ERR_UNKNOWNMODE(client->getPrefix(), modechar[1]));
}
