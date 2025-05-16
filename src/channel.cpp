/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:21:05 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/16 10:35:28 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"

Channel::Channel(const std::string &name): _name(name)
{
	_topic = "Welcome to " + getName();
};

const std::string &Channel::getName() const
{
	return (_name);
}

const std::string &Channel::getTopic()
{
	return (_topic);
}

const std::vector<ChannelUser> &Channel::getUsers() const
{
	return (_users);
}

/**
 * This function works now
 */
std::string Channel::getClientList()
{
	std::string clientList = "";

	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (!clientList.empty())
			clientList += " ";
		if (_users[i].isOperator)
			clientList += "@" + _users[i].client->getNick();
		else
			clientList += _users[i].client->getNick();
	}
	std::cout << "Printing client list" << std::endl;
	std::cout << clientList << std::endl;
	return (clientList);
}

void Channel::setName(const std::string &name)
{
	_name = name;
}

bool Channel::isMember(Client *client)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->client == client)
			return (true);
	}
	return (false);
}

void Channel::addMember(Client *client)
{
	_users.push_back(ChannelUser(client, false));
}


bool Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (_users[i].client == client && _users[i].isOperator)
			return (true);
	}
	return (false);
}

void Channel::addOperator(Client *client)
{
	_users.push_back(ChannelUser(client, true));
}

void Channel::removeUser(Client *client)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->client == client)
		{
			_users.erase(it);
			break ;
		}
	}
}

/**
 * Sends a message to all members of a channel except the user specified
 */
void Channel::broadcast(const std::string &message, const Client *exclude)
{
	// loop through the _users vector to send the message
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->client != exclude)
		{
			std::cout << "[DEBUG] About to send to FD: " << it->client->getFd()
			<< ", nick: " << it->client->getNick() << std::endl;
			sendReply(it->client->getFd(), message);
		}
	}
}

/**
 * Sends the message to everybody.
 */
void Channel::broadcast(const std::string &message)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		sendReply(it->client->getFd(), message);
	}
}
