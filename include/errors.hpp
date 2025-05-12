/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/12 12:36:25 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/12 12:46:10 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define once

#include "iostream"

#define CRLF "\r\n"

// 421
std::string ERR_UNKNOWNCOMMAND(const std::string &serverName, const std::string &clientNick, const std::string &cmd);
// 461
std::string ERR_NEEDMOREPARAMS(const std::string &serverName, const std::string &clientNick, const std::string &cmd);
