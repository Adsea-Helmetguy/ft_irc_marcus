/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:21:05 by gyong-si          #+#    #+#             */
/*   Updated: 2025/06/23 15:23:31 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"

Channel::Channel(const std::string &name, const std::string &password)
{
	_name = name;
	_password = password;
	_topic = "Welcome to " + getName();
	_created_time = getFormattedTime();
	_inviteOnly = false;
	_topicRestricted = false;
    _topicSetter = "";
    _topicTime = 0;
    _channellimitSize = 0;
    _channelIslimited = false;
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

size_t Channel::getUsersSize() const
{
	return (_users.size());
}

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
	return (clientList);
}

void Channel::addOperator(Client *client)
{
	this->_users.push_back(ChannelUser(client, true));
}

const std::string &Channel::getPassword() const
{
	return (_password);
}

const std::string &Channel::getCreationTime() const
{
	return (_created_time);
}

void Channel::setName(const std::string &name)
{
	_name = name;
}

void Channel::setPassword(const std::string &password)
{
	_password = password;
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

bool Channel::checkNickNameUsed(const std::string &nickname)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->client->getNick() == nickname)
			return (true);
	}
	return (false);
}

bool Channel::hasPassword() const
{
	return (!_password.empty());
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

void Channel::broadcast(const std::string &message, const Client *exclude)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		if (it->client != exclude)
		{
			std::cout << "Sending to client with fd: " << it->client->getFd() << std::endl;
			sendReply(it->client->getFd(), message);
		}
	}
}

void Channel::broadcast(const std::string &message)
{
	for (std::vector<ChannelUser>::iterator it = _users.begin(); it != _users.end(); ++it)
	{
		sendReply(it->client->getFd(), message);
	}
}

void Channel::setTopic(const std::string& topic)
{
    this->_topic = topic;
    std::cout << CYAN << "[DEBUG] Topic set to: '" << topic << "'" << RT << std::endl;
}

std::string Channel::getTopic() const
{
    return this->_topic;
}

void Channel::setTopicSetter(const std::string& setter)
{
    this->_topicSetter = setter;
}

std::string Channel::getTopicSetter() const
{
    return this->_topicSetter;
}

void Channel::setTopicTime(time_t timestamp)
{
    this->_topicTime = timestamp;
}

time_t Channel::getTopicTime() const
{
    return this->_topicTime;
}

std::string Channel::getTopicTimeString() const
{
    std::ostringstream oss;
    oss << this->_topicTime;
    return oss.str();
}

bool Channel::kickUser(Client *operator_client, Client *target_client, const std::string &reason)
{
	if (!isOperator(operator_client))
		return false;
	
	if (!isMember(target_client))
		return false;
	std::string kickMessage = ":" + operator_client->getPrefix() + " KICK " + _name + " " + 
		target_client->getNick() + " :" + reason + "\r\n";
	
	broadcast(kickMessage);
	removeUser(target_client);
	
	return true;
}