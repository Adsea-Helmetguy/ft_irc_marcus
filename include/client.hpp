#pragma once

#include <iostream>

class Client
{
	private:
		int 		_fd;
		std::string _ip_addr;
		std::string _nick;
		std::string _username;
		std::string _hostname;
		bool		_authenticated;

	public:
		Client(int client_fd, std::string &client_ip);
		~Client() {};
		const int &getFd() const;
		void set_fd(int &fd);
		void set_ip(std::string ip_addr);
		void set_nick(std::string &nick);
		void set_username(std::string &username);
		void set_hostname(std::string &hostname);
		void authenticate();
		bool is_authenticated();
};
