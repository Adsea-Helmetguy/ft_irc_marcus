/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:41:53 by gyong-si          #+#    #+#             */
/*   Updated: 2025/06/20 12:36:21 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

bool Server::_signal = false;

Server::Server(const std::string &port, const std::string &password)
{
	if (port.empty() || password.empty())
	{
		std::cerr << RED << "Error: Arguments are empty!" << RT << std::endl;
		exit(1);
	}
	if (!isValidPort(port.c_str()))
	{
		std::cerr << RED << "Error: Invalid port number. Must be between 1024 and 65535."
			<< RT << std::endl;
		exit(1);
	}
	if (!isValidPassword(password))
	{
		std::cerr << RED << "Error: Password is invalid. Does it have spaces?"
			<< RT << std::endl;
		exit(1);
	}
	_name = "ircserv";
	_port = std::strtol(port.c_str(), NULL, 10);
	_password = sha256(password);
	_created_time = getFormattedTime();

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


Client* Server::getClientByNick(const std::string &clientNick)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if ((*it)->getNick() == clientNick)
			return (*it);
	}
	return (NULL);
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
	std::cout << "Server created on port: " << _port << " with password" << std::endl;
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
			else if (events[i].events & EPOLLIN)
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

	// set the client fd to non blocking
	if (setnonblocking(client_fd) == -1)
	{
		close(client_fd);
		return ;
	}

	// Create and store the new client
	Client* newClient = new Client(client_fd, client_ip);

	// this adds the client into the clients list
	_clients.push_back(newClient);

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


void Server::sendWelcome(Client *client)
{
	std::string serverName = this->getName();
	std::string nick = client->getNick();
	int fd = client->getFd();

	sendReply(fd, RPL_WELCOME(serverName, nick));
	sendReply(fd, RPL_YOURHOST(serverName, nick));
	sendReply(fd, RPL_CREATED(serverName, nick, _created_time));
	sendReply(fd, RPL_MYINFO(serverName, nick));

	sendReply(fd, RPL_MOTDSTART(serverName, nick));
	sendReply(fd, RPL_MOTD(serverName, nick));
	sendReply(fd, RPL_ENDOFMOTD(serverName, nick));
}

void	Server::execute_cmd(int fd, std::list<std::string> cmd_lst)
{
	Client* client = getClientByFd(fd);
	const std::string &cmd = cmd_lst.front();

	if (cmd == "PASS")
		handlePass(fd, cmd_lst);// authenticate the user
	else if (cmd == "NICK")
		handleNick(fd, cmd_lst);// set the nickname
	else if (cmd == "USER")
		handleUser(fd, cmd_lst);
	else if (cmd == "JOIN")
		handleJoin(fd, cmd_lst);
	else if (cmd == "PING")
		handlePing(fd, cmd_lst);
	else if (cmd == "CAP")
		;
	else if (cmd == "MODE")
		handleMode(fd, cmd_lst);
	else if (cmd == "PART")
		handlePart(fd, cmd_lst);
	else if (cmd == "PRIVMSG")
		handlePrivmsg(fd, cmd_lst);
	else if (cmd == "KICK")
		handleKick(fd, cmd_lst);
	else if (cmd == "INVITE")
		handleInvite(fd, cmd_lst);
	else if (cmd == "TOPIC")
		handleTopic(fd, cmd_lst);
	else
		sendError(fd, ERR_UNKNOWNCOMMAND(getName(), client->getNick(), cmd));
}


void Server::handleClientConnection(int fd)
{
	char buffer[513];
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

//
//-marcus ->detect the EOF properly!
		Client*			client = getClientByFd(fd);
		std::string&	CommandBuffer = client->getCommandBuffer();	// ① (Get per-client buffer)
		CommandBuffer.append(buffer);								// ② (Add new data to buffer)
	
		size_t newlinePos = 0;//stores the '\n' it finds in.
		while ((newlinePos = CommandBuffer.find('\n')) != std::string::npos)// ③ (full command "\n")
		{
			// Extract the line from getCommandBuffer string
			std::string line = CommandBuffer.substr(0, newlinePos);	// ④ (Extract full command "CommandBuffer")
	
			// Remove \r if it exists before \n
			if (!line.empty() && line[line.length() - 1] == '\r')		// ⑤ (Remove \r)
				line.erase(line.length() - 1);
	
			std::cout << "    Command token =" << line << std::endl;
	
			// Tokenize the line into command + arguments
			std::list<std::string> cmd_lst = splitString(line);			// ⑥ (Split into tokens)
	
			if (!cmd_lst.empty())
				execute_cmd(fd, cmd_lst);
	
			// Remove processed line from buffer
			CommandBuffer.erase(0, newlinePos + 1);			// ⑦ (Remove processed line)
		}
//-marcus
//

		// -Ivan's old code-
		// break the command from the user into individual str
		// the client can send multiple lines to the user
		// run a loop and break each line to execute it.
// 		std::istringstream	iss(command);
// 		std::string			line;

		// while (std::getline(iss, line))
		// {
		// 	if (!line.empty() && line[line.size() - 1] == '\r')
		// 		line.erase(line.size() - 1, 1);
		// 	//std::cout << line << std::endl;
		// 	std::list<std::string> cmd_lst = splitString(line);

		// 	// function to execute cmd
		// 	if (!cmd_lst.empty())
		// 		execute_cmd(fd, cmd_lst);
		// }
	}
}

Client*	Server::getClientByFd(int fd)
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getFd() == fd)
		{
			return (*it);
		}
	}
	return (NULL);
};

Channel* Server::getChannelByName(const std::string &channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++)
	{
		if (it->getName() == (channelName))
		{
			return &(*it);
		}
	}
	return (NULL);
}

void	Server::removeChannel(const std::string &channelName)
{
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	{
		if (it->getName() == channelName)
		{
			_channels.erase(it);
			std::cout << "[INFO] Channel \"" << channelName << "\" has been removed." << std::endl;
			return ;
		}
	}
}


// remove the client from the client list
void	Server::removeClient(int fd)
{
	Client* client = getClientByFd(fd);
    if (!client)
		return;
	
	for (std::vector<Channel>::iterator it = _channels.begin(); it != _channels.end(); it++) 
	{
        it->removeUser(client); 
    }
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		if ((*it)->getFd() == fd)
		{
			close(fd);
			delete *it;
			_clients.erase(it);
			std::cout << "Client " << fd << " removed." << std::endl;
			return ;
		}
	}
}

void	Server::closeClients()
{
	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
	{
		close((*it)->getFd());
	}
	_clients.clear();
	std::cout << "ALL the remaining client Fds are closed." << std::endl;
}

const std::vector<Client*>& Server::getClients() const
{
	return (_clients);
}

//|--------------------------------------|
//|        -MARCUS FUNCTIONS-            |
//|--------------------------------------|
void	Server::modeTo_execute(char operation, char mode, Channel *targetChannel, Client &client, std::string stringPassed)
{
	std::stringstream logMsg;
	std::string msg;
	logMsg.clear();
	msg.clear();

	if (operation == '\0' || mode == '\0' || targetChannel == NULL)
	{
		std::cout << RED << "SOMETHING IS WRONG HERE" << RT << std::endl;
		return ;
	}
	if (mode == 'i' || mode == 't')
		msg = ":" + client.getPrefix() + " MODE " + targetChannel->getName() + " " + operation + mode + "\r\n";
	else if ((mode == 'k' || mode == 'l') && operation == '-')
	{
		if (mode == 'l')
			msg = ":" + client.getPrefix() + " MODE " + targetChannel->getName() + " " + operation + mode + "\r\n";
		else
			msg = ":" + client.getPrefix() + " MODE " + targetChannel->getName() + " " + operation + mode + " *" + "\r\n";
	}
	else
	{
		if (stringPassed.empty())
			return ;
		msg = ":" + client.getPrefix() + " MODE " + targetChannel->getName() + " " + operation 
			+ mode + " " + stringPassed + "\r\n";
	}
	targetChannel->broadcast(msg);
}

//sets channel to invite only
void	Server::invite_only(Channel *targetChannel, char operation, Client &client)
{
	if (operation == '+')
		targetChannel->SetInviteOnly(true);//set the channel as invite only
	else if (operation == '-')
		targetChannel->SetInviteOnly(false);
	if (targetChannel->getchannelIsInviteOnly() == true)
		std::cout << GREEN << "[DEBUG] Channel is invite only now." << RT << std::endl;
	std::cout << YELLOW << "[DEBUG] Current value of channel-> \"" << RED 
		<< targetChannel->getchannelIsInviteOnly() << RT << "\"" << std::endl;
	modeTo_execute(operation, 'i', targetChannel, client, "");
}

void	Server::topic_restriction(Channel *targetChannel, char operation, Client &client)
{
	if (operation == '+')
		targetChannel->setTopicRestriction(true);
	else if (operation == '-')
		targetChannel->setTopicRestriction(false);

	modeTo_execute(operation, 't', targetChannel, client, "");
	if (targetChannel->getisTopicRestricted() == true)
		std::cout << GREEN << "[DEBUG] Channel has a Topic now." << RT << std::endl;
	std::cout << YELLOW << "[DEBUG] Topic value-> \"" << RED 
		<< targetChannel->getTopic() << RT << "\"" << std::endl;
}

void	Server::channel_password(Channel *targetChannel, char operation, std::list<std::string>::iterator &it, Client &client)
{
	if (!(*it).empty())
	{
		if (operation == '+')
				targetChannel->setchannelPassword(*it);
		else if (operation == '-')
			targetChannel->removechannelPassword();

		modeTo_execute(operation, 'k', targetChannel, client, targetChannel->getchannelPassword());
		if (!targetChannel->getchannelPassword().empty())
		{
			std::cout << GREEN << "[DEBUG] Channel has a password." << RT << std::endl;
			std::cout << YELLOW << "[DEBUG] Password-> \"" << RED 
				<< targetChannel->getchannelPassword() << RT << "\"" << std::endl;
		}
		else
			std::cout << YELLOW << "[DEBUG] Password unavailable." << RT << std::endl;
	}
}

//add the client's fd to the operatorlist
void	Server::operator_addon(Channel *targetChannel, char operation, std::list<std::string>::iterator &it, Client &client)
{
	bool	print_success = false;

	std::cout << YELLOW << "Inside operator_addon" << RT << std::endl;
	if (operation == '+')
		targetChannel->OperatorTrue(it, print_success);
	else if (operation == '-')
		targetChannel->OperatorFalse(it, print_success);
	
	if (print_success == true)
	{
		std::cout << GREEN << "[JUST CHECKING] Name is---> " << *it << RT << std::endl;
		modeTo_execute(operation, 'o', targetChannel, client, *it);
	}
}

//broadcast message?
void	Server::user_limit(Channel *targetChannel, char operation, std::list<std::string>::iterator &it, std::list<std::string> &cmd_lst, Client &client)
{
	bool	print_success = false;

	std::cout << YELLOW << "Inside user_limit" << RT << std::endl;
	if (operation == '+' && (++it != cmd_lst.end()))
		targetChannel->limitSet(it, print_success);
	else if (operation == '-')
		targetChannel->limitUnset(print_success);

	if (print_success == true)
		modeTo_execute(operation, 'l', targetChannel, client, *it);
}
//Marcus functions
