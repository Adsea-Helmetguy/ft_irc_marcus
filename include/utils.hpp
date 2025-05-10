/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 14:56:17 by gyong-si          #+#    #+#             */
/*   Updated: 2025/05/10 11:07:28 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <iostream>
#include <list>
#include "server.hpp"

class Server;

bool isValidPort(const char *portStr);
void setupSignalHandler();
std::list<std::string> splitString(std::string &cmd);
void sendError(int fd, const std::string &message);
std::string getFormattedTime() ;
