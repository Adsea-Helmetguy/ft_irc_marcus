/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:41:53 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/18 15:53:54 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"


Server::Server(long &port, const std::string &password) : _port(port), _password(password)
{
	std::cout << "Server created on port: " << port << " with password: " << password << std::endl;
}

Server::~Server()
{
	std::cout << "Server with port " << _port << " is destroyed." << std::endl;
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
