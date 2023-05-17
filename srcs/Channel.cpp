/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:33:01 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/17 17:53:34 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, ClientInfo *client)
	: _name(name), _key(key), _clients(), _operators(), _nbClient(0)
{
	addClient(client);
	_operators.push_back(client);
}

Channel::~Channel() {}

std::vector<ClientInfo *>& Channel::getClients( void ) {return _clients;}

std::vector<ClientInfo *>& Channel::getOperators( void ) {return _operators;}

size_t Channel::getNbClient() const {return _nbClient;}

const std::string & Channel::getName() const {return _name;}

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
std::cout << "	SEND ALL" << std::endl;
	for (std::vector<ClientInfo *>::iterator it = _clients.begin(); it != _clients.end(); it++)
		(*it)->writetosend(message);
}

bool Channel::isOperator(const ClientInfo* client) const
{
	for (std::vector<ClientInfo*>::const_iterator it = _operators.begin(); it != _operators.end(); it++)
	{
		if (*it == client)
			return true;
	}
	return false;
}
