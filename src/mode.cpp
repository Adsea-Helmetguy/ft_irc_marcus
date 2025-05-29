#include "../include/channel.hpp"

// -marcus-
//|--------------------------------------|
//|             -INVITE-                 |
//|--------------------------------------|
void Channel::SetInviteOnly(bool enable_invite, int fd)
{
	(void)fd;
	if (enable_invite == true && this->_inviteOnly != true)
	{
		//clear the invite list before making the channel into a invite only channel
		this->clearInviteList();
		std::cout << YELLOW << "[DEBUG] Checking if list is cleared" << RT << std::endl;
		if (this->_inviteList.empty())
		{
			std::cout << GREEN << "[SUCCESS] List is cleared!" << RT << std::endl;
		}
		else
			std::cout << RED << "[DEBUG] Failed to clear list? WHY?" << RT << std::endl;
	}
	this->_inviteOnly = enable_invite;
	//sendReply(fd, "mode/" + targetChannel->getName() + " [+i] by client " + client->getNick());
}

bool	Channel::getchannelIsInviteOnly() const
{
	return (this->_inviteOnly);
}

void	Channel::inviteClient(int clientFd)
{
	if (std::find(_inviteList.begin(), _inviteList.end(), clientFd) == _inviteList.end())
		this->_inviteList.push_back(clientFd);
}

bool	Channel::getisClientInvited(int clientFd) const
{
	return (std::find(_inviteList.begin(), _inviteList.end(), clientFd) != _inviteList.end());
}

// once you joined after invite you can't join back unless invited again
void	Channel::removeInvite(int clientFd)
{
	std::vector<int>::iterator it = std::find(_inviteList.begin(), _inviteList.end(), clientFd);
	if (it != _inviteList.end())
		this->_inviteList.erase(it);
}

void	Channel::clearInviteList()
{
	this->_inviteList.clear();
}




//|--------------------------------------|
//|              -TOPIC-                 |
//|--------------------------------------|
void	Channel::setTopicRestriction(bool setTopic, int fd)
{
	(void)fd;
	this->_topicRestricted = setTopic;
	//sendReply(fd, "mode/" + targetChannel->getName() + " [+t/-t] by client " + client->getNick());
}

bool	Channel::getisTopicRestricted() const
{
	return (this->_topicRestricted);
}


//|--------------------------------------|
//|            -PASSWORD-                |
//|--------------------------------------|
void	Channel::setchannelPassword(std::string password, int fd)
{
	(void)fd;
	this->_password = password;
	//sendReply(fd, "mode/" + this->getName() + " [+k] by client " + client->getNick());
}

void	Channel::removechannelPassword(int fd)
{
	(void)fd;
	if (!this->_password.empty())
		this->_password.clear();
	//sendReply(fd, "mode/" + this->getName() + " [-k] by client " + client->getNick());
}

std::string	Channel::getchannelPassword() const
{
	return (this->_password);
}

//|--------------------------------------|
//|            -OPERATOR-                |
//|--------------------------------------|
void	Channel::OperatorTrue(std::list<std::string>::iterator &it)
{
	std::cout << RED << "VALUE OF IT = \"" << RT << *it << RED << "\"" << RT << std::endl;
	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (this->_users[i].client->getNick() == *it)
		{
			this->_users[i].isOperator = true;
			std::cout << "Operator \"" << this->getUserOperator_status(this->_users[i]) 
					<< "\"" << RT << std::endl;
			return;
		}
	}
	std::cout << RED << "Dude can't be found here." << RT << std::endl;
	//reply guide ask for help
	//ERR_USERNOTINCHANNEL
}

void	Channel::OperatorFalse(std::list<std::string>::iterator &it)
{
	std::cout << RED << "VALUE OF IT = \"" << RT << *it << RED << "\"" << RT << std::endl;
	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (this->_users[i].client->getNick() == *it)
		{
			this->_users[i].isOperator = false;

			std::cout << "Operator \"" << this->getUserOperator_status(this->_users[i]) 
					<< "\"" << RT << std::endl;
			return;
		}
	}
	std::cout << RED << "Dude can't be found here." << RT << std::endl;
	//reply guide ask for help
	//ERR_USERNOTINCHANNEL
}

bool	Channel::getUserOperator_status(const ChannelUser user)
{
	for (size_t i = 0; i < this->_users.size(); ++i)
	{
		if (this->_users[i].client->getUserName() == user.client->getUserName())
			return (this->_users[i].isOperator);
	}
	return (user.isOperator);
}

bool	Channel::isOperator(Client *client) const
{
	for (size_t i = 0; i < this->_users.size(); ++i)
	{
		if (this->_users[i].client == client && this->_users[i].isOperator)
			return (true);
	}
	return (false);
}


//|--------------------------------------|
//|           -USER LIMIT-               |
//|--------------------------------------|
void	Channel::limitSet(std::list<std::string>::iterator &it)
{
	//check if its a number first
	if (isNumber(*it) == false)
	{
		std::cout << RED << "A NON-NUMBER! RETURNED!" << RT << std::endl;
		return;
	}
	//it will contain the value of the limit we need to set.
	std::stringstream	value1;
	int					limitset;
	value1.clear();

	value1.str(*it);
	value1 >> limitset;
	std::cout << GREEN << "Value of limitset = " << RT << limitset << std::endl;

	//Once the moment you got the number, update the channel's values
	this->_channelIslimited = true;
	this->_channellimitSize = limitset;
}

void	Channel::limitUnset()
{
	this->_channelIslimited = false;
}

bool	Channel::IsChannelLimited() const
{
	return (this->_channelIslimited);
}

size_t	Channel::getchannelLimit() const
{
	return (this->_channellimitSize);
}

