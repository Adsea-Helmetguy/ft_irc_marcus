/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:41:53 by gyong-si          #+#    #+#             */
/*   Updated: 2025/04/16 15:25:26 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

bool Server::_signal = false;

Server::Server(const std::string &port, const std::string &password)
{
	if (!isValidPort(port.c_str()))
	{
		std::cerr << "Error: Invalid port number. Must be between 1024 and 65535." << std::endl;
		exit(1);
	}
	_port = std::strtol(port.c_str(), NULL, 10);
	_password = password;

	// setup the TCP socket
	this->serverInit();
}

Server::~Server()
{
	close(_socket_fd);
	std::cout << "Server with port " << _port << " is shutting down." << std::endl;
}


Server::Server(const Server &src)
{
	*this = src;
}

Server &Server::operator=(const Server &src)
{
	if (this != &src)
	{
		_port = src._port;
		_password = src._password;
	}
	return (*this);
}

void Server::serverInit()
{
	_signal = false;

	// create the TCP socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
	{
		std::cerr << "Error: Could not create socket" << std::endl;
		exit(1);
	}
	// create the epoll fd
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
	{
		std::cerr << "Error: Could not create epoll instance\n";
		exit(1);
	}

	// Add the server socket to epoll
	struct epoll_event ev;
	ev.events = EPOLLIN;  // Listening for incoming connections
	ev.data.fd = _socket_fd;

	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket_fd, &ev) == -1)
	{
		std::cerr << "Error: Could not add server socket to epoll\n";
		exit(1);
	}

	// configure the sockaddr
	memset(&_serverAdd, 0, sizeof(_serverAdd));
	_serverAdd.sin_family = AF_INET;
	_serverAdd.sin_port = htons(_port);
	_serverAdd.sin_addr.s_addr = INADDR_ANY;

	// sets a timeout of the socket
	struct timeval timeout;
	timeout.tv_sec = 1;  // Set timeout to 1 second
	timeout.tv_usec = 0;
	if (setsockopt(_socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0)
	{
		std::cerr << "Error: Could not set timeout on accept()" << std::endl;
	}

	// bind the socket to IP and port
	if (bind(_socket_fd, (struct sockaddr *)&_serverAdd, sizeof(_serverAdd)) == -1)
	{
		std::cerr << "Error: Could not bind to port " << _port << std::endl;
		close(_socket_fd);
		exit(1);
	}

	// put the server in listening mode
	// 5 is the backlog number, increase this if needed
	if (listen(_socket_fd, 5) == -1)
	{
		std::cerr << "Error: Could not listen on port " << _port << std::endl;
		close(_socket_fd);
		exit(1);
	}
	std::cout << "Server created on port: " << _port << " with password: " << _password << std::endl;
	std::cout << "Server is listening on port " << _port << std::endl;
}

void Server::runServer()
{
	while (1)
	{
		if (_signal)
			break;

		struct epoll_event events[MAX_EVENTS];

		int n = epoll_wait(_epoll_fd, events, MAX_EVENTS, -1);
		if (n == -1)
		{
			if (_signal)
				break;
			std::cerr << "epoll_wait failed\n";
			continue;
		}

		for (int i = 0; i < n; ++i)
		{
			int fd = events[i].data.fd;
			if (fd == _socket_fd)
				handleIncomingNewClient();
			else
				handleClientConnection(fd);
		}
	}
	closeClients();
	close(_socket_fd);
	close(_epoll_fd);
	std::cout << "Shutting down server..." << std::endl;
}

void Server::signalHandler(int signum)
{
	(void)signum;
	std::cout << "\n";
	std::cout << "Signal Received! Stopping server..." << std::endl;
	_signal = true;
}

void	Server::handleIncomingNewClient()
{
	// creates the client fd
	struct sockaddr_in clientAddr;
	socklen_t clientLen = sizeof(clientAddr);
	int client_fd = accept(_socket_fd, (struct sockaddr *)&clientAddr, &clientLen);

	if (client_fd == -1)
	{
		std::cerr << "Error: Failed to accept client" << std::endl;
		return ;
	}

	// get the client ip
	std::string client_ip = inet_ntoa(clientAddr.sin_addr);
	std::cout << "Client connected from: " << client_ip << ":"
		<< ntohs(clientAddr.sin_port) << std::endl;

	// Create and store the new client
	Client newClient(client_fd, client_ip);

	// this adds the client into the clients list
	_clients.push_back(newClient);

	// set the client fd to non blocking
	int flags = fcntl(client_fd, F_GETFL, 0);
	fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

	// this adds the client fd into epoll
	struct epoll_event clientEvent;
	clientEvent.events = EPOLLIN;
	clientEvent.data.fd = client_fd;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &clientEvent) == -1)
	{
		std::cerr << "Failed to add client to epoll: " << strerror(errno) << std::endl;
		close(client_fd);
		return ;
	}
}

void	Server::handleClientCommands(int fd, std::string &message, Client *client)
{
	std::istringstream	iss(message);
	std::string			line;

	while (std::getline(iss, line))
	{
		std::istringstream linestream(line);
		std::string cmd;
		linestream >> cmd;

		if (cmd == "PASS")
		{
			std::string pass;
			linestream >> pass;
			if (pass == _password)
			{
				client->authenticate();
				send(fd, "NOTICE AUTH :Password accepted\r\n", 33, 0);
				std::cout << "Client " << fd << " : has been authenticated.\n";
			}
			else
			{
				send(fd, "ERROR :Invalid password\r\n", 26, 0);
				removeClient(fd);
				return ;
			}
		}
		else if (cmd == "NICK" || cmd == "USER")
		{
			// check if client has been authenticated
			if (!client->is_authenticated())
			{
				send(fd, "ERROR :You must authenticate with PASS first\r\n", 46, 0);
				std::cout << "[WARN] Client " << fd << " tried to send NICK/USER before PASS\n";
				continue;
			}
			if (cmd == "NICK")
			{
				std::string nick;
				linestream >> nick;
				client->set_nick(nick);
				std::cout << "[NICK] " << nick << " has been saved." << std::endl;
			}
			else if (cmd == "USER")
			{
				std::string username, unused, hostname;
				linestream >> username >> unused >> hostname;
				client->set_username(username);
				client->set_hostname(hostname);
				std::cout << "[USER] " << username << ", " << hostname  << std::endl;
			}
		}
		send(fd, "Welcome to the IRC Server\r\n", 29, 0);
	}
}


void Server::handleClientConnection(int fd)
{
	char buffer[1024];
	ssize_t bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

	// if the client quits, recv will receive 0 when closed or -1 when there is error
	if (bytesRead <= 0)
	{
		std::cout << "Client " << fd << " disconnected.\n";
		epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
		removeClient(fd);
	}
	else
	{
		buffer[bytesRead] = '\0';
		std::string message(buffer);

		std::cout << "Received from client " << fd << ": " << message << std::endl;
		Client* client = getClientByFd(fd);
		handleClientCommands(fd, message, client);
	}
}

Client*	Server::getClientByFd(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->getFd() == fd)
		{
			return &(*it);
		}
	}
	return (NULL);
};



// remove the client from the client list
void	Server::removeClient(int fd)
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if (it->getFd() == fd)
		{
			close(it->getFd());
			_clients.erase(it);
			std::cout << "Client " << fd << " removed." << std::endl;
			return ;
		}
	}
}

void	Server::closeClients()
{
	for (std::vector<Client>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		close(it->getFd());
	}
	_clients.clear();
	std::cout << "ALl the remaining client Fds are closed." << std::endl;
}

const std::vector<Client>& Server::getClients() const
{
	return (_clients);
}

void setupSignalHandler()
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
}

bool isValidPort(const char *portStr)
{
	for (size_t i = 0; portStr[i]; i++)
		if (!isdigit(portStr[i]))
			return false;

	long portNum = std::strtol(portStr, NULL, 10);

	if (portNum < 1024 || portNum > 65535)
		return false;
	return true;
}
