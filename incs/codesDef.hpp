/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codesDef.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 17:43:45 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/22 15:44:59 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODESDEF_HPP
#define CODESDEF_HPP

#include <iostream>
#include <string>

#define ERR_NOSUCHNICK(source, target)			"401 " + source + " " + target + " :No such nickname / channel"
#define ERR_NOSUCHCHANNEL(source, target)		"403 " + source + " " + target + " :Channel name is invalid, or does not exist"
#define ERR_CANNOTSENDTOCHAN(source, target)		"404 " + source + " " + target + " :Cannot send a message to channel"
#define ERR_UNKNOWNCOMMAND(source, command)		"421 " + source + " " + command + " :Command unknown"
#define ERR_NONICKNAMEGIVEN(source, command)		"431 " + source + " " + command + " :No new nickname has been given"
#define ERR_ERRONEUSNICKNAME(source)			"432 " + source + " :This nickname is Invalid" 
#define ERR_NICKNAMEINUSE(source)			"433 " + source + " :This nickname is already in use"
#define ERR_USERNOTINCHANNEL(source, target, channel) 	"441 " + source + " " + target + " " + channel + " :They aren't on that channel"
#define ERR_NOTONCHANNEL(source, channel)		"442 " + source + " " + channel + " :You're not on that channel"
#define ERR_NOTREGISTERED(source)			"451 " + source + " : You have not registered"
#define ERR_NEEDMOREPARAMS(source, command)		"461 " + source + " " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED(source)			"462 " + source + " :You may not reregister, you already have!"
#define ERR_PASSWDMISMATCH(source)			"464 " + source + " :Is NOT the correct password for the given name."
#define ERR_CHANNELISFULL(source, channel)		"471 " + source + " " + channel + " :Cannot join channel (+l)"
#define ERR_UNKNOWNMODE(source, modechar)       "472 " + source + " " + modechar + " :is unknown char to me"
#define ERR_INVITEONLYCHAN(source, channel)		"473 " + source + " " + channel + " :Cannot join channel (+i)"
#define ERR_BADCHANNELKEY(source, channel)		"475 " + source + " " + channel + " :Cannot join channel (+k)"
#define ERR_CHANOPRIVSNEEDED(source, channel)  		"482 " + source + " " + channel + " :You're not channel creator or operator"

#define RPL_WELCOME(source)		       		"001 " + source + " :Welcome to the ft_irc network " + source
#define RPL_ENDOFWHO(source, channel)			"315 " + source + " " + channel + " :End of WHO list"
#define RPL_CHANNELMODEIS(source, target, modes, params)		"324 " + source + " " + target + " " + modes + " " + params
#define RPL_NOTOPIC(source, channel)	        	"331 " + source + " " + channel + " :No topic is set"
#define RPL_TOPIC(source, channel, topic)	        "332 " + source + " " + channel + " " + topic
#define RPL_INVITING(source, nickname, channel)		"341 " + source + " " + nickname + " " + channel
#define RPL_NAMREPLY(source, channel, users)    	"353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel)			"366 " + source + " " + channel + " :End of /NAMES list."
#define RPL_JOIN(source, channel)	    		":" + source + " JOIN :" + channel	
#define RPL_PING(source, command)		    	":" + source + " PONG :" + command
#define RPL_PRIVMSG(source, target, message)    	":" + source + " PRIVMSG " + target + " :" + message
#define RPL_PART(source, channel, message)	    	":" + source + " PART " + channel + " " + message
#define RPL_NOTICE(source, target, message)	    	":" + source + " NOTICE " + target + " :" + message
#define RPL_NICK(source, newnick)			":" + source + " NICK :" + newnick
#define RPL_KICK(source, channel, target, message)	":" + source + " KICK " + channel + " " + target + " " + message
#define RPL_QUIT(source, message)			":" + source + " QUIT " + " :" + message

#define NICK_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-^_[]{}\\|"

#endif
