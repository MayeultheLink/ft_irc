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
	_cmdsMap["PART"] = & Server::CmdPart;
	_cmdsMap["KICK"] = & Server::CmdKick;
	_cmdsMap["INVITE"] = & Server::CmdInvite;
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

void Server::debugPrints()
{
	std::cout << "CHANNELS :\n";
	for (std::map<std::string, Channel *>::iterator it =  _channelsMap.begin(); it != _channelsMap.end(); it++)
	{
		std::cout << it->second->getName() << " : ";
		for (std::vector<ClientInfo *>::iterator it2 =  it->second->getClients().begin(); it2 != it->second->getClients().end(); it2++)
		{
			std::cout << (*it2)->getNickname() << ", ";
		}
		std::cout << std::endl;

	}

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
	// char buffer[BUFFER_SIZE + 1];
	// int bytes_read;
	struct epoll_event events[MAX_EVENTS];
	
	while (running) 
	{
std::cout << "entering running loop\n" << std::endl;
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (!running)
			break;
std::cout << "passed epoll_wait and event_count = " << event_count << std::endl;
		for (int i = 0; i < event_count; i++)
		{
std::cout << "entering for loop\n" << std::endl;
			if (events[i].data.fd == _sockfd)
			{
std::cout << "new client trying to connect\n" << std::endl;
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
			// else
			// {
			// 	printf("entering else condition\n");
			// 	bytes_read = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);
			// 	buffer[bytes_read] = 0;
			// 	std::string cast(buffer);
			// 	std::cout << cast << std::endl;
			// 	//send(_sockfd, buffer, strlen(buffer), 0);
			// 	//write(1, "\n", 1);
			// 	//printf("%s\n", buffer);
			// //	running = 0;
			// }
			else
			{
std::cout << "other fd communication" << std::endl;

				std::string message;
				int r = getMsgFromFd(events[i].data.fd, &message);
std::cout << "message received = '" << message << "'" << std::endl;
std::cout << "r = " << r << std::endl;
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
				else
					execMsg(_clientsMap[events[i].data.fd], message);


					// if (errno != EWOULDBLOCK)
						//throw std::runtime_error("Error while receiving message from client.");	
				// int fd = events[i].data.fd;
				// char buffer[1024];
				// bzero(buffer, 1024);
				// ssize_t n = recv(fd, buffer, sizeof buffer, 0);
				// if(n == -1)
				// {
				// 	perror("Error while receiving data.");
				// 	break;
				// }
				// else if(n == 0)
				// {
				// 	onClientDisconnect(fd, epoll_fd);
				// 	continue;
				// }
				// else
				// {
				// 	onClientMessage(fd, buffer, n);
				// 	//break;
				// }
			}
		}

		debugPrints();
	}

	if (close(epoll_fd))
		throw std::runtime_error("Failed to close file descriptor");
	
}

size_t getMsgFromFd(int fd, std::string * message)
{
	// std:: string message;
	char tmp[100] = {0};
	int r;
	while (message->find("\r\n") == std::string::npos && message->find("\n") == std::string::npos)
	{
		r = recv(fd, tmp, 100, 0);
		if (r < 0)
		{
			if (errno != EWOULDBLOCK)
				throw std::runtime_error("Error while receiving message from client.");			
		}
		if (r == 0)
			return r;
			
		message->append(tmp, r);

	}

	return r;
}

void	Server::newClientConnect (sockaddr_in connect_sock, int connect_fd)
{
	char hostname[100];
	if (getnameinfo((struct sockaddr *) &connect_sock, sizeof(connect_sock), hostname, 100, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("Error while getting hostname on new client.");

	std::cout << "CLIENT CONNECT: hostname = " << hostname << std::endl;

	// (void) connect_fd;

	ClientInfo *client = new ClientInfo(hostname, connect_fd, ntohs(connect_sock.sin_port));
	std::cout << hostname << ":" << ntohs(connect_sock.sin_port) << " has connected." << std::endl << std::endl;
	// _clientsMap.insert(std::make_pair(connect_fd, client)); 
	_clientsMap[connect_fd] = client;

	std:: string message;
	getMsgFromFd(connect_fd, &message);
//	char tmp[100] = {0};
//	while (message.find("\r\n") == std::string::npos)
//	{
//		int r = recv(connect_fd, tmp, 100, 0);
//		if (r < 0)
//		{
//			if (errno != EWOULDBLOCK)
//				throw std::runtime_error("Error while receiving message from client.");			
//		}
//		message.append(tmp, r);
//	}
std::cout << "message = " << message << std::endl;

	execMsg(client, message);

	// if (_errorpass == 0)
	// {
		// client->setRegistered(1);
//	client->reply(RPL_WELCOME(client->getNickname()));
		// std::cout << "password: " << client->getPassword() << std::endl;
std::cout << "newco nickname: " << client->getNickname() << std::endl;
std::cout << "newco username: " << client->getUsername() << std::endl;
std::cout << "newco realname: " << client->getRealname() << std::endl;
	// }
	// else
	// {
	// 	_errorpass = 0;
	// 	client->reply_command(RPL_ERROR(client->getNickname()));
	// 	onClientDisconnect(client->getFd(), getEpollfd());
	// }
}

// void	Server::clientMessage(int fd, char *tmp, size_t r)
// {
// 	try
// 	{
// 		std::string message = recvMessage(fd, tmp, r);
// 		std::cout << RED << "\nmessage = " << message << RESET;
// 		Client	*client = _clients.at(fd);
// 		_commandHandler->recup_msg(client, message);
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}
// }

void		Server::clientDisconnect(int fd, int epoll_fd)
{
std::cout << "CLIENT DISCONNECT" << std::endl;

	// Remove the client from the epoll instance (flag EPOLL_CTL_DEL)
	epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, 0);
	// Close the socket for this client
	std::cout << "Client n°" << fd << " va se déconnecter." << std::endl;
	delete _clientsMap.at(fd);
	_clientsMap.erase(fd);
	close(fd);
	// Log the client disconnection
	std::cout << "Client n°" << fd << " s'est déconnecté." << std::endl;
}

void	Server::execMsg(ClientInfo *client, std::string message)
{
std::cout << "EXEC MSG : " << message << std::endl;

	std::stringstream	ssMsg(message);
	std::string			msg_parse;

	while(std::getline(ssMsg, msg_parse))
	{
		int len = msg_parse.length();
		if (msg_parse[len - 1] == '\r')
			msg_parse = msg_parse.substr(0, len - 1);

		std::string cmd_name = msg_parse.substr(0, msg_parse.find(' '));
		try
		{
			// Command 					*command = _commands.at(cmd_name);
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
			// command->execute(client, arguments);
		}
		catch (const std::out_of_range &e)
		{
			client->reply(ERR_UNKNOWNCOMMAND(client->getNickname(), cmd_name));
		}
	}
}

ClientInfo*		Server::getClientByNick(const std::string &nickname)
{
	for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); it++)
	{
		if (!nickname.compare(it->second->getNickname()))
			return (it->second);
	}
	return NULL;
}

