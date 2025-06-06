/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   operators.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbandara <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 17:13:55 by cbandara          #+#    #+#             */
/*   Updated: 2025/05/29 17:13:57 by cbandara         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/server.hpp"
#include "../include/replies.hpp"

/**
 * Handles the KICK command
 * Format: KICK <channel> <user> [reason]
 * Kicks a user from a specified channel
 */
 void Server::handleKick(int fd, std::list<std::string> cmd_list)
 {
     Client *client = getClientByFd(fd);
     if (!client || !client->is_authenticated())
     {
         sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
         return;
     }
 
     if (cmd_list.size() < 3) // KICK + channel + nickname (at minimum)
     {
         sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "KICK"));
         return;
     }
 
     cmd_list.pop_front(); // Remove "KICK" from the front of the list
     std::string channelName = cmd_list.front();
     cmd_list.pop_front();
     std::string targetNick = cmd_list.front();
     cmd_list.pop_front();
     
     // Collect reason if provided
     std::string reason = "Kicked from channel";
     if (!cmd_list.empty())
     {
         std::string firstPart = cmd_list.front();
         cmd_list.pop_front();
         
         // If reason starts with ':', remove it
         if (!firstPart.empty() && firstPart[0] == ':')
             firstPart = firstPart.substr(1);
         
         reason = firstPart;
         
         // Append the rest of the parameters to form the complete reason
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
 
     // Perform the kick operation
     if (channel->kickUser(client, targetClient, reason))
     {
         // The kickUser method handles sending messages to the channel members
     }
 }

 /**
 * Handles the INVITE command
 * Format: INVITE <nickname> <channel>
 * Invites a user to join a channel
 */
void Server::handleInvite(int fd, std::list<std::string> cmd_list)
{
    std::cout << RED << "[DEBUG] *** HANDLE INVITE FUNCTION CALLED ***" << RT << std::endl;
    Client *client = getClientByFd(fd);
    if (!client || !client->is_authenticated())
    {
        sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
        return;
    }
    if (cmd_list.size() < 3) // INVITE + nickname + channel
    {
        sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "INVITE"));
        return;
    }
    cmd_list.pop_front(); // Remove "INVITE"
    std::string targetNick = cmd_list.front();
    cmd_list.pop_front();
    std::string channelName = cmd_list.front();

    // Find the target client
    Client *targetClient = getClientByNick(targetNick);
    if (!targetClient)
    {
        sendError(fd, ERR_NOSUCHNICK(getName(), client->getNick(), targetNick));
        return;
    }
    std::cout << CYAN << "[DEBUG] Target client " << targetNick << " found with fd: " 
              << targetClient->getFd() << RT << std::endl;
    // Find the channel
    Channel *channel = getChannelByName(channelName);
    if (!channel)
    {
        sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
        return;
    }
    std::cout << CYAN << "[DEBUG] Channel " << channelName << " found. Is invite-only: " 
              << (channel->getchannelIsInviteOnly() ? "YES" : "NO") << RT << std::endl;


    // Check if the inviting user is on the channel
    if (!channel->isMember(client))
    {
        sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
        return;
    }
    // Check if channel is invite-only and user is not an operator
    if (channel->getchannelIsInviteOnly() && !channel->isOperator(client))
    {
        sendError(fd, ERR_CHANOPRIVSNEEDED(getName(), client->getNick(), channelName));
        return;
    }
    // Check if target is already on the channel
    if (channel->isMember(targetClient))
    {
        sendError(fd, ERR_USERONCHANNEL(getName(), client->getNick(), targetNick, channelName));
        return;
    }
    std::cout << YELLOW << "[DEBUG] All checks passed. About to add client fd " 
              << targetClient->getFd() << " to invite list" << RT << std::endl;

    // Add the target to the invite list
    channel->inviteClient(targetClient->getFd());

    std::cout << GREEN << "[DEBUG] After inviting, checking invite list status..." << RT << std::endl;
    // Verify the invite was added
    if (channel->getisClientInvited(targetClient->getFd()))
    {
        std::cout << GREEN << "[SUCCESS] Invite confirmed in list!" << RT << std::endl;
    }
    else
    {
        std::cout << RED << "[ERROR] Invite NOT found in list after adding!" << RT << std::endl;
    }

    // Send confirmation to the inviting user
    sendReply(fd, RPL_INVITING(getName(), client->getNick(), targetNick, channelName));
    // Send invite notification to the target user
    std::string inviteMessage = ":" + client->getPrefix() + " INVITE " + targetNick + " " + channelName + "\r\n";
    sendReply(targetClient->getFd(), inviteMessage);

    std::cout << GREEN << "[INFO] " << client->getNick() << " invited " << targetNick 
              << " to channel " << channelName << RT << std::endl;
}

