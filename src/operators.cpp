/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gyong-si <gyong-si@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 17:13:55 by cbandara          #+#    #+#             */
/*   Updated: 2025/06/23 14:28:14 by gyong-si         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/replies.hpp"

void Server::handleKick(int fd, std::list<std::string> cmd_list)
 {
     Client *client = getClientByFd(fd);
     if (!client || !client->is_authenticated())
     {
         sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
         return;
     }
 
     if (cmd_list.size() < 3)
     {
         sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "KICK"));
         return;
     }
 
     cmd_list.pop_front();
     std::string channelName = cmd_list.front();
     cmd_list.pop_front();
     std::string targetNick = cmd_list.front();
     cmd_list.pop_front();
     
     std::string reason = "Kicked from channel";
     if (!cmd_list.empty())
     {
         std::string firstPart = cmd_list.front();
         cmd_list.pop_front();
         
         if (!firstPart.empty() && firstPart[0] == ':')
             firstPart = firstPart.substr(1);
         
         reason = firstPart;
         while (!cmd_list.empty())
         {
             reason += " " + cmd_list.front();
             cmd_list.pop_front();
         }
     }
 
     Channel *channel = getChannelByName(channelName);
     if (!channel)
     {
         sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     if (!channel->isMember(client))
     {
         sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     if (!channel->isOperator(client))
     {
         sendError(fd, ERR_CHANOPRIVSNEEDED(getName(), client->getNick(), channelName));
         return;
     }
 
     Client *targetClient = getClientByNick(targetNick);
     if (!targetClient)
     {
         sendError(fd, ERR_NOSUCHNICK(getName(), client->getNick(), targetNick));
         return;
     }
 
     if (!channel->isMember(targetClient))
     {
         sendError(fd, ERR_USERNOTINCHANNEL(getName(), client->getNick(), targetNick, channelName));
         return;
     }
 
     channel->kickUser(client, targetClient, reason);
 }

void Server::handleInvite(int fd, std::list<std::string> cmd_list)
{
    std::cout << RED << "[DEBUG] *** HANDLE INVITE FUNCTION CALLED ***" << RT << std::endl;
    Client *client = getClientByFd(fd);
    if (!client || !client->is_authenticated())
    {
        sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
        return;
    }
    if (cmd_list.size() < 3)
    {
        sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "INVITE"));
        return;
    }
    cmd_list.pop_front();
    std::string targetNick = cmd_list.front();
    cmd_list.pop_front();
    std::string channelName = cmd_list.front();

    Client *targetClient = getClientByNick(targetNick);
    if (!targetClient)
    {
        sendError(fd, ERR_NOSUCHNICK(getName(), client->getNick(), targetNick));
        return;
    }
    std::cout << CYAN << "[DEBUG] Target client " << targetNick << " found with fd: " 
              << targetClient->getFd() << RT << std::endl;
    Channel *channel = getChannelByName(channelName);
    if (!channel)
    {
        sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
        return;
    }
    std::cout << CYAN << "[DEBUG] Channel " << channelName << " found. Is invite-only: " 
              << (channel->getchannelIsInviteOnly() ? "YES" : "NO") << RT << std::endl;

    if (!channel->isMember(client))
    {
        sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
        return;
    }
    if (channel->getchannelIsInviteOnly() && !channel->isOperator(client))
    {
        sendError(fd, ERR_CHANOPRIVSNEEDED(getName(), client->getNick(), channelName));
        return;
    }
    if (channel->isMember(targetClient))
    {
        sendError(fd, ERR_USERONCHANNEL(getName(), client->getNick(), targetNick, channelName));
        return;
    }
    std::cout << YELLOW << "[DEBUG] All checks passed. About to add client fd " 
              << targetClient->getFd() << " to invite list" << RT << std::endl;

    channel->inviteClient(targetClient->getFd());

    std::cout << GREEN << "[DEBUG] After inviting, checking invite list status..." << RT << std::endl;
    if (channel->getisClientInvited(targetClient->getFd()))
    {
        std::cout << GREEN << "[SUCCESS] Invite confirmed in list!" << RT << std::endl;
    }
    else
    {
        std::cout << RED << "[ERROR] Invite NOT found in list after adding!" << RT << std::endl;
    }

    sendReply(fd, RPL_INVITING(getName(), client->getNick(), targetNick, channelName));
    std::string inviteMessage = ":" + client->getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n";
    sendReply(targetClient->getFd(), inviteMessage);

    std::cout << GREEN << "[INFO] " << client->getNick() << " invited " << targetNick 
              << " to channel " << channelName << RT << std::endl;
}

 void Server::handleTopic(int fd, std::list<std::string> cmd_list)
 {
     Client *client = getClientByFd(fd);
     if (!client || !client->is_authenticated())
     {
         sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
         return;
     }
 
     if (cmd_list.size() < 2)
     {
         sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "TOPIC"));
         return;
     }
 
     cmd_list.pop_front();
     std::string channelName = cmd_list.front();
     cmd_list.pop_front();
 
     std::cout << CYAN << "[DEBUG] TOPIC command from " << client->getNick() 
               << " for channel " << channelName << RT << std::endl;
 
     Channel *channel = getChannelByName(channelName);
     if (!channel)
     {
         sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     if (!channel->isMember(client))
     {
         sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     if (cmd_list.empty())
     {
         std::cout << CYAN << "[DEBUG] Showing current topic for " << channelName << RT << std::endl;
         
         std::string currentTopic = channel->getTopic();
         if (currentTopic.empty())
         {
             sendReply(fd, RPL_NOTOPIC(getName(), client->getNick(), channelName));
         }
         else
         {
             sendReply(fd, RPL_TOPIC(getName(), client->getNick(), channelName, currentTopic));
             sendReply(fd, RPL_TOPICWHOTIME(getName(), client->getNick(), channelName, 
                      channel->getTopicSetter(), channel->getTopicTime()));
         }
         return;
     }
 
     std::string newTopic;
     
     bool first = true;
     while (!cmd_list.empty())
     {
         if (!first)
             newTopic += " ";
         newTopic += cmd_list.front();
         cmd_list.pop_front();
         first = false;
     }
 
     if (!newTopic.empty() && newTopic[0] == ':')
        newTopic.erase(0, 1);
    
     std::cout << CYAN << "[DEBUG] Attempting to set topic to: '" << newTopic << "'" << RT << std::endl;
 
     if (channel->getisTopicRestricted())
     {
         std::cout << YELLOW << "[DEBUG] Channel has topic restriction (+t mode)" << RT << std::endl;
         if (!channel->isOperator(client))
         {
             std::cout << RED << "[ERROR] " << client->getNick() 
                       << " is not an operator, cannot set topic" << RT << std::endl;
             sendError(fd, ERR_CHANOPRIVSNEEDED(getName(), client->getNick(), channelName));
             return;
         }
         else
         {
             std::cout << GREEN << "[SUCCESS] " << client->getNick() 
                       << " is an operator, can set topic" << RT << std::endl;
         }
     }
     else
     {
         std::cout << CYAN << "[DEBUG] Channel allows anyone to set topic (no +t mode)" << RT << std::endl;
     }
 
     std::string oldTopic = channel->getTopic();
     channel->setTopic(newTopic);
     channel->setTopicSetter(client->getNick());
 
     std::cout << GREEN << "[SUCCESS] Topic changed from '" << oldTopic 
               << "' to '" << newTopic << "' by " << client->getNick() << RT << std::endl;
 
     std::string topicChangeMsg = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
     channel->broadcast(topicChangeMsg, NULL);
 }
 


