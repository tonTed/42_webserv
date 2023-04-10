#include "Server.hpp"

Server::Server():_reqs(NULL) {}

Server::~Server()
{
	closePollFds();
	if (_reqs != NULL)
		delete [] _reqs;
}

//*****************************SET/GET/TERS*************************************
void	Server::setNbFdPort(const int& nbPort)	{_nbfdPort = nbPort;}
int		Server::getNbFdPort() const				{return _nbfdPort;}
//II -> IndexInfo
void	Server::setIIServerNum(const int& index, const int& serverNum)		{_indexInfo[index].serverNum = serverNum;}
int		Server::getIIServerNum(const int& index) const						{return _indexInfo[index].serverNum;}
void	Server::setIICGIReadIndex(const int& index, const int& CGIReadIndex){_indexInfo[index].CGIReadIndex = CGIReadIndex;}
int		Server::getIICGIReadIndex(const int& index) const					{return _indexInfo[index].CGIReadIndex;}
void	Server::setIIClientIndex(const int& index, const int& ClientIndex)	{_indexInfo[index].clientIndex = ClientIndex;}
int		Server::getIIClientIndex(const int& index) const					{return _indexInfo[index].clientIndex;}
//PF -> pollFds
void	Server::setPFFd(const int& index, const int& fd)				{_pollFds[index].fd = fd;}
int		Server::getPFFd(const int& index) const							{return _pollFds[index].fd;}
void	Server::setPFEvents(const int& index, const int& events)		{_pollFds[index].events = events;}
int		Server::getPFEvents(const int& index) const						{return _pollFds[index].events;}
void	Server::setPFRevents(const int& index, const int& revents)		{_pollFds[index].revents = revents;}
int		Server::getPFRevents(const int& index) const					{return _pollFds[index].revents;}

//*****************************BOOTING SERVER***********************************

//Setup le server avant utilisation
void	Server::booting()
{
	uint16_t	port[POLLFD_LIMIT];

	initIndexInfo();
	recordPort(port);
	setPortSocket(port);
}


//Initialise les valeurs de port
//Compte le nombre de fd de port
void	Server::recordPort(uint16_t port[POLLFD_LIMIT])
{
	ConfigServer&	config = *ConfigServer::getInstance();
	std::vector<ServerData>	serversData = config.getServerData();

	_nbfdPort = 0;

	for (std::vector<ServerData>::iterator itServer = serversData.begin();
			itServer != serversData.end(); itServer++)
	{
		for (std::vector<int>::iterator itPort = itServer->_serverPorts.begin();
			itPort != itServer->_serverPorts.end(); itPort++)
		{
			port[_nbfdPort] = static_cast<uint16_t>(*itPort);
			_nbfdPort++;
		}
	}
}

//Pour chaque fd de port, socket/setsockopt/bind/listen
void	Server::setPortSocket(const uint16_t port[POLLFD_LIMIT])
{
	for (int iSocket = 0; iSocket < _nbfdPort; iSocket++)
	{
		bootSocket(iSocket);		//SET SERVER SOCKET (FD)
		bootSetSockOpt(iSocket);	//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
		bootBind(iSocket, port);	//BINDING PORT AND SOCKET
		bootListen(iSocket);		//LISTENING
	}
}

