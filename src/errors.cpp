/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:36:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/13 12:58:59 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/errors.hpp"

// 421
std::string ERR_UNKNOWNCOMMAND(const std::string &serverName, const std::string &clientNick, const std::string &cmd)
{
	return (":" + serverName + " 421 " + clientNick + " " + cmd + " :Unknown command" + CRLF);
}

// 461
std::string ERR_NEEDMOREPARAMS(const std::string &serverName, const std::string &clientNick, const std::string &cmd)
{
	return (":" + serverName + " 461 " + clientNick + " " + cmd + " :Not enough parameters" + CRLF);
}

// 403
std::string ERR_NOSUCHCHANNEL(const std::string &serverName, const std::string &clientNick, const std::string &channelName)
{
	return (":" + serverName + " 403 " + clientNick + " " + channelName + " :No such channel" + CRLF);
}

// 442
std::string ERR_NOTONCHANNEL(const std::string &serverName, const std::string &clientNick, const std::string &channelName)
{
	return (":" + serverName + " 442 " + clientNick + " " + channelName + " :You're not on that channel" + CRLF);
}
