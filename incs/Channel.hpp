/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:16:58 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/19 15:11:11 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Channel;

#include "Server.hpp"
#include "ClientInfo.hpp"

class Channel
{

	private: 

		const std::string		_name;
		std::string			_key;
		std::string			_topic;
		std::vector<ClientInfo *>	_clients;
		std::vector<ClientInfo *>	_operators;
		std::vector<ClientInfo *>	_invited;
		size_t				_nbClient;
		size_t				_maxClient;
		
		bool				_iMode;
		bool				_kMode;
		bool				_lMode;
		bool				_tMode;
		
	public:

		Channel(const std::string &name, const std::string &key, ClientInfo *admin);
		~Channel();

		const std::string&		getName() const;
		const std::string&		getKey() const;
		void				setKey(const std::string & key);
		size_t				getNbClient() const;
		const size_t&			getMaxClient() const;
		void				setMaxClient(const size_t & maxClient);
		const std::string&		getTopic() const;
		void				setTopic(const std::string& topic);
		bool				getIMode() const;
		void				setIMode(bool mode);
		bool				getKMode() const;
		void				setKMode(bool mode);
		bool				getLMode() const;
		void				setLMode(bool mode);
		bool				getTMode() const;
		void				setTMode(bool mode);

		std::vector<ClientInfo *>&	getClients();
		std::vector<ClientInfo *>&	getOperators();
		std::vector<ClientInfo *>&	getInvited();

		void				addClient(ClientInfo *client);
		void				removeClient(const ClientInfo *client);

		void				removeOperator(const ClientInfo* client);

		void				setAllToInvited(void);

		void				sendAll(const std::string& message);
		void				sendAll(const std::string& message, ClientInfo *exclude);

		bool				isOperator(const ClientInfo* client) const;
		bool				isInvited(const ClientInfo* client) const;

};

#endif