//Fonction socket protected by exception
void	Server::bootSocket(const int& iSocket)
{
	if ((_pollFds[iSocket].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerBootingException::FctSocketFail();
}

//SO_REUSEADDR permet la reutilisation du socket
// opt = temp de deconnexion du socket et fd
void	Server::bootSetSockOpt(const int& iSocket)
{
	int opt = 1;
	
	if (setsockopt(_pollFds[iSocket].fd,
			SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		throw ServerBootingException::FctSetsockoptFail();
}

//BINDING SOCKET WITH PORT
void	Server::bootBind(const int& iSocket, const uint16_t port[POLLFD_LIMIT])
{
	sockaddr_in addr;

	setAddrServer(addr, port[iSocket]);
	if (bind(_pollFds[iSocket].fd,
			reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) < 0)

		throw ServerBootingException::FctBindFail();
}

//set addr for server socket purposes (bind)
void	Server::setAddrServer(sockaddr_in& addr, uint16_t port)
{
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
}

//Listen function protected by exception
void	Server::bootListen(const int& iSocket)
{
	if (listen(_pollFds[iSocket].fd, LISTEN_BACKLOG) < 0)
		throw ServerBootingException::FctBindFail();
}

//******************************************************************************


//*****************************LOOPING SERVER***********************************

//main function of server
void	Server::serverRoutine()
{
	booting();
	operating();
}


// server loop (listen signal & redirection)
void	Server::operating()
{
	//TODO UNMUTED UNDER
	//_reqs = new Request[(POLLFD_LIMIT - _nbfdPort) / 2];
	

	int signalIndex;
	while (1)
	{
		if (poll(_pollFds, POLLFD_LIMIT, POLL_TIMEOUT) < 0)
				std::cout << "poll return < 0: should never append" << std::endl;

		//find the index of the signal find by poll in pollfds
		if ((signalIndex = pollIndexSignal()) >= 0)
		{
			switch (indexOrigin(signalIndex))
			{
				case FROM_PORT:
					if (setRequest(_reqs[reqIndex(signalIndex)], signalIndex) != -1)
						std::cout << "Request made" << std::endl;
						//TODO UNMUTED UNDER, REMOVE OVER
						//_reqs[reqIndex(signalIndex)].initRequest();
					break;
				
				case FROM_CGI:
					//PARSE CGI && RESPOND
					//TODO MAKE A RESPOND CALL AND REMOVE UNDER COUT
					std::cout << "Parsing CGI AND RESPONDING" << std::endl;
					break;
				
				case FROM_CGI_PARSING:
					//Here we only handelling closing socket
					closeConnection(signalIndex);
					break;

				default:
					//Dev & debug purpose
					std::cout << "indexInfo data corrupt" << std::endl;
			}
		}
		else
			throw ServerOperatingException::EndServer();
	}
}

/*Return the first index of pollFds that have revents = POLLIN
  Set server socket revents to 0
  Return -1 if no fd found
*/
int	Server::pollIndexSignal()
{
	for (unsigned int index = 0; index < POLLFD_LIMIT; index++)
	{
		if (_pollFds[index].revents & POLLIN)
		{
			_pollFds[index].revents = 0;
			return index;
		}
	}
	return -1;
}

//return the POLLIN signal origin
int	Server::indexOrigin(const int& signalIndex) const
{
	if (signalIndex < _nbfdPort)
		return FROM_PORT;
	else if (_indexInfo[signalIndex].clientIndex == signalIndex)
		return FROM_CGI_PARSING;
	else if (_indexInfo[signalIndex].CGIReadIndex == signalIndex)
		return FROM_CGI;
	else
		return -1;
}

//******************************************************************************


//*****************************REQUEST******************************************

/*Set reqInfo for request
	- accept client (protected by condition)
	- pipe for CGI (protected by condition)
	- set pollFds with clientFd & CGIReadFd
	- set indexInfo
*/
int	Server::setRequest(Request& req, const int& signalIndex)
{
	//TODO REMOVE VOID UNDER
	(void)req;
	int clientFd = acceptClient(signalIndex);

	if (clientFd >= 3) //client connected
	{
		if (pollFdsAvailable() == false) //Enough place in pollFds
		{
			int CGIPipe[2];
			if (pipe(CGIPipe) != -1) //pipe status ok
			{
				int	clientIndex = setPollFds(clientFd);
				int	CGIReadIndex = setPollFds(CGIPipe[PIPE_READ]);
				
				setIndexInfo(clientIndex, CGIReadIndex, signalIndex);
				
				//TODO UNMUTED UNDER AFTER MERGE
				/* _reqs[reqIndex(clientIndex)].setClientFd(clientFd);
				_reqs[reqIndex(clientIndex)].setServerId(signalIndex);
				_reqs[reqIndex(clientIndex)].setCGIFd(CGIPipe);
				 */

				return clientIndex;
			}
			//busy response to clientfd
			close(clientFd);
		}
	}
	//Client not connected Impossible to answer
	return -1;
}

//accept function with stuct sockaddr_in
int	Server::acceptClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	return (accept(_pollFds[signalIndex].fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen));
}

//Return true if pollFds have at least 2 disponibles fd
bool Server::pollFdsAvailable() const
{
	int nbFdRequire = 2;

	for (int index = POLLFD_LIMIT - 1; index > 0; index--)
	{
		if (_pollFds[index].fd == 0)
		{
			nbFdRequire--;
			if (nbFdRequire == 0)
				return true;
		}
	}
	return false;
}

//set the next disponible fd of _pollFds (fd at 0) and return the index
int	Server::setPollFds(const int& fd)
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		if (_pollFds[index].fd == 0)
		{
			_pollFds[index].fd = fd;
			_pollFds[index].events = POLLIN;
			_pollFds[index].revents = 0;
			return index;
		}
	}
	return -1;
}

//bridge between signalIndex and _reqs index
int	Server::reqIndex(const int& signalIndex) const
{
	return ((signalIndex - _nbfdPort) / 2);
}

//******************************************************************************


//****************************INDEXINFO*****************************************

//Reset all indexInfo
void	Server::initIndexInfo()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		resetIndexInfo(index);
}

//reset _indexInfo[index]
void	Server::resetIndexInfo(const int& index)
{
	_indexInfo[index].serverNum = -1;
	_indexInfo[index].CGIReadIndex = -1;
	_indexInfo[index].clientIndex = -1;
}

//Set a new client cgi in _indexInfo
void	Server::setIndexInfo(const int& clientIndex, const int& CGIReadIndex, const int& serverNum)
{
	_indexInfo[clientIndex].clientIndex = clientIndex;
	_indexInfo[clientIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[clientIndex].serverNum = serverNum;
	_indexInfo[CGIReadIndex].clientIndex = clientIndex;
	_indexInfo[CGIReadIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[CGIReadIndex].serverNum = serverNum;
}

//******************************************************************************


//****************************CLOSE CONNECTION**********************************

//- Reset _indexInfo for the current request
//- Close clienFd and CGIReadFd in _pollFds
//- Reset _reqs[reqIndex(signalIndex)]
void	Server::closeConnection(const int& signalIndex)
{
	int	clientIndex = _indexInfo[signalIndex].clientIndex;
	int	CGIReadIndex = _indexInfo[signalIndex].CGIReadIndex;

	resetIndexInfo(clientIndex);
	resetIndexInfo(CGIReadIndex);

	safeClose(_pollFds[clientIndex].fd);
	safeClose(_pollFds[CGIReadIndex].fd);

	//TODO UNMUTED AFTER MERGE
	//_reqs[reqIndex(signalIndex)].resetRequest();
}

//Close _pollFds[index] if >= 3 and reset it to 0
void	Server::safeClose(int& fdSource)
{
	if (fdSource >= 3)
	{
		close(fdSource);
		fdSource = 0;
	}
}

//Close all _pollFds != 0
void	Server::closePollFds()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		safeClose(_pollFds[index].fd);
}