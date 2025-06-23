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
		bool						_inviteOnly;
		std::vector<int>			_inviteList;
		bool						_topicRestricted;
		size_t						_channellimitSize;
		bool						_channelIslimited;
		std::string					_topicSetter;
		time_t						_topicTime;


	public:
		Channel(const std::string &name, const std::string &password);
		~Channel() {};

		const std::string &getName() const;
		const std::string &getTopic();
		const std::vector<ChannelUser> &getUsers() const;
		size_t getUsersSize() const;
		std::string getClientList();
		const std::string &getPassword() const;
		const std::string &getCreationTime() const;

		void setName(const std::string &name);
		void setPassword(const std::string &password);

		bool isMember(Client *client);
		void addMember(Client *client);
		bool checkNickNameUsed(const std::string &nickname);

		void addOperator(Client *client);

		bool hasPassword() const;
		void removeUser(Client *client);

		void broadcast(const std::string &message, const Client *exclude);
		void broadcast(const std::string &message);

		bool kickUser(Client *operator_client, Client *target_client, const std::string &reason);
		void		setTopic(const std::string& topic);
		void		setTopicSetter(const std::string& setter);
		void		setTopicTime(time_t timestamp);
		std::string getTopic() const;
		std::string getTopicSetter() const;
		time_t		getTopicTime() const;
		std::string getTopicTimeString() const;

		void	SetInviteOnly(bool enable_invite);
		bool	getchannelIsInviteOnly() const;
		void	inviteClient(int clientFd);
		bool	getisClientInvited(int clientFd) const;
		void	removeInvite(int clientFd);
		void	clearInviteList();
		void	setTopicRestriction(bool setTopic);
		bool	getisTopicRestricted() const;
		void		setchannelPassword(std::string password);
		void		removechannelPassword();
		std::string	getchannelPassword() const;
		void	OperatorTrue(std::list<std::string>::iterator &it, bool &print_success);
		void	OperatorFalse(std::list<std::string>::iterator &it, bool &print_success);
		bool	getUserOperator_status(ChannelUser user);
		bool	isOperator(Client *client) const;
		void	limitSet(std::list<std::string>::iterator &it, bool &print_success);
		void	limitUnset(bool &print_success);
		bool	IsChannelLimited() const;
		size_t	getchannelLimit() const;
};
