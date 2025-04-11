/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:31:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/04/11 12:12:48 by gyong-si         ###   ########.fr       */
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
#include <vector>
#include <csignal>
#include <cerrno>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/time.h>

#include "../include/client.hpp"

# define MAX_EVENTS 10

class Client;

class Server
{
	private:
		long 				_port;
		std::string			_password;
		static bool			_signal;
		int					_socket_fd;
		struct sockaddr_in 	_serverAdd;
		int 				_epoll_fd;
		std::vector<Client>	_clients;


		Server(const Server &src);
		Server &operator=(const Server &src);

	public:
		Server(const std::string &port, const std::string &password);
		//Server(long &port, const std::string &password);
		~Server();
		void		runServer();
		void		serverInit();
		static void signalHandler(int signum);

		// add and remove clients
		void		addClient(const Client &client);
		void		removeClient(int fd);
		const std::vector<Client>& getClients() const;
		void		closeClients();

};

bool isValidPort(const char *portStr);
void setupSignalHandler();
