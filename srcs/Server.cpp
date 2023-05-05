#include "Server.hpp"

Server::Server( const std::string & port, const std::string & password ) : _port(port), _password(password)
{
	_cmdsMap["CAP"] = & Server::CmdCap;
	_cmdsMap["NICK"] = & Server::CmdNick;
	_cmdsMap["USER"] = & Server::CmdUser;
}

Server::~Server( void ) {}

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
	generate_socket();
	
	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0)
		throw std::runtime_error("Error while creating epoll file descriptor\n");

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event)) {
		close(epoll_fd);
		throw std::runtime_error("Error while adding file descriptor to epoll\n");
	}

	int running = 1;
	int event_count;
	char buffer[BUFFER_SIZE + 1];
	int bytes_read;
	struct epoll_event events[MAX_EVENTS];
	
	while (running) 
	{
		std::cout << "entering running loop\n" << std::endl;
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		std::cout << "passed epoll_wait\n" << std::endl;
		for (int i = 0; i < event_count; i++) {
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
			else
			{
				printf("entering else condition\n");
				bytes_read = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);
				buffer[bytes_read] = 0;
				std::string cast(buffer);
				std::cout << cast << std::endl;
				//send(_sockfd, buffer, strlen(buffer), 0);
				//write(1, "\n", 1);
				//printf("%s\n", buffer);
			//	running = 0;
			}
		}
	}

	if (close(epoll_fd))
		throw std::runtime_error("Failed to close file descriptor");
	
}

void	Server::newClientConnect (sockaddr_in connect_sock, int connect_fd)
{
	char hostname[100];
	if (getnameinfo((struct sockaddr *) &connect_sock, sizeof(connect_sock), hostname, 100, NULL, 0, NI_NUMERICSERV) != 0)
		throw std::runtime_error("Error while getting hostname on new client.");

	std::cout << "CLIENT CONNECT: hostname = " << hostname << std::endl;

	(void) connect_fd;

	ClientInfo *client = new ClientInfo(hostname, connect_fd, ntohs(connect_sock.sin_port));
	std::cout << hostname << ":" << ntohs(connect_sock.sin_port) << " has connected." << std::endl << std::endl;
	// _clientsMap.insert(std::make_pair(connect_fd, client)); 
	_clientsMap[connect_fd] = client;

	std:: string message;
	char tmp[100] = {0};
	while (message.find("\r\n") == std::string::npos)
	{
		int r = recv(connect_fd, tmp, 100, 0);
		if (r < 0)
		{
			if (errno != EWOULDBLOCK)
				throw std::runtime_error("Error while receiving message from client.");			
		}
		message.append(tmp, r);
	}
	std::cout << "message = " << message << std::endl;

	parseMsg(client, message);
	// if (_errorpass == 0)
	// {
	// 	client->setRegistered(1);
	// 	client->reply(RPL_WELCOME(client->getNickname()));
	// 	std::cout << "password: " << client->getPassword() << std::endl;
	// 	std::cout << "nickname: " << client->getNickname() << std::endl;
	// 	std::cout << "username: " << client->getUsername() << std::endl;
	// 	std::cout << "realname: " << client->getRealname() << std::endl;
	// 	std::cout << RESET;
	// }
	// else
	// {
	// 	_errorpass = 0;
	// 	client->reply_command(RPL_ERROR(client->getNickname()));
	// 	onClientDisconnect(client->getFd(), getEpollfd());
	// }
}

void	Server::parseMsg(ClientInfo *client, std::string message)
{
	std::stringstream	ssMsg(message);
	std::string			msg_parse;

	while(std::getline(ssMsg, msg_parse))
	{
		int len = msg_parse.length();
		if (msg_parse[len - 1] == '\r')
			msg_parse = msg_parse.substr(0, len - 1);

		std::string cde_name = msg_parse.substr(0, msg_parse.find(' '));
		try
		{
			// Command 					*command = _commands.at(cde_name);
			std::vector<std::string>	arguments;
			std::string 				buf;
			std::stringstream 			ssArg(msg_parse.substr(cde_name.length(), msg_parse.length()));

			buf.clear();
			while (ssArg >> buf)
			{
				arguments.push_back(buf);
			}

			((this->*_cmdsMap[cde_name]))(client, arguments);
			// command->execute(client, arguments);
		}
		catch (const std::out_of_range &e)
		{
			client->reply(ERR_UNKNOWNCOMMAND(client->getNickname(), cde_name));
		}
	}
}

