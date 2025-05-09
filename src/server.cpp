/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:41:53 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/09 16:47:26 by gyong-si         ###   ########.fr       */
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
	_name = "ircserv";
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

const std::string &Server::getName() const
{
	return (_name);
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

void Server::handlePass(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 2)
	{
		// need to change this
		send(fd, "ERROR :PASS command requires exactly one argument\r\n", 50, 0);
		return ;
	}
	// iterate to the second item in the list
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	const std::string &provided = *it;
	if (provided == _password)
	{
		client->authenticate();
		send(fd, "NOTICE AUTH :Password accepted\r\n", 33, 0);
		std::cout << "Client " << fd << " : has been authenticated.\n";
	}
	else
		send(fd, "ERROR :Invalid password\r\n", 26, 0);
}

void Server::handleNick(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 2)
	{
		// need to change this
		send(fd, "ERROR :PASS command requires exactly one argument\r\n", 50, 0);
		return ;
	}
	// check if user has authenticated.
	if (!client->is_authenticated())
	{
		const std::string &errorMsg = "ERROR :You must authenticate with PASS first\r\n";
		sendError(fd, errorMsg);
		std::cout << "[WARN] Client " << fd << " tried to send NICK/USER before PASS\n";
		return ;
	}
	// iterate to the second item in the list
	// /NICK nickname
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	std::string second = *it;
	client->set_nick(second);
	std::cout << "[NICK] " << second << " has been saved." << std::endl;
}

void Server::sendWelcome(Client *client)
{
	std::string serverName = this->getName();
	std::string nick = client->getNick();
	int fd = client->getFd();

	std::string msg001 = ":" + serverName + " 001 " + nick +
		" :Welcome to the IRC server!\r\n";
	std::string msg002 = ":" + serverName + " 002 " + nick +
		" :Your host is " + serverName + ", running version 1.0\r\n";
	std::string msg003 = ":" + serverName + " 003 " + nick +
		" :This server was created today\r\n";
	std::string msg004 = ":" + serverName + " 004 " + nick +
		" " + serverName + " 1.0 o o\r\n";
	std::string msg375 = ":" + serverName + " 375 " + nick +
		" :- " + serverName + " This is ft-irc - \r\n";
	std::string msg372 = ":" + serverName + " 372 " + nick +
		" :- Welcome to the IRC server!\r\n";
	std::string msg376 = ":" + serverName + " 376 " + nick +
		" :End of /MOTD command.\r\n";

	send(fd, msg001.c_str(), msg001.size(), 0);
	send(fd, msg002.c_str(), msg002.size(), 0);
	send(fd, msg003.c_str(), msg003.size(), 0);
	send(fd, msg004.c_str(), msg004.size(), 0);
	send(fd, msg375.c_str(), msg375.size(), 0);
	send(fd, msg372.c_str(), msg372.size(), 0);
	send(fd, msg376.c_str(), msg376.size(), 0);
}

void Server::handleUser(int fd, std::list<std::string> cmd_list)
{
	//std::cout << "entered handleUser";
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 5)
	{
		const std::string &errorMsg = "ERROR :USER command requires three argument\r\n";
		sendError(fd, errorMsg);
		return ;
	}
	// check if user has authenticated.
	if (!client->is_authenticated())
	{
		const std::string &errorMsg = "ERROR :You must authenticate with PASS first\r\n";
		sendError(fd, errorMsg);
		std::cout << "[WARN] Client " << fd << " tried to send NICK/USER before PASS\n";
		return ;
	}
	// iterate to the second item in the list
	// /USER username something hostname
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	std::string username = *it;
	//std::cout << second << std::endl;
	client->set_username(username);
	++it;++it;
	std::string hostname = *it;
	//std::cout << fourth << std::endl;

	client->set_hostname(hostname);
	std::cout << "[USER] : " << username << ", " << "[USER] : "<< hostname  << std::endl;
	// check if all is set, return a message
	if (!client->getNick().empty() && !client->getUserName().empty()
		&& !client->getHostName().empty() && !client->is_registered())
	{
		client->register_client();
		sendWelcome(client);
	}
}

void Server::handleJoin(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 2)
	{
		// need to change this
		send(fd, "ERROR :JOIN command requires exactly one argument\r\n", 50, 0);
		return ;
	}
	// extract the channel name
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	const std::string &channelName = *it;

	// iterate over _channels to search if the channel already exist
	Channel *channel = NULL;
	for (std::vector<Channel>::iterator ch = _channels.begin(); ch != _channels.end(); ++ch)
	{
		if (ch->getName() == channelName)
		{
			channel = &(*ch);
			break ;
		}
	}
	// if the channel does not exit, create the channel
	if (!channel)
	{
		// create a new channel with the name given
		Channel nc(channelName);
		// add the client to the channel
		nc.addMember(client);
		// add the client as operator
		nc.addOperator(client);
		_channels.push_back(nc);
		// send a message back to client
		std::string reply = "JOIN " + channelName + "\r\n";
		send(fd, reply.c_str(), reply.size(), 0);
		std::cout << "[INFO] New channel " << channelName
				  << " created by " << client->getNick() << "\n";
	}
	else
	{
		// this channel already exit, add the client as member
		// need to check if client is already a member
		if (!channel->isMember(client))
		{
			channel->addMember(client);
			std::string reply = "JOIN " + channelName + "\r\n";
			send(fd, reply.c_str(), reply.size(), 0);
		}
	}
}


void	Server::execute_cmd(int fd, std::list<std::string> cmd_lst)
{
	const std::string &cmd = cmd_lst.front();
	const std::string &errorMsg = "ERROR :Unknown command\r\n";
	if (cmd == "PASS")
		// authenticate the user
		handlePass(fd, cmd_lst);
	else if (cmd == "NICK")
		// set the nickname
		handleNick(fd, cmd_lst);
	else if (cmd == "USER")
		handleUser(fd, cmd_lst);
	else if (cmd == "JOIN")
		handleJoin(fd, cmd_lst);
	else if (cmd == "CAP")
		;
	else if (cmd == "MODE")
		;
	else
		sendError(fd, errorMsg);
}


void Server::handleClientConnection(int fd)
{
	char buffer[512];
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
		std::string command(buffer);

		std::cout << "Received from client " << fd << ": " << command << std::endl;
		//Client* client = getClientByFd(fd);

		// break the command from the user into individual str
		// the client can send multiple lines to the user
		// run a loop and break each line to execute it.
		std::istringstream	iss(command);
		std::string			line;

		while (std::getline(iss, line))
		{
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.size() - 1, 1);
			std::cout << line << std::endl;
			std::list<std::string> cmd_lst = splitString(line);
			// function to execute cmd
			if (!cmd_lst.empty())
				execute_cmd(fd, cmd_lst);
		}
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
