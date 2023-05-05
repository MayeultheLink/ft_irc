/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codesDef.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/05 17:43:45 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/05 17:44:30 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODESDEF_HPP
#define CODESDEF_HPP

#include <iostream>
#include <string>

//Messages d'erreur
#define ERR_NOSUCHNICK(source, target)					"401 " + source + " " + target + " :No such nickname / channel"
#define ERR_NOSUCHCHANNEL(source, target)				"403 " + source + " " + target + " :Channel name is invalid, or does not exist"
#define ERR_CANNOTSENDTOCHAN(source, target)			"404 " + source + " " + target + " :Cannot send a message to channel"
#define ERR_TOOMANYCHANNELS(source, channel)			"405 " + source + " " + channel + " :You have joined too many channels"
#define ERR_UNKNOWNCOMMAND(source, command)				"421 " + source + " " + command + " :Command unknown"
#define ERR_NONICKNAMEGIVEN(source, command)			"431 " + source + " " + command + " :No new nickname has been given"
#define ERR_ERRONEUSNICKNAME(source)					"432 " + source + " :This nickname is Invalid" 
#define ERR_NICKNAMEINUSE(source)						"433 " + source + " :This nickname is already in use"
#define ERR_USERNOTINCHANNEL(source, target, channel) 	"441 " + source + " " + target + " " + channel + " :the target user of the command is not on channel"
#define ERR_NOTONCHANNEL(source, channel)				"442 " + source + " " + channel + " :This user is not a member of this channel"
#define ERR_NEEDMOREPARAMS(source, command)				"461 " + source + " " + command + " :Not enough parameters"
#define ERR_ALREADYREGISTERED(source)					"462 " + source + " :You may not reregister, you already have!"
#define ERR_PASSWDMISMATCH(source)						"464 " + source + " :Is NOT the correct password for the given name."
#define ERR_CHANNELISFULL(source, channel)				"471 " + source + " " + channel + " :This channel can't accept more clients"
#define ERR_UNKNOWNMODE(source, unkmode)				"472 " + source + " " + unkmode + " :The mode is unknown"
#define ERR_BADCHANNELKEY(source, channel)				"475 " + source + " " + channel + " :Cannot join channel, please verify your password"
#define ERR_NOPRIVILEGES(source)						"481 " + source + " :This command requiring special privileges (operator)"
#define ERR_CHANOPRIVSNEEDED(source, channel)   		"482 " + source + " " + channel + " :You're not channel creator or operator"
#define ERR_UMODEUNKNOWNFLAG()							"501 : Mode flag was not recognised"
#define ERR_USERSDONTMATCH()							"502 : Cannot change mode for other users"

//Réponses du server
#define RPL_WELCOME(source)						"001 " + source + " :Welcome to the ft_irc network " + source
#define RPL_UMODEIS(target, modes)				"221 " + target + " " + modes
//faudra rajouter <mode_params> si on code des modes avec apram dans chan.
#define RPL_WHOISUSER(nick, user)				"311 " + nick + " is " + user
#define RPL_WHOISSERVER(nick, server)			"312 " + nick + " is using " + server
#define RPL_WHOISOPERATOR(nick)					"313 " + nick + " is an IRC Operator"
#define RPL_WHOISCHANNELS(nick, channel)		"319 " + nick + " is on " + channel
#define RPL_ENDOFWHO(source)					"315 " + source + " :End of WHO list"
#define RPL_ENDOFWHOIS(source)					"318 " + source + " :End of WHOIS list"
#define RPL_LIST(source, channel, count, topic)	"322 " + source + " " + channel + " " + count + " :" + topic
#define RPL_LISTEND(source)						"323 " + source + " :End of /LIST" 
#define RPL_CHANNELMODEIS(target, modes, param)	"324 MODE " + target + " " + modes + " " + param
#define RPL_NOTOPIC(source, channel)			"331 " + source + " " + channel + " :No topic is set"
#define RPL_TOPIC(source, channel, topic)		"332 " + source + " " + channel + " " + topic
#define RPL_WHOREPLY(mask, hopcount, realname)	"352 " + mask + " :" + hopcount + " " + realname
#define RPL_NAMREPLY(source, channel, users)    "353 " + source + " = " + channel + " :" + users
#define RPL_ENDOFNAMES(source, channel)			"366 " + source + " " + channel + " :End of /NAMES list."
#define RPL_YOUREOPER(source)					"381 " + source + " ::You are now an IRC operator"

//Réponses suite aux commandes (custom)
#define RPL_JOIN(source, channel)					":" + source + " JOIN :" + channel	
#define RPL_PING(source, command)					":" + source + " PONG :" + command
#define RPL_PRIVMSG(source, target, message)		":" + source + " PRIVMSG " + target + " :" + message
#define RPL_PART(source, channel, message)			":" + source + " PART " + channel + " " + message
#define RPL_NOTICE(source, target, message)			":" + source + " NOTICE " + target + " :" + message
#define RPL_NICK(source, newnick)					":" + source + " NICK :" + newnick
#define RPL_MODE_NORIGHT(source, target, message)	":" + source + " MODE " + target + " :" + message
#define RPL_MODE(source, target, modes, param)		":" + source + " MODE " + target + " " + modes + " " + param
#define RPL_TOPICALL(source, channel, topic)		":" + source + " TOPIC " + channel + " " + topic
#define RPL_KICK(source, channel, target, message)	":" + source + " KICK " + channel + " " + target + " " + message
#define RPL_KILL(source, target, message)			":" + source + " KILL " + target + " " + message
#define RPL_QUIT(source, message)					":" + source + " QUIT " + " :" + message
#define RPL_ERROR(source)							":" + source + " ERROR " + " :Server closing client connection->wrong password"

#define NICK_VALID_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-^_[]{}\\|"

#endif
