/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/19 10:19:08 by gyong-si          #+#    #+#             */
/*   Updated: 2025/06/23 14:23:38 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"

void Server::handlePass(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 2)
	{
		send(fd, "ERROR :PASS command requires exactly one argument\r\n", 50, 0);
		return ;
	}
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	const std::string &provided = *it;
	if (_password == sha256(provided))
	{
		client->authenticate();
		send(fd, "NOTICE AUTH :Password accepted\r\n", 33, 0);
		std::cout << "Client " << fd << " : has been authenticated.\n";
	}
	else
		send(fd, "ERROR :Invalid password\r\n", 26, 0);
}

void Server::handleNick(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() != 2)
	{
		send(fd, "ERROR :PASS command requires exactly one argument\r\n", 50, 0);
		return ;
	}
	if (!client->is_authenticated())
	{
		const std::string &errorMsg = "ERROR :You must authenticate with PASS first\r\n";
		sendError(fd, errorMsg);
		std::cout << "[WARN] Client " << fd << " tried to send NICK/USER before PASS\r\n";
		return ;
	}
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	std::string newNick = *it;
	if (newNick.length() > 9)
	{
		sendError(fd, "ERROR :Nickname too long\r\n");
		return;
	}
	std::string oldNick = client->getNick();
	client->set_nick(newNick);
	std::cout << "[NICK] " << oldNick << " changed to " << newNick << std::endl;
	std::string nickReply = ":" + oldNick + "!" + client->getUserName() + "@" +
	client->getHostName() + " NICK :" + newNick + CRLF;
	sendReply(fd, nickReply);
}

void Server::handleUser(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_list.size() < 5)
	{
		const std::string &errorMsg = "ERROR :USER command requires four argument. Eg: USER username 0 * 0 *\r\n";
		sendError(fd, errorMsg);
		return ;
	}
	if (!client->is_authenticated())
	{
		const std::string &errorMsg = "ERROR :You must authenticate with PASS first\r\n";
		sendError(fd, errorMsg);
		std::cout << "[WARN] Client " << fd << " tried to send NICK/USER before PASS\n";
		return ;
	}
	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	std::string username = *it;
	client->set_username(username);
	++it;++it;
	std::string hostname = *it;

	client->set_hostname(hostname);
	std::cout << "[USER] : " << username << ", " << "[USER] : "<< hostname  << std::endl;
	if (!client->getNick().empty() && !client->getUserName().empty()
		&& !client->getHostName().empty() && !client->is_registered())
	{
		client->register_client();
		sendWelcome(client);
	}
}

void Server::handleJoin(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;

	if (cmd_list.size() < 2 || cmd_list.size() > 3)
	{
		send(fd, "ERROR :JOIN command requires 1 or 2 arguments\r\n", 47, 0);
		return ;
	}

	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	const std::string channelName = *it;
	std::string password = "";
	if (++it != cmd_list.end())
		password = *it;
	Channel *channel = getChannelByName(channelName);

	std::string serverName = this->getName();
	std::string userNick = client->getNick();
	std::string userName = client->getUserName();
	std::string userHost = client->getHostName();
	if (!channel)
	{
		_channels.push_back(Channel(channelName, password));
		channel = &_channels.back();
		channel->addOperator(client);
		std::cout << "[INFO] New channel " << channelName
				  << " created by " << client->getNick() << "\r\n";
		if (!password.empty())
			std::cout << channelName << " is a password protected channel" << "\r\n";
		std::string clientList = channel->getClientList();
		sendReply(fd,
		RPL_JOINMSG(userNick, userName, userHost, channelName) +
		RPL_TOPIC(serverName, userNick, channelName, channel->getTopic()) +
		RPL_CREATIONTIME(serverName, userNick, channelName, channel->getCreationTime()) +
		RPL_NAMEREPLY(serverName, userNick, channelName, clientList) +
		RPL_ENDOFNAMES(serverName, userNick, channelName));
		channel->broadcast(RPL_JOINMSG(userNick, userName, userHost, channelName), client);
	}
	else
	{
		if (!channel->isMember(client))
		{
			if (channel->getchannelIsInviteOnly() == true)
			{
				std::cout << RED << "[DEBUG] I am channel is invite only!" << RT << std::endl;
				if (channel->getisClientInvited(fd) == false)
				{
					std::cout << RED << "Error: " << userNick 
						<< " tried to join but was not invited" << RT << std::endl;
					sendError(fd, ERR_INVITEONLYCHAN(serverName, userNick, channelName));
					return ;
				}
			}
			if (channel->IsChannelLimited() == true)
			{
				if (channel->getUsersSize() >= channel->getchannelLimit())
				{
					sendError(fd, ERR_CHANNELISFULL(serverName, userNick, channelName));
					return ;
				}
				std::cout << RED << "[DEBUG] MAN THE SIZE IS CURRENTLY-> " << RT << 
						channel->getUsersSize() << RED << " WHILE THE LIMITSIZE IS-> " << RT 
						<< channel->getchannelLimit() << std::endl;
			}
			if (channel->hasPassword() && channel->getPassword() != password)
			{
				std::cout << "Error: " << userNick << " tried to join without a correct password" << std::endl;
				sendError(fd, ERR_BADCHANNELKEY(serverName, userNick, channelName));
				return ;
			}
			channel->addMember(client);

			if (channel->getchannelIsInviteOnly())
            {
                std::cout << YELLOW << "[DEBUG] Removing " << userNick << " (fd: " << fd 
                          << ") from invite list after successful join" << RT << std::endl;
                channel->removeInvite(fd);
                std::cout << GREEN << "[DEBUG] Successfully removed from invite list" << RT << std::endl;
            }

			std::cout << "[JOIN] " << userNick << " joined " << channelName << "\n";
			std::cout << "[USERS] " << channel->getClientList() << "\n";

			std::string clientList = channel->getClientList();
			sendReply(fd,
				RPL_JOINMSG(userNick, userName, userHost, channelName) +
				RPL_TOPIC(serverName, userNick, channelName, channel->getTopic()) +
				RPL_CREATIONTIME(serverName, userNick, channelName, channel->getCreationTime()) +
				RPL_NAMEREPLY(serverName, userNick, channelName, clientList) +
				RPL_ENDOFNAMES(serverName, userNick, channelName));
			std::cout << RPL_JOINMSG(userNick, userName, userHost, channelName) << std::endl;
			channel->broadcast(RPL_JOINMSG(userNick, userName, userHost, channelName), client);
		}
	}
}

void	Server::handleMode(int fd, std::list<std::string> cmd_lst)
{
	Client	*client = getClientByFd(fd);
	if (!client)
		return ;
	if (cmd_lst.size() < 3)
	{
		std::cout << RED << "[DEBUG] cmd_lst.size() = " << cmd_lst.size() << RT << std::endl;
		return;
	}

	std::cout << RED << "cmd_list size-> " << YELLOW << cmd_lst.size() << RT << std::endl;
	std::list<std::string>::iterator	it = cmd_lst.begin();
	if (++it == cmd_lst.end())
		return;
	std::string	hash_and_channelName;
	hash_and_channelName.clear();
	if (!(*it).empty())
		hash_and_channelName = *it;
	if (hash_and_channelName.empty() || hash_and_channelName[0] != '#')
		return;

	Channel	*targetChannel = NULL;
	for (size_t i = 0; i < _channels.size(); ++i)
	{
		std::cout << "[DEBUG] Comparing _channel: " << GREEN << _channels[i].getName() << RT << std::endl;
		if (_channels[i].getName() == hash_and_channelName)
		{
			targetChannel = &_channels[i];
			std::cout << "[DEBUG] FOUND CHANNEL = " << GREEN << targetChannel->getName() << RT << std::endl;
			break;
		}
	}

	if (!targetChannel)
	{
		std::cout << GREEN << "[DEBUG] No such channel friend." << RT << std::endl;
		sendReply(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), hash_and_channelName));
		return;
	}

	if (!targetChannel->isMember(client))
	{
		std::cout << GREEN << "[DEBUG] Member is not in channel." << RT << std::endl;
		sendReply(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), targetChannel->getName()));
		return;
	}
	if (!targetChannel->isOperator(client))
	{
		std::cout << GREEN << "[DEBUG] U ain't the operator. GET OUTTA HERE!" << RT << std::endl;
		sendReply(fd, ERR_CHANOPRIVSNEEDED(getName(), client->getNick(), targetChannel->getName()));
		return;
	}

	if (++it == cmd_lst.end())
		return;
	std::string	modeCommand;
	if (!(*it).empty())
	{
		modeCommand = *it;
		std::cout << YELLOW << "VALUE OF MODECOMMAND = " << RT << modeCommand << std::endl;
	}
	if (modeCommand.empty() || modeCommand.size() < 2)
	{
		std::cout << RED << "[DEBUG] Your Command for mode is not enough or empty." << RT << std::endl;
		return;
	}

	char	operation = '\0';

	for (size_t i = 0; i < 2; i++)
	{
		if (!modeCommand.empty() && (modeCommand[i] == '+' || modeCommand[i] == '-'))
		{
			operation = modeCommand[i];
			continue ;
		}
		if (i != 0 && modeCommand[i] && operation)
		{
			std::cout << YELLOW << "[DEBUG] (operation = \"" RT << operation << YELLOW 
				<< "\" | Value of this = \"" << RT << modeCommand[i] 
				<< YELLOW << "\")" << RT << std::endl;
			std::cout << RED << "modeCommand.size() = " << RT << modeCommand.size() << std::endl;
			if (modeCommand[i] == 'i' && modeCommand.size() == 2)
				invite_only(targetChannel , operation, *client);
			else if (modeCommand[i] == 't' && modeCommand.size() == 2)
				topic_restriction(targetChannel, operation, *client);
			else if (modeCommand[i] == 'k')
			{
				if (++it != cmd_lst.end())
					channel_password(targetChannel, operation, it, *client);
			}
			else if (modeCommand[i] == 'o')
			{
				if (++it != cmd_lst.end())
					operator_addon(targetChannel, operation, it, *client);
			}
			else if (modeCommand[i] == 'l')
				user_limit(targetChannel, operation, it, cmd_lst, *client);
		}
	}
	std::cout << GREEN << "[SUCCESS] END of the LINE! FINISH!" << RT << std::endl;
}

void	Server::handlePing(int fd, std::list<std::string> cmd_lst)
{
	if (cmd_lst.size() < 2)
	{
		sendError(fd, "ERROR :No ping argument\r\n");
		return;
	}
	std::list<std::string>::iterator it = cmd_lst.begin();
	++it;
	std::string token = *it;
	sendReply(fd, RPL_PONG(token));
	std::cout << "[PING] Replied with: " << RPL_PONG(token);
}

void	Server::handlePart(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
		return ;

	if (cmd_list.size() == 1)
	{
		sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), cmd_list.front()));
		std::cout << "[PART] Not enough parameters" << std::endl;
		return ;
	}

	std::list<std::string>::const_iterator it = cmd_list.begin();
	++it;
	const std::string channelName = *it;
	std::string reason = "";
	if (cmd_list.size() > 2)
	{
		++it;
		while (it != cmd_list.end())
		{
			if (!reason.empty())
			{
				reason += " ";
			}
			reason += *it;
			++it;
		}
	}
	std::cout << "Reason" << std::endl;
	std::cout << reason << std::endl;
	Channel *channel = getChannelByName(channelName);

	if (!channel)
	{
		sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
		std::cout << "[PART] Channel " << channelName << " not found" << std::endl;
		return;
	}
	if (!channel->isMember(client) && !channel->isOperator(client))
	{
		sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
		std::cout << "[PART] Client not in channel " << channelName << std::endl;
		return;
	}

	std::string partMsg = ":" + client->getPrefix() + " PART " + channelName;
	if (!reason.empty())
		partMsg += " " + reason;
	else
		partMsg + " :";
	partMsg += "\r\n";

	std::cout << "DEBUG PART MSG: [" << partMsg << "]" << std::endl;
	sendReply(client->getFd(), partMsg);
	channel->broadcast(partMsg, client);
	channel->removeUser(client);

	if (channel->getUsers().empty())
	{
		removeChannel(channel->getName());
		sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
	}
	else
		sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
}

void	Server::handlePrivmsg(int fd, std::list<std::string> cmd_list)
{
	Client *client = getClientByFd(fd);
	if (!client)
	{
		std::cout << "[PRIVMSG] No client found for fd: " << fd << std::endl;
		return ;
	}
	if (cmd_list.size() < 3)
	{
		sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), cmd_list.front()));
		std::cout << "[PRIVMSG] Not enough parameters" << std::endl;
		return ;
	}
	std::list<std::string>::iterator it = cmd_list.begin();
	++it;
	std::string target = *it++;
	std::string message;
	while (it != cmd_list.end())
	{
		if (!message.empty())
			message += " ";
		message += *it++;
	}
	if (message[0] == ':')
		message = message.substr(1);

	if (target[0] == '#')
	{
		Channel *channel = getChannelByName(target);
		if (!channel)
		{
			sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), target));
			return ;
		}
		if (!channel->isMember(client))
		{
			sendError(fd, ERR_CANNOTSENDTOCHAN(getName(), client->getNick(), channel->getName()));
			return ;
		}
		std::string out = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + CRLF;
		std::cout << out << std::endl;
		channel->broadcast(out, client);
	}
	else
	{
		Client* targetUser = getClientByNick(target);
		if (!targetUser)
		{
			ERR_NOSUCHNICK(getName(), client->getNick(), target);
			return ;
		}
		std::string out = ":" + client->getPrefix() + " PRIVMSG " + target + " :" + message + CRLF;
		std::cout << out << std::endl;
		sendReply(targetUser->getFd(), out);
	}
}
