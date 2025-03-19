/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:31:38 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/19 10:28:19 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main(int ac, char **av)
{
	if (ac == 3)
	{
		Server server(av[1], av[2]);
		// main loop will wait for connections
		server.runServer();
	}
	else
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
	return (0);
}