void	Server::createChannel(const std::string &name, const std::string &key, ClientInfo *client)
{
std::cout << "CREATING CHANNEL : " << name << " key = " << key << " by " << client->getNickname() << std::endl;
	Channel *channel = new Channel(name, key, client);
	// client->_channelsMap[name] = channel;
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

	//La taille du nick ne doit pas depasser 9 idealement
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

// syntax : /nick <new nick> -> 1 paramètre obligatoire !
void Server::CmdNick(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : NICK" << std::endl;
	if (arg.size() < 1)
	{
		client->reply(ERR_NONICKNAMEGIVEN(client->getNickname(), "NICK"));
		return;
	}

	std::string reply = client->getNickname();
	std::string nickname = arg[0];
	if (checkNickInvalid(nickname) == false)
	{
		client->reply(ERR_ERRONEUSNICKNAME(client->getNickname()));
		return;
	}
	// std::map<int, ClientInfo *> _clientsMap = getClients();

	for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); it++)
	{
		if (nickname == it.operator*().second->getNickname())
				nickname.push_back('_');
	}
	if (getClientByNick(nickname))
	{
		client->reply(ERR_NICKNAMEINUSE(client->getNickname()));
		return;
	}
	std::string oldNick = client->getNickname();
	std::string newNick = nickname;
	client->setNickname(nickname);
	reply.append(" changed his nickname to ");
	reply.append(client->getNickname()); 
	client->reply_command(reply);
	client->reply_command(RPL_NICK(oldNick, newNick));

}

void Server::CmdUser(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : USER" << std::endl;

		for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it)
		{
			std::cout << "boucle vector : " << *it << std::endl;
		}




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
	// 	_server->setErrorPass(1);
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
			// std::string tosend; 
			// tosend = (*it); 
			// message.append(" ");
			// message.append(tosend);

			message.append(" ");
			message.append(*it);
		}
		message.append("\t\n");
