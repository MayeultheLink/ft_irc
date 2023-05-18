/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:33:01 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/18 18:21:39 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(const std::string &name, const std::string &key, ClientInfo *client)
	: _name(name), _key(key), _clients(), _operators(), _invited(), _nbClient(0), _maxClient(0), _iMode(false), _kMode(false), _lMode(false)
{
	addClient(client);
	_operators.push_back(client);
}

Channel::~Channel() {}

const std::string& Channel::getKey( void ) const {return _key;}
void Channel::setKey( const std::string & key ) {_key = key;}

bool Channel::getIMode( void ) const {return _iMode;}
void Channel::setIMode( bool set ) {_iMode = set;}

bool Channel::getKMode( void ) const {return _kMode;}
void Channel::setKMode( bool set ) {_kMode = set;}

bool Channel::getLMode( void ) const {return _lMode;}
void Channel::setLMode( bool set ) {_lMode = set;}

std::vector<ClientInfo *>& Channel::getClients( void ) {return _clients;}
std::vector<ClientInfo *>& Channel::getOperators( void ) {return _operators;}
std::vector<ClientInfo *>& Channel::getInvited( void ) {return _invited;}

const std::string & Channel::getName() const {return _name;}
size_t Channel::getNbClient() const {return _nbClient;}
const size_t& Channel::getMaxClient() const {return _maxClient;}

void Channel::setMaxClient(const size_t& maxClient) {_maxClient = maxClient;}

void Channel::addClient(ClientInfo *client)
{
	_clients.push_back(client);
	_nbClient++;
}

void Channel::removeClient(const ClientInfo *client)
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

void Channel::removeOperator(const ClientInfo *client)
{
	for (std::vector<ClientInfo*>::iterator it = _operators.begin(); it != _operators.end(); ++it)
	{
		if (*it == client)
		{
			_operators.erase(it);
			break;
		}
	}
}

void Channel::setAllToInvited( void )
{
	for (std::vector<ClientInfo*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
		_invited.push_back(*it);
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

bool Channel::isInvited(const ClientInfo* client) const
{
	for (std::vector<ClientInfo*>::const_iterator it = _invited.begin(); it != _invited.end(); it++)
	{
		if (*it == client)
			return true;
	}
	return false;
}
