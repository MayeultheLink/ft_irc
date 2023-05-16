/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:33:01 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/16 17:30:45 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, ClientInfo *admin)
	: _name(name), _key(key), _admin(admin), _clients(), _nbClient(0)
{
	addClient(admin);
	(void) _admin;
	return;
}

Channel::~Channel() {}

size_t Channel::getNbClient() const {return _nbClient;}

void Channel::addClient(ClientInfo *client)
{
	_clients.push_back(client);
	_nbClient++;
}

void Channel::removeClient(ClientInfo *client)
{
	for (std::vector<ClientInfo*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		if (*it == client)
		{
			_clients.erase(it);
			break;
		}
	}
	_nbClient--;
}

void Channel::sendAll(const std::string & message, ClientInfo * client)
{
	for (std::vector<ClientInfo *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		if (*it != client)
			(*it)->writetosend(message);
}

void Channel::sendAll(const std::string & message)
{
	for (std::vector<ClientInfo *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		(*it)->writetosend(message);
}
