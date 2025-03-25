#include <iostream>

class Client
{
	private:
		int 		_fd;
		std::string _ip_addr;

	public:
		Client() {};
		~Client() {};
		const int &getFd() const;
		void set_fd(int &fd);
		void set_ip(std::string ip_addr);
};
