/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-la-s <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 15:16:58 by mde-la-s          #+#    #+#             */
/*   Updated: 2023/05/18 18:02:40 by mde-la-s         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

class Channel;

#include "Server.hpp"
#include "ClientInfo.hpp"

class Channel
{
	public:

		Channel(const std::string &name, const std::string &key, ClientInfo *admin);
		~Channel();

		// GETTERS
		const std::string&				getName() const;
		const std::string&				getKey() const;
//		Client	 					*getAdmin();
		size_t						getNbClient() const;
		const size_t &					getMaxClient() const;
//		size_t						getNboperators() const; //ok
//		std::vector<Client *>		getOperators() const;
//		std::string					getNicknamesList();
//		std::vector<std::string>	getNicknames();
//		std::vector<std::string>	getNicknamesOpe(); //en cours ??? pas certaines de ce que je fais
//		std::vector<std::string>	getNicknamesClients(); //en cours ??? pas certaines de ce que je fais
//		std::string					getModes() const;
//		std::string					getTopic() const;
//		std::vector<Client *>		getClients() const;
		bool				getIMode() const;
		bool				getKMode() const;
		bool				getLMode() const;
//
		std::vector<ClientInfo *>&	getClients();
		std::vector<ClientInfo *>&	getOperators();
		std::vector<ClientInfo *>&	getInvited();

		// SETTERS
		void			setKey(const std::string & key);
		void			setMaxClient(const size_t & maxClient);
//		void			setNbclients(const size_t nbclients);
//		void			setNboperators(const size_t nboperators); //ok
//		void			setModes(const std::string &modes);
//		void			setTopic(const std::string &topic);
		void			setIMode(bool mode);
		void			setKMode(bool mode);
		void			setLMode(bool mode);
	
		// FCT MEMBRES
		void			addClient(ClientInfo *client);
		void			removeClient(const ClientInfo *client);

		void			removeOperator(const ClientInfo* client);

//		void			removeOperator(Client *operators); //ok à tester +ajout quand -o à faire
//		bool			client_is_operator(Client *client); //à faire puis tester;
//		bool			client_is_inchannel(Client *client); // àtester
		
		void			setAllToInvited(void);
		void			sendAll(const std::string& message);
		void			sendAll(const std::string& message, ClientInfo *exclude);

		bool			isOperator(const ClientInfo* client) const;
		bool			isInvited(const ClientInfo* client) const;

//		void			consolDeBUGchannel();

	private: 

		std::string const			_name;
		std::string				_key;
		std::vector<ClientInfo *>		_clients;
		std::vector<ClientInfo *>		_operators;
		std::vector<ClientInfo *>		_invited;
		size_t					_nbClient;
		size_t					_maxClient;
//		size_t					_nboperators; //ok
		//pour la gestion des modes:
//		std::string				_modes;
//		std::string				_topic;
		
		bool					_iMode;
		bool					_kMode;
		bool					_lMode;
		
};

#endif
