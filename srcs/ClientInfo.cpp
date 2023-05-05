#include "ClientInfo.hpp"

ClientInfo::ClientInfo(const std::string &hostname, int fd, int port)
	: _hostname(hostname), _fd(fd), _port(port)/*, _realname(""), _username(""), _nickname(""), _password(""), _modes(""), _isoperator(0), _isregistered(0), _Cchannels()*/  
{
	if (_hostname.size() > 63)
	{
		std::stringstream ss;
		ss << port;
		_hostname = ss.str();
	}

	std::cout << "ClientInfo created with hostname : " << _hostname << ", fd : " << _fd << ",port: " << _port << std::endl;

	return;
}


ClientInfo::~ClientInfo()
{
	return;
}

// GETTERS
std::string		ClientInfo::getHostname() const	{ return(_hostname); }
int				ClientInfo::getFd() const			{return(_fd);}
int				ClientInfo::getPort() const			{return(_port);}
std::string		ClientInfo::getRealname() const		{return(this->_realname);}
std::string		ClientInfo::getUsername() const		{return(this->_username);}
std::string		ClientInfo::getNickname() const		{return(this->_nickname);}
// std::string		ClientInfo::getPassword() const		{return(this->_password);}
// std::string		ClientInfo::getModes() const		{return(this->_modes);}
// bool			ClientInfo::getOperator() const		{return(this->_isoperator);}
bool			ClientInfo::getRegistered() const	{return(this->_isregistered);}
// std::string		ClientInfo::getListChannel()
// {
// 	std::string list_channel;
// 	for (std::vector<Channel *>::iterator it = _Cchannels.begin(); it != _Cchannels.end(); it++)
// 	{
// 		// std::cout << "valeur de it:" << (*it) << "name: " << std::endl;
// 		Channel *channel = it.operator*();
// 		std::string name = channel->getName(); 
// 		// std::cout << "name it: " << name << std::endl;
// 		list_channel.append(name);
// 	}
// 	return(list_channel);
// }

// std::vector<Channel *>	ClientInfo::getChannel() const
// {
// 	return(_Cchannels);
// }

// bool			ClientInfo::findChannel(std::string chan_name)
// {
// 	std::cout << RED << "ClientInfo : findChannel - start" << RESET << std::endl;
// 	for (std::vector<Channel *>::iterator it = _Cchannels.begin(); it != _Cchannels.end(); it++)
// 	{
// 		std::cout << "je suis dans le for et cchannel size" << _Cchannels.size() << std::endl;
// 		std::cout << "on entre dans le for ?" << chan_name << std::endl;
// 		// if (_Cchannels.size() == )
// 		if (it.operator*()->getName() == chan_name)
// 			return(true);
// 	}
// 	return (false);
// }

// SETTERS
void    ClientInfo::setRealname(const std::string &realname)	{_realname = realname;}
void    ClientInfo::setUsername(const std::string &username)	{_username = username;}
void    ClientInfo::setNickname(const std::string &nickname)	{_nickname = nickname;}
// void    ClientInfo::setPassword(const std::string &password)	{_password = password;}
// void    ClientInfo::setModes(const std::string &modes) 			{_modes = modes;}

// void    ClientInfo::setOperator(const bool &isoperator) 
// {
// 	_isoperator = isoperator;
// 	std::vector<Channel *>::iterator	it;
// 	std::string nickname = getNickname();
// 	for (it = _Cchannels.begin(); it != _Cchannels.end(); it++)
// 	{
// 		Channel *channel = it.operator*();
// 		std::vector<std::string> nickope = channel->getNicknamesOpe();
// 		if (isoperator == false)
// 		{
// 			if(std::find(nickope.begin(), nickope.end(), nickname) != nickope.end())
// 			{
// 					channel->removeOperator(this);
// 					continue;
// 					//si on change un droit, faut l'enlever dans les operateurs des chans
// 					//verifier si à la création d'un chan on check les droits du creatoeurs ou pendant join
// 			}
// 		}
// 		if (isoperator == true)
// 		{
// 			if(std::find(nickope.begin(), nickope.end(), nickname) != nickope.end())
// 			{
// 					channel->addOperator(this);
// 					continue;
// 			}
// 		}
// 	}
// }

void    ClientInfo::setRegistered(const bool &isregistered)	{_isregistered = isregistered;}

// // void	ClientInfo::setChannel(Channel *channel)
// // {
// // 	_channel = channel;
// // }

// void	ClientInfo::setChannel(Channel *channel)
// {
// 	std::cout << "je suis dans mon set channel de mon ClientInfo: " << _nickname << " et le chan ajouté: " << channel->getName() << std::endl; 
// 	_Cchannels.push_back(channel);
// }

//FCT MEMBRES
void	ClientInfo::writetosend(const std::string &message) const 
{
	std::cout << "---> " << message << std::endl;

	std::string buffer = message + "\r\n";
	if (send(_fd, buffer.c_str(), buffer.length(), 0) < 0)
		throw std::runtime_error("Error while sending message to ClientInfo.");
}

