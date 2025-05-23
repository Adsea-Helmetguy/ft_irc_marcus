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
		std::string 				_name;
		std::vector<ChannelUser>	_users;
		std::string					_topic;
		std::string					_password;
		std::string					_created_time;
		bool						_inviteOnly;// -marcus-
		std::vector<int>			_inviteList;// -marcus- 

	public:
		Channel(const std::string &name, const std::string &password);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::string &getTopic();
		const std::vector<ChannelUser> &getUsers() const;
		std::string getClientList();
		const std::string &getPassword() const;
		const std::string &getCreationTime() const;
		// setters
		void setName(const std::string &name);
		void setPassword(const std::string &password);
		// add members
		bool isMember(Client *client);
		void addMember(Client *client);
		// add operators
		void addOperator(Client *client);

		bool isOperator(Client *client) const;
		bool hasPassword() const;
		void removeUser(Client *client);

		void broadcast(const std::string &message, const Client *exclude);
		void broadcast(const std::string &message);


		//for mode -marcus-:
		void	SetInviteOnly(bool enable_invite);
		bool	getchannelIsInviteOnly() const;
			// Invite list methods
			void	inviteClient(int clientFd);
			bool	getisClientInvited(int clientFd) const;
			void	removeInvite(int clientFd);
			void	clearInviteList(); // Optional, maybe on mode -i
		//for mode -marcus-:
};
