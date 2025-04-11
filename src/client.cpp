/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 13:03:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/04/10 13:27:00 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"

Client::Client(int client_fd, std::string &client_ip) : _fd(client_fd), _ip_addr(client_ip) {};


const int &Client::getFd() const
{
	return (_fd);
}

void Client::set_fd(int &fd)
{
	this->_fd = fd;
}

void Client::set_ip(std::string ip_addr)
{
	this->_ip_addr = ip_addr;
}