std::string ClientInfo::getPrefix() const 
{
	//return (_nickname + "@" + _hostname);
	//return (_username + "@" + _hostname);
	return (_nickname + "!" + _username + "@" + _hostname);
}

void	ClientInfo::reply(const std::string &reply)
{
	writetosend(":" + getPrefix() + " " + reply);
	//writetosend(":" + reply);
	//std::cout << ":" << getPrefix() << " " << reply << std::endl;
}

void	ClientInfo::reply_command(const std::string &reply)
{
	//writetosend(":" + getPrefix() + " " + reply);
	writetosend(reply);
	//std::cout << ":" << getPrefix() << " " << reply << std::endl;
}

// void	ClientInfo::join_channel(Channel *channel)
// {
// 	std::cout << GREEN << "\nClientInfo : join_channel - start " << channel->getName() << RESET << std::endl;
// 	if (channel->getNbClientInfos() != 0)
// 	{
// 		channel->addClientInfo(this);
// 		if (channel->ClientInfo_is_operator(this) == true)
// 		{
// 			channel->addOperator(this);
// 		}
// 		setChannel(channel);
// 	}
// 	else
// 	{
// 		channel->setNbClientInfos(1);
// 		// std::vector<ClientInfo *> _operatorschan = channel->getOperators();
// 		// 	size_t sizeop = _operatorschan.size();
// 		// 	channel->setNboperators(sizeop);
// 	}
	
// 	std::cout << "ClientInfo : " << getNickname() << " has been added to channel : " << channel->getName() << std::endl; 
// //	_channel = channel; // Store a reference to the channel the ClientInfo has joined
// 	//setChannel(channel);
// 	// Get a list of nicknames of ClientInfos in the channel
// 	const std::vector<std::string>& nicknames = channel->getNicknamesClientInfos();
// 	// Concatenate the nicknames into a single string, separated by spaces
// 	std::string users;
// 	for (std::vector<std::string>::const_iterator it = nicknames.begin(); it != nicknames.end(); ++it) 
// 		users += *it + " ";
// 	//Send a reply to the ClientInfo with the list of nicknames
// 	reply(RPL_NAMREPLY(_nickname, channel->getName(), users));
// 	//Send another reply indicating the end of the list of nicknames
// 	reply(RPL_ENDOFNAMES(_nickname, channel->getName()));
// 	//Send a message to all ClientInfos in the channel to notify them of the current ClientInfo joining
// 	channel->sendall(RPL_JOIN(getPrefix(), channel->getName()));
// 	if(channel->getTopic() == "")
// 	{
// 		reply(RPL_NOTOPIC(getNickname(), channel->getName()));
// 	}
// 	else
// 	{
// 		reply(RPL_TOPIC(getNickname(), channel->getName(), channel->getTopic()));
// 	}	
// 	std::cout << _nickname << " has joined channel " << channel->getName() << std::endl;
// 	std::cout << YELLOW << _nickname << "estdans les channel suivants: " << getListChannel() << RESET << std::endl;
// 	std::cout << GREEN << "ClientInfo : join_channel - end " << channel->getName() << RESET << std::endl;
// }

// void	ClientInfo::leave_channel(Channel *channel, std::string message, bool kill)
// {
// 	std::cout << ORANGE << "\nClientInfo : leave_channel - start" << RESET << std::endl;
// 	if (!channel)
// 		return;
// 	std::cout << ORANGE << "leave_channel du ClientInfo !" << RESET << std::endl;
// 	std::cout << PURPLE << "Send reply message is: " << message << std::endl;
// 	std::cout << GREEN << "Rpl sent to socket: " << _fd << std::endl;
// 	if (kill == 0)
// 		channel->sendall(RPL_PART(getPrefix(), channel->getName(), message));
// 	channel->removeClientInfo(this);
// 	// if (channel->getNbClientInfos() == 0)
// 	// {
// 	// 	_server->destroyChannel(channel);
// 	// }
// 	std::vector<Channel *>::iterator	it;
// 	for (it = _Cchannels.begin(); it != _Cchannels.end(); it++)
// 	{	
// 		//std::cout << ORANGE << "nom du channel: " << it.operator*()->getName() << RESET << std::endl;
// 		if (*it == channel)
// 		//if (it.operator*()->getName() == channel->getName())
// 		{
// 			std::cout << ROYALBLUE << "taille du vector de channels : " << _Cchannels.size() << RESET << std::endl;
		
// 			// it.operator*()->sendall(RPL_PART(getPrefix(), chan_name, message));
// 			// it.operator*()->removeClientInfo(this);
// 			_Cchannels.erase(it);
// 			std::cout << ROYALBLUE << "taille du vector de channels apres erase : " << _Cchannels.size() << RESET << std::endl;

// 			// if (_Cchannels.size() == 0);
// 			// 	_Cchannels.push_back("");
// 		break;
// 		}
// 	}
// 	std::cout << ORANGE << "ClientInfo : leave_channel - end" << RESET << std::endl;
// }
