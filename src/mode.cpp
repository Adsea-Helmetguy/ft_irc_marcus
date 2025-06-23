#include "../include/channel.hpp"

void Channel::SetInviteOnly(bool enable_invite)
{
	if (enable_invite == true && this->_inviteOnly != true)
	{
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

void	Channel::setTopicRestriction(bool setTopic)
{
	this->_topicRestricted = setTopic;
}

bool	Channel::getisTopicRestricted() const
{
	return (this->_topicRestricted);
}

void	Channel::setchannelPassword(std::string password)
{
	this->_password = password;
}

void	Channel::removechannelPassword()
{
	if (!this->_password.empty())
		this->_password.clear();
}

std::string	Channel::getchannelPassword() const
{
	return (this->_password);
}

void	Channel::OperatorTrue(std::list<std::string>::iterator &it, bool &print_success)
{
	std::cout << RED << "VALUE OF IT = \"" << RT << *it << RED << "\"" << RT << std::endl;
	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (this->_users[i].client->getNick() == *it)
		{
			this->_users[i].isOperator = true;
			std::cout << "Operator \"" << this->getUserOperator_status(this->_users[i]) 
					<< "\"" << RT << std::endl;
			print_success = true;
			return;
		}
	}
	std::cout << RED << "Dude can't be found here." << RT << std::endl;
}

void	Channel::OperatorFalse(std::list<std::string>::iterator &it, bool &print_success)
{
	std::cout << RED << "VALUE OF IT = \"" << RT << *it << RED << "\"" << RT << std::endl;
	for (size_t i = 0; i < _users.size(); ++i)
	{
		if (this->_users[i].client->getNick() == *it)
		{
			this->_users[i].isOperator = false;

			std::cout << "Operator \"" << this->getUserOperator_status(this->_users[i]) 
					<< "\"" << RT << std::endl;
			print_success = true;
			return;
		}
	}
	std::cout << RED << "Dude can't be found here." << RT << std::endl;
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

void	Channel::limitSet(std::list<std::string>::iterator &it, bool &print_success)
{
	if (isNumber(*it) == false)
	{
		std::cout << RED << "A NON-NUMBER! RETURNED!" << RT << std::endl;
		return;
	}

	std::stringstream	value1;
	int					limitset;
	value1.clear();

	value1.str(*it);
	value1 >> limitset;
	std::cout << GREEN << "Value of limitset = " << RT << limitset << std::endl;

	this->_channelIslimited = true;
	this->_channellimitSize = limitset;
	print_success = true;
}

void	Channel::limitUnset(bool &print_success)
{
	this->_channelIslimited = false;
	print_success = true;
}

bool	Channel::IsChannelLimited() const
{
	return (this->_channelIslimited);
}

size_t	Channel::getchannelLimit() const
{
	return (this->_channellimitSize);
}

