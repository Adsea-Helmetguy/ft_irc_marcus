/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/18 15:31:38 by gyong-si          #+#    #+#             */
/*   Updated: 2025/03/25 13:31:59 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

int main(int ac, char **av)
{
	if (ac == 3)
	{
		try
		{
			Server server(av[1], av[2]);
			setupSignalHandler();
			server.runServer();
		}
		catch (const std::exception &e)
		{
			std::cerr << RED << "Server Function side failed: " 
				<< e.what() << RT << std::endl;
			return (-1);
		}
	}
	else
		std::cerr << "Usage: ./ircserv <port> <password>" << std::endl;
	return (0);
}
