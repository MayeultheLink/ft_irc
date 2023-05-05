#include "Server.hpp"

Server::Server( const std::string & port, const std::string & password ) : _port(port), _password(password) {}

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

	std::cout << "Server socket generated\n";	
}

void Server::launch( void )
{

	generate_socket();
	
	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0)
		throw std::runtime_error("Error while creating epoll file descriptor\n");

	struct epoll_event event;
	event.events = EPOLLIN;
	event.data.fd = _sockfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event))
	{
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
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		std::cout << "Event_count = " << event_count << std::endl;
		for (int i = 0; i < event_count; i++)
		{
			std::cout << "connection received, i = " << i << std::endl;
			if (events[i].data.fd == _sockfd)
			{
				int connect;
				sockaddr_in connect_serv = {AF_INET, 0, {0}, {0}};
				socklen_t size = sizeof connect_serv;
				connect = accept(_sockfd, (struct sockaddr*) &connect_serv, &size);
				if (connect < 0)
					throw std::runtime_error("Error while accepting a connection\n");
				event.data.fd = connect;
				event.events = EPOLLIN;
				if (!epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connect, &event))
				{
					std::cout << "Client n " << event.data.fd << " connected" << std::endl;
					break ;
				}
			}
			else
			{
				bytes_read = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);
				//bytes_read = read(events[i].data.fd, buffer, BUFFER_SIZE);
				buffer[bytes_read] = 0;
				std::string cast(buffer);
				std::cout << cast << std::endl;
				//send(_sockfd, buffer, strlen(buffer), 0);
				//write(1, "\n", 1);
				//printf("%s\n", buffer);
				if (!strcmp(buffer, "stop\n"))
					running = 0;
			}
		}
	}

	if (close(epoll_fd))
		throw std::runtime_error("Failed to close file descriptor");
	
}