std::cout << message << std::endl;
		size_t start;
		size_t end;
		start = message.find(":");
		if(message.find(":") == std::string::npos)
		{
			realname = arg[2];
std::cout << "USER Cmd start var = " << start << std::endl;
		}
		else
		{
			end = message.find_first_of("\t\n", start);
			realname = message.substr(start + 1, end - start -1);
		}
std::cout << "USER Cmd Realname found = " << realname << std::endl;
		client->setRealname(realname);
		client->setRegistered(1);
		client->reply(RPL_WELCOME(client->getNickname()));
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
	client->reply(RPL_QUIT(client->getPrefix(), message));
	clientDisconnect(client->getFd(), epoll_fd);
//	std::vector<Channel *>	channels_userkill;
//	channels_userkill = client->getChannel();
//	for (std::vector<Channel *>::iterator it = channels_userkill.begin(); it != channels_userkill.end(); it++)
//	{
//		client->leave_channel((*it), leave, 1);
//		if ((*it)->getNbclients() == 0)
//			_server->destroyChannel(*it);
//	}

	//_server->~Server();
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
	{
	//	std::cout << RED << "ATTENTION: on est dans PassCommand, on set le password avant l'enregistrement!" << RESET << std::endl;
		//cette partie à utiliser si on enlève le parsing du début
		std::string Pass = arg[0];
	//	std::cout << Pass << std::endl;
		client->setPassword(Pass);
	}
//	if (client->getPassword() != "" && client->getPassword() != _server->getPassword())
//	{
//		client->reply(ERR_PASSWDMISMATCH(client->getNickname()));
//		//_server->onClientDisconnect(client->getFd(), _server->getEpollfd());
//		_server->setErrorPass(1);
//		return;
//	}
}

void Server::CmdPing(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : PING" << std::endl;
	std::string message = arg.at(0);
	client->reply_command(RPL_PING(client->getPrefix(), message));
}

void Server::CmdJoin(ClientInfo *client, std::vector<std::string> arg)
{
std::cout << "COMMAND : JOIN" << std::endl;
	if (arg.size() == 1)
		arg.push_back("");
	if (_channelsMap.find(arg[0]) == _channelsMap.end())
		createChannel(arg[0], arg[1], client);
	else if (_channelsMap[arg[0]]->getIMode() && !_channelsMap[arg[0]]->isInvited(client))
		client->reply(ERR_INVITEONLYCHAN(client->getNickname(), arg[0]));
	else if (client->getChannelsMap().find(arg[0]) == client->getChannelsMap().end())
	{
		_channelsMap[arg[0]]->addClient(client);
		client->getChannelsMap()[arg[0]] = _channelsMap[arg[0]];
	}
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
		//else if ([ban / chan with no external msg allowed])
		//{
		//	client->reply(ERR_CANNOTSENDTOCHAN(client->getNickname(), target));
		//	return;
		//}
		_channelsMap[target]->sendAll(RPL_PRIVMSG(client->getPrefix(), target, message), client);
		return;
	}
	else if (getClientByNick(target))
		getClientByNick(target)->writetosend(RPL_PRIVMSG(client->getPrefix(), target, message));
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
		//else if ([ban / chan with no external msg allowed])
		//	return;
		_channelsMap[target]->sendAll(RPL_NOTICE(client->getPrefix(), target, message), client);
		return;
	}
	else if (getClientByNick(target))
		getClientByNick(target)->writetosend(RPL_NOTICE(client->getPrefix(), target, message));
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
//		if (it->at(0) != '#')
//		{
//			std::string	tmp = *it;
//			*it = "#";
//			it.operator*().append(tmp);
//		}
		std::string	reason = "";
		if (arg.size() == 2)
			reason = arg[1];
//		Channel	*channel = getChannel(*it);
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
//	if (!_channelsMap[chanName]->isOperator(client))
//	{
//		client->reply(ERR_CHANOPRIVSNEEDED(client->getNickname(), chanName));
//		return;
//	}

	_channelsMap[chanName]->getInvited().push_back(getClientByNick(nickname));
	client->reply(RPL_INVITING(client->getPrefix(), nickname, chanName));
	std::string invite = client->getNickname() + " has invited you to " + chanName;
	getClientByNick(nickname)->reply(invite);

//	std::string msg = client->getPrefix() + " INVITE " + nickname + " " + chanName;
//	getClientByNick(nickname)->reply(msg);

}
