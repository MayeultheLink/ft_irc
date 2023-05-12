/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:33:01 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/12 15:41:57 by mde-la-s         ###   ########.fr       */
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

void Channel::addClient(ClientInfo *client)
{
	_clients.push_back(client);
	_nbClient++;
}
