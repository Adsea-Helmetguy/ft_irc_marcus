/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:31:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/19 10:22:45 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Server
{
	private:
		long 				_port;
		std::string			_password;
		int					_socket_fd;
		struct sockaddr_in 	_serverAdd;

		Server(const Server &src);
		Server &operator=(const Server &src);

	public:
		Server(const std::string &port, const std::string &password);
		//Server(long &port, const std::string &password);
		~Server();
		void runServer();

};

bool isValidPort(const char *portStr);
