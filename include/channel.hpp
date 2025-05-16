#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include "../include/client.hpp"
#include "../include/utils.hpp"

struct ChannelUser
{
	Client* client;
	bool	isOperator;

	ChannelUser(Client* c, bool isOp = false) : client(c), isOperator(isOp) {}
};

class Channel
{
	private:
		std::string 			_name;
		std::vector<ChannelUser> _users;
		std::string				_topic;

	public:
		Channel(const std::string &name);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::string &getTopic();
		const std::vector<ChannelUser> &getUsers() const;
		std::string getClientList();
		// setters
		void setName(const std::string &name);

		// add members
		bool isMember(Client *client);
		void addMember(Client *client);
		// add operators
		void addOperator(Client *client);

		bool isOperator(Client *client) const;
		void removeUser(Client *client);

		void broadcast(const std::string &message, const Client *exclude);
		void broadcast(const std::string &message);

};
