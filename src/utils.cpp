/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 14:56:47 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/14 09:42:31 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/utils.hpp"

void	setupSignalHandler()
{
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
}

bool	isValidPort(const char *portStr)
{
	for (size_t i = 0; portStr[i]; i++)
		if (!isdigit(portStr[i]))
			return false;

	long portNum = std::strtol(portStr, NULL, 10);

	if (portNum < 1024 || portNum > 65535)
		return false;
	return true;
}

bool	isValidPassword(const std::string password)
{
	//IF Password is printable ASCII
	for (size_t i = 0; i < password.size(); i++)
	{
		if (password[i] < 33 || password[i] > 126)
			return false;
	}
	return true;
}

std::list<std::string> splitString(std::string &cmd)
{
	std::list<std::string> lst;
	std::istringstream stm(cmd);
	std::string token;

	while (stm >> token)
	{
		lst.push_back(token);
		token.clear();
	}
	return (lst);
}

void	sendError(int fd, const std::string &message)
{
	if (send(fd, message.c_str(), message.size(),0) == -1)
		std::cerr << "Error MSG sent" << std::endl;
}

void	sendReply(int fd, const std::string &message)
{
	if (send(fd, message.c_str(), message.size(),0) == -1)
		std::cerr << "Reply sent" << std::endl;
}

std::string getFormattedTime()
{
	time_t rawTime;
	std::tm *timeInfo;
	char buffer[80];

	std::time(&rawTime);
	timeInfo = std::gmtime(&rawTime); // Convert to UTC

	// Format: "Wed Jul 17 2024 at 21:46:54 UTC"
	std::strftime(buffer, sizeof(buffer), "%a %b %d %Y at %H:%M:%S UTC", timeInfo);
	return std::string(buffer);
}

int	setnonblocking(int client_fd)
{
	int flags = fcntl(client_fd, F_GETFL, 0);
	if (flags == -1)
		return (-1);

	return (fcntl(client_fd, F_SETFL, flags | O_NONBLOCK));
}
