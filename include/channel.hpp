#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "../include/client.hpp"
#include "../include/utils.hpp"

class Channel
{
	private:
		std::string 			_name;
		std::vector<Client*>	_members;
		std::vector<Client*>	_operators;
		std::string				_topic;

	public:
		Channel(const std::string &name);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::vector<Client*> &getMembers() const;
		const std::vector<Client*> &getOperators() const;
		const std::string &getTopic();
		std::string getClientList();
		// setters
		void setName(const std::string &name);

		// add members
		void addMember(Client *client);
		void removeMember(Client *client);
		bool isMember(Client *client);
		// add operators
		void addOperator(Client *client);
		void removeOperator(Client *client);
		bool isOperator(Client *client) const;

		void broadcast(const std::string &message, Client *exclude);
};
