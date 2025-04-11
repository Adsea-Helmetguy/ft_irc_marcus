#pragma once
#include <iostream>

class Client
{
	private:
		int 		_fd;
		std::string _ip_addr;

	public:
		Client(int client_fd, std::string &client_ip);
		~Client() {};
		const int &getFd() const;
		void set_fd(int &fd);
		void set_ip(std::string ip_addr);
};
