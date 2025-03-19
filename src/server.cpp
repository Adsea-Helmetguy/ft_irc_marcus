/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:41:53 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/19 18:43:17 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"


Server::Server(const std::string &port, const std::string &password)
{
	if (!isValidPort(port.c_str()))
	{
		std::cerr << "Error: Invalid port number. Must be between 1024 and 65535." << std::endl;
		exit(1);
	}
	_port = std::strtol(port.c_str(), NULL, 10);
	_password = password;

	std::cout << "Server created on port: " << port << " with password: " << password << std::endl;

	// create the TCP socket
	_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_socket_fd == -1)
	{
		std::cerr << "Error: Could not create socket" << std::endl;
		exit(1);
	}

	// configure the sockaddr
	memset(&_serverAdd, 0, sizeof(_serverAdd));
	_serverAdd.sin_family = AF_INET;
	_serverAdd.sin_port = htons(_port);
	_serverAdd.sin_addr.s_addr = INADDR_ANY;

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
	std::cout << "Server is listening on port " << _port << std::endl;
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

void Server::runServer()
{
	while (true)
	{
		std::cout << "Waiting for a connection..." << std::endl;

		struct sockaddr_in clientAddr;
		socklen_t clientLen = sizeof(clientAddr);
		int client_fd = accept(_socket_fd, (struct sockaddr *)&clientAddr, &clientLen);

		if (client_fd == -1)
		{
			std::cerr << "Error: Failed to accept client" << std::endl;
			continue;
		}

		std::cout << "Client connected from: " << inet_ntoa(clientAddr.sin_addr) << ":"
			<< ntohs(clientAddr.sin_port) << std::endl;

		const char *welcomeMsg = "Welcome to the IRC server!\n";
		send(client_fd, welcomeMsg, strlen(welcomeMsg), 0);

		close(client_fd);
		std::cout << "Client disconnected." << std::endl;
	}
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
