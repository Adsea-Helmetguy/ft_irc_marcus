/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/16 16:21:05 by gyong-si          #+#    #+#             */
/*   Updated: 2025/04/17 12:49:16 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/channel.hpp"

Channel::Channel(const std::string &name): _name(name) {};

const std::string &Channel::getName() const
{
	return (_name);
}

const std::vector<Client*> &Channel::getMembers() const
{
	return (_members);
}


void Channel::setName(const std::string &name)
{
	_name = name;
}

void Channel::addMember(Client *client)
{
	_members.push_back(client);
}

void Channel::removeMember(Client *client)
{
	_members.erase(
		std::remove(_members.begin(), _members.end(), client),
		_members.end()
	);
}

void Channel::addOperator(Client *client)
{
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
