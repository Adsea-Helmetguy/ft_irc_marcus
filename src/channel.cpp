/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:21:05 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/14 10:25:10 by gyong-si         ###   ########.fr       */
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

const std::vector<Client*> &Channel::getMembers() const
{
	return (_members);
}

const std::vector<Client*> &Channel::getOperators() const
{
	return (_operators);
}


const std::string &Channel::getTopic()
{
	return (_topic);
}


/**
 * This function works now
 */
std::string Channel::getClientList()
{
	std::string clientList = "";

	for (size_t i = 0; i < _operators.size(); ++i)
	{
		clientList += "@" + _operators[i]->getNick();
		if ((i + 1) < _operators.size())
			clientList + " ";
	}
	if (_members.size())
		clientList += " ";
	for (size_t i = 0; i < _members.size(); ++i)
	{
		clientList += "@" + _members[i]->getNick();
		if ((i + 1) < _members.size())
			clientList + " ";
	}
	std::cout << "Printing client list" << std::endl;
	std::cout << clientList << std::endl;
	return (clientList);
}

void Channel::setName(const std::string &name)
{
	_name = name;
}

void Channel::addMember(Client *client)
{
	if (!isMember(client))
		_members.push_back(client);
}

void Channel::removeMember(Client *client)
{
	_members.erase(
		std::remove(_members.begin(), _members.end(), client),
		_members.end()
	);
}

bool Channel::isMember(Client *client)
{
	for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it == client)
			return (true);
	}
	return (false);
}

void Channel::addOperator(Client *client)
{
	if (!isOperator(client))
		_operators.push_back(client);
}

void Channel::removeOperator(Client *client)
{
	_operators.erase(
		std::remove(_operators.begin(), _operators.end(), client),
		_operators.end()
	);
}

bool Channel::isOperator(Client *client) const
{
	return (std::find(_operators.begin(), _operators.end(), client) != _operators.end());
}

/**
 * Sends a message to all members of a channel
 */
void Channel::broadcast(const std::string &message, Client *exclude)
{
	for (std::vector<Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if (*it != exclude)
			sendReply((*it)->getFd(), message);
	}
	for (std::vector<Client*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it != exclude)
			sendReply((*it)->getFd(), message);
	}
}
