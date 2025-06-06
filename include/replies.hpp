/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   replies.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/10 13:47:50 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/18 10:03:13 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define once

#include "iostream"

#define CRLF "\r\n"

//#define RPL_NAMEREPLY(serverName, clientNick, channelName, clientList) (":" + serverName + " 353 " + clientNick + " " + channelName + " :" + clientList + CRLF)
//#define RPL_ENDOFNAMES(serverName, channelName) (":" + serverName + " 366 " + clientNick + " " + channelName + " :End of /NAMES" + CRFL)

// 001
std::string RPL_WELCOME(const std::string &serverName, const std::string &clientNick);
// 002
std::string RPL_YOURHOST(const std::string &serverName, const std::string &clientNick);
// 003
std::string RPL_CREATED(const std::string &serverName, const std::string &clientNick, const std::string &createdTime);
// 004
std::string RPL_MYINFO(const std::string &serverName, const std::string &clientNick);
// 329
std::string RPL_CREATIONTIME(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &timestamp);
// 375
std::string RPL_MOTDSTART(const std::string &serverName, const std::string &clientNick);
// 372
std::string RPL_MOTD(const std::string &serverName, const std::string &clientNick);
// 376
std::string RPL_ENDOFMOTD(const std::string &serverName, const std::string &clientNick);
// 332
std::string RPL_TOPIC(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &topic);
// 353
std::string RPL_NAMEREPLY(const std::string &serverName, const std::string &clientNick, const std::string &channelName, std::string &clientList);
// 366
std::string RPL_ENDOFNAMES(const std::string &serverName, const std::string &clientNick, const std::string &channelName);

std::string RPL_JOINMSG(const std::string &clientNick, const std::string &clientUsername, const std::string &clientHostname, const std::string &channelName);

std::string RPL_PONG(const std::string token);

std::string RPL_TOPICWHOTIME(const std::string &serverName, const std::string &clientNick, const std::string &channelName, const std::string &setter, time_t timestamp);