/**
 * Handles the TOPIC command
 * Format: TOPIC <channel> [<topic>]
 * 
 * Without topic parameter: Shows current topic
 * With topic parameter: Sets new topic (if allowed)
 */
 void Server::handleTopic(int fd, std::list<std::string> cmd_list)
 {
     Client *client = getClientByFd(fd);
     if (!client || !client->is_authenticated())
     {
         sendError(fd, ERR_NOTREGISTERED(getName(), "*"));
         return;
     }
 
     if (cmd_list.size() < 2) // TOPIC + channel minimum
     {
         sendError(fd, ERR_NEEDMOREPARAMS(getName(), client->getNick(), "TOPIC"));
         return;
     }
 
     cmd_list.pop_front(); // Remove "TOPIC"
     std::string channelName = cmd_list.front();
     cmd_list.pop_front();
 
     std::cout << CYAN << "[DEBUG] TOPIC command from " << client->getNick() 
               << " for channel " << channelName << RT << std::endl;
 
     // Find the channel
     Channel *channel = getChannelByName(channelName);
     if (!channel)
     {
         sendError(fd, ERR_NOSUCHCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     // Check if user is on the channel
     if (!channel->isMember(client))
     {
         sendError(fd, ERR_NOTONCHANNEL(getName(), client->getNick(), channelName));
         return;
     }
 
     // If no topic parameter provided, show current topic
     if (cmd_list.empty())
     {
         std::cout << CYAN << "[DEBUG] Showing current topic for " << channelName << RT << std::endl;
         
         std::string currentTopic = channel->getTopic();
         if (currentTopic.empty())
         {
             // No topic set
             sendReply(fd, RPL_NOTOPIC(getName(), client->getNick(), channelName));
         }
         else
         {
             // Send current topic
             sendReply(fd, RPL_TOPIC(getName(), client->getNick(), channelName, currentTopic));
             // Optionally send topic creation time if you track it
             sendReply(fd, RPL_TOPICWHOTIME(getName(), client->getNick(), channelName, 
                      channel->getTopicSetter(), channel->getTopicTime()));
         }
         return;
     }
 
     // Topic parameter provided - trying to set new topic
     std::string newTopic;
     
     // Reconstruct the topic from remaining parameters (in case topic has spaces)
     bool first = true;
     while (!cmd_list.empty())
     {
         if (!first)
             newTopic += " ";
         newTopic += cmd_list.front();
         cmd_list.pop_front();
         first = false;
     }
 
     std::cout << CYAN << "[DEBUG] Attempting to set topic to: '" << newTopic << "'" << RT << std::endl;
 
     // Check if topic is restricted to operators only
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
 
     // Set the new topic
     std::string oldTopic = channel->getTopic();
     channel->setTopic(newTopic);
     channel->setTopicSetter(client->getNick());
     //channel->setTopicTime(time(NULL)); // Set current timestamp
 
     std::cout << GREEN << "[SUCCESS] Topic changed from '" << oldTopic 
               << "' to '" << newTopic << "' by " << client->getNick() << RT << std::endl;
 
     // Broadcast topic change to all channel members
     std::string topicChangeMsg = ":" + client->getPrefix() + " TOPIC " + channelName + " :" + newTopic + "\r\n";
     channel->broadcast(topicChangeMsg, NULL); // Send to everyone including the setter
 }
 


