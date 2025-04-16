#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "../include/client.hpp"

class Channel
{
	private:
		std::string 		_name;
		std::vector<Client*> _members;

	public:
		Channel(const std::string &name);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::vector<Client*> &getMembers() const;
		// setters
		void setName(const std::string &name);

		void addMember(Client *client);
		void removeMember(Client *client);
};
