/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 13:03:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/13 14:50:09 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/client.hpp"

Client::Client(int client_fd, std::string &client_ip) : _fd(client_fd), _ip_addr(client_ip), _authenticated(false), _registered(false) {};


const int &Client::getFd() const
{
	return (_fd);
}

const std::string &Client::getNick() const
{
	return (_nick);
}

const std::string &Client::getUserName() const
{
	return (_username);
}

const std::string &Client::getHostName() const
{
	return (_hostname);
}

std::string Client::getPrefix() const
{
	std::string prefix;
	prefix = getNick() + "!" + getUserName() + "@" + getHostName();
	return (prefix);
}

void Client::set_fd(int &fd)
{
	this->_fd = fd;
}

void Client::set_ip(std::string ip_addr)
{
	this->_ip_addr = ip_addr;
}


void Client::set_nick(std::string &nick)
{
	this->_nick = nick;
}

void Client::set_username(std::string &username)
{
	this->_username = username;
}

void Client::set_hostname(std::string &hostname)
{
	this->_hostname = hostname;
}

void Client::authenticate()
{
	_authenticated = true;
}

void Client::register_client()
{
	_registered = true;
}

bool Client::is_authenticated()
{
	return (_authenticated);
}

bool Client::is_registered()
{
	return (_registered);
}

std::string&	Client::getCommandBuffer()// -marcus-
{
	return (this->_CommandBuffer);
}
