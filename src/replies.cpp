/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 21:19:57 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/18 11:48:23 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "replies.hpp"
#include "utils.hpp"

// Builds 001 string
std::string RPL_WELCOME(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 001 " + clientNick + " :Welcome to the IRC server!" + CRLF);
}

// Builds 002 string
std::string RPL_YOURHOST(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 002 " + clientNick +
		" :Your host is " + serverName + ", running version 1.0" + CRLF);
}

// Builds 003 string
std::string RPL_CREATED(const std::string &serverName, const std::string &clientNick, const std::string &createdTime)
{
	return (":" + serverName + " 003 " + clientNick + " :This server was created on " + createdTime + CRLF);
}

// Builds 004 string
std::string RPL_MYINFO(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 004 " + clientNick + " " + serverName + " 1.0 " + CRLF);
}

// 329 Channel Creation time
std::string RPL_CREATIONTIME(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &timestamp)
{
	return (":" + serverName + " 329 " + clientNick + " " + channelName + " " + timestamp + CRLF);
}

// Builds 375 string Start of Message of The Day
std::string RPL_MOTDSTART(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 375 " + clientNick + " :- " + serverName + " This is ft-irc!" + CRLF);
}

// 372 Contains a line of the MOTD
std::string RPL_MOTD(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 372 " + clientNick + " :- Welcome to the IRC server!" + CRLF);
}

// 376 End of MOTD
std::string RPL_ENDOFMOTD(const std::string &serverName, const std::string &clientNick)
{
	return (":" + serverName + " 376 " + clientNick + " :End of /MOTD command." + CRLF);
}

// 332 RPL_TOPIC
std::string RPL_TOPIC(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &topic)
{
	return (":" + serverName + " 332 " + clientNick + " " + channelName + " :" + topic + CRLF);
}

// Builds 353 NAMEREPLY string
std::string RPL_NAMEREPLY(const std::string &serverName, const std::string &clientNick, const std::string &channelName, std::string &clientList)
{
	return (":" + serverName + " 353 " + clientNick + " = " + channelName + " :" + clientList + CRLF);
}

// Builds 366 Indicate all the users nick sent out
std::string RPL_ENDOFNAMES(const std::string &serverName, const std::string &clientNick, const std::string &channelName)
{
	return (":" + serverName + " 366 " + clientNick + " " + channelName + " :End of /NAMES list." + CRLF);
}

// join room message
std::string RPL_JOINMSG(const std::string &clientNick, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName)
{
	return (":" + clientNick + "!" + clientUsername + "@" + clientHostname + " JOIN " + channelName + CRLF);
}

// pong reply
std::string RPL_PONG(const std::string token)
{
	return ("PONG " + token + CRLF);
}

// 333 -chanika- Marcus-> I dont think 333 exist.....
std::string RPL_TOPICWHOTIME(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &setter, time_t timestamp)
{
	std::ostringstream oss;
	oss << timestamp;
	return (":" + serverName + " 333 " + clientNick + " " + channelName + " " + setter + " " + oss.str() + CRLF);
}