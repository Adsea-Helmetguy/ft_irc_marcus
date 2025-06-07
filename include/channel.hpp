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
		bool						_topicRestricted;// -marcus-
		size_t						_channellimitSize;//chanika
		bool						_channelIslimited;//chanika
		std::string					_topicSetter;//chanika
		time_t						_topicTime;//chanika


	public:
		Channel(const std::string &name, const std::string &password);
		~Channel() {};

		// getters
		const std::string &getName() const;
		const std::string &getTopic();
		const std::vector<ChannelUser> &getUsers() const;
		size_t getUsersSize() const;
		std::string getClientList();
		const std::string &getPassword() const;
		const std::string &getCreationTime() const;
		// setters
		void setName(const std::string &name);
		void setPassword(const std::string &password);
		// add members
		bool isMember(Client *client);
		void addMember(Client *client);

		// add operator
		void addOperator(Client *client);

		bool hasPassword() const;
		void removeUser(Client *client);

		void broadcast(const std::string &message, const Client *exclude);
		void broadcast(const std::string &message);


		// -chanika-
			// channel operator commands
			bool kickUser(Client *operator_client, Client *target_client, const std::string &reason);
			//Topic-related methods 
			void		setTopic(const std::string& topic);
			void		setTopicSetter(const std::string& setter);
			void		setTopicTime(time_t timestamp);
			std::string getTopic() const;
			std::string getTopicSetter() const;
			time_t		getTopicTime() const;
			std::string getTopicTimeString() const;
		// -chanika-


		//for mode -marcus-:
			// INVITE (i)
			void	SetInviteOnly(bool enable_invite);
			bool	getchannelIsInviteOnly() const;
			void	inviteClient(int clientFd);
			bool	getisClientInvited(int clientFd) const;
			void	removeInvite(int clientFd);
			void	clearInviteList(); // Optional, maybe on mode -i
			// TOPIC (t)
			void	setTopicRestriction(bool setTopic);
			bool	getisTopicRestricted() const;
			// PASSWORD (k)
			void		setchannelPassword(std::string password);
			void		removechannelPassword();
			std::string	getchannelPassword() const;
			// OPERATOR PRIVILEGE (o)
			void	OperatorTrue(std::list<std::string>::iterator &it, bool &print_success);
			void	OperatorFalse(std::list<std::string>::iterator &it, bool &print_success);
			bool	getUserOperator_status(ChannelUser user);
			bool	isOperator(Client *client) const;
			// USER LIMIT (l)
			void	limitSet(std::list<std::string>::iterator &it, bool &print_success);
			void	limitUnset(bool &print_success);
			bool	IsChannelLimited() const;
			size_t	getchannelLimit() const;
		//for mode -marcus-:
};
