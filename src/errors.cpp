/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:36:35 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/12 12:47:27 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors.hpp"

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