ClientInfo*		Server::getClient(const std::string &nickname)
{
	for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); it++)
	{
		if (!nickname.compare(it->second->getNickname()))
			return (it->second);
	}
	return NULL;
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
	if (getClient(nickname))
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
	// else if (client->getPassword() != _server->getPassword())
	// {
	// 	client->reply(ERR_PASSWDMISMATCH(client->getNickname()));
	// 	//_server->onClientDisconnect(client->getFd(), _server->getEpollfd());
	// 	_server->setErrorPass(1);
	// 	return;
	// }
	else
	{
		std::string message;
		std::string realname;
		std::string username = arg[0];
		client->setUsername(username);
		for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it) 
		{
			std::string tosend; 
			tosend = (*it); 
			message.append(" ");
			message.append(tosend);
		}
		message.append("\t\n");
		std::cout << message << std::endl;
		size_t start;
		size_t end;
		start = message.find(":");
		if(message.find(":") == std::string::npos)
		{
			realname = arg[2];
			std::cout << start << std::endl;
		}
		else
		{
			end = message.find_first_of("\t\n", start);
			realname = message.substr(start + 1, end - start -1);
		}
		client->setRealname(realname);
		client->setRegistered(1);
	}

}

/*TEST*/

// void CmdCap(Server *server, ClientInfo *client, std::vector<std::string> arg)
// {
// std::cout << "COMMAND : CAP" << std::endl;
// 	(void)client;
// 	(void)arg;
// 	return;
// }

// bool checkNickInvalid(std::string nickname)
// {
// 	int i = 0;
// 	int length = nickname.length();

// 	//La taille du nick ne doit pas depasser 9 idealement
// 	if (length > 9)
// 	{
// 		std::cout << "NICK Error: Nickname is too big (>9)" << std::endl;
// 		return (false);
// 	}
// 	while (i < length)
// 	{
// 		if (!std::strchr(NICK_VALID_CHARS, nickname[i]))
// 		{
// 			std::cout << "NICK Error: Found special characters" << std::endl;
// 			return (false);
// 		}
// 		i++;
// 	}
// 	if (!isalpha(nickname[0]))
// 	{
// 		std::cout << "NICK Error: The nickname does not start with an alpha" << std::endl;
// 		return (false);
// 	}
// 	return true;
// }

// // syntax : /nick <new nick> -> 1 paramètre obligatoire !
// void CmdNick(Server *server, ClientInfo *client, std::vector<std::string> arg)
// {
// std::cout << "COMMAND : NICK" << std::endl;
// 	if (arg.size() < 1)
// 	{
// 		client->reply(ERR_NONICKNAMEGIVEN(client->getNickname(), "NICK"));
// 		return;
// 	}

// 	std::string reply = client->getNickname();
// 	std::string nickname = arg[0];
// 	if (checkNickInvalid(nickname) == false)
// 	{
// 		client->reply(ERR_ERRONEUSNICKNAME(client->getNickname()));
// 		return;
// 	}
// 	// std::map<int, ClientInfo *> _clientsMap = getClients();

// 	for (std::map<int, ClientInfo *>::iterator it = _clientsMap.begin(); it != _clientsMap.end(); it++)
// 	{
// 		if (nickname == it.operator*().second->getNickname())
// 				nickname.push_back('_');
// 	}
// 	if (getClient(nickname))
// 	{
// 		client->reply(ERR_NICKNAMEINUSE(client->getNickname()));
// 		return;
// 	}
// 	std::string oldNick = client->getNickname();
// 	std::string newNick = nickname;
// 	client->setNickname(nickname);
// 	reply.append(" changed his nickname to ");
// 	reply.append(client->getNickname()); 
// 	client->reply_command(reply);
// 	client->reply_command(RPL_NICK(oldNick, newNick));

// }

// void CmdUser(Server *server, ClientInfo *client, std::vector<std::string> arg)
// {
// std::cout << "COMMAND : USER" << std::endl;
// 	int param_size = arg.size();
// 	if (param_size < 3)
// 	{
// 		client->reply(ERR_NEEDMOREPARAMS(client->getNickname(), "USER"));
// 		return;
// 	}
// 	else if (client->getRegistered() == 1)
// 	{
// 		client->reply(ERR_ALREADYREGISTERED(client->getNickname()));
// 		return;
// 	}
// 	// else if (client->getPassword() != _server->getPassword())
// 	// {
// 	// 	client->reply(ERR_PASSWDMISMATCH(client->getNickname()));
// 	// 	//_server->onClientDisconnect(client->getFd(), _server->getEpollfd());
// 	// 	_server->setErrorPass(1);
// 	// 	return;
// 	// }
// 	else
// 	{
// 		std::string message;
// 		std::string realname;
// 		std::string username = arg[0];
// 		client->setUsername(username);
// 		for (std::vector<std::string>::iterator it = arg.begin(); it != arg.end(); ++it) 
// 		{
// 			std::string tosend; 
// 			tosend = (*it); 
// 			message.append(" ");
// 			message.append(tosend);
// 		}
// 		message.append("\t\n");
// 		std::cout << message << std::endl;
// 		size_t start;
// 		size_t end;
// 		start = message.find(":");
// 		if(message.find(":") == std::string::npos)
// 		{
// 			realname = arg[2];
// 			std::cout << start << std::cout;
// 		}
// 		else
// 		{
// 			end = message.find_first_of("\t\n", start);
// 			realname = message.substr(start + 1, end - start -1);
// 		}
// 		client->setRealname(realname);
// 		client->setRegistered(1);
// 	}

// }