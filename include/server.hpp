/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:31:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/18 20:39:44 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

class Server
{
	private:
		long 			_port;
		std::string		_password;

		Server(const Server &src);
		Server &operator=(const Server &src);

	public:
		Server(long &port, const std::string &password);
		~Server();

};

bool isValidPort(const char *portStr);
