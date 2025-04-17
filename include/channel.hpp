#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "../include/client.hpp"

class Channel
{
	private:
		std::string 			_name;
		std::vector<Client*>	_members;
		std::vector<Client*>	_operators;

	public:
		Channel(const std::string &name);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::vector<Client*> &getMembers() const;
		// setters
		void setName(const std::string &name);

		// add members
		void addMember(Client *client);
		void removeMember(Client *client);
		// add operators
		void addOperator(Client *client);
		void removeOperator(Client *client);
		bool isOperator(Client *client) const;
};
