#include "Server.hpp"

Server::Server():_reqs(NULL) {serverRoutine();}

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

	indexInfoInit();
	pollFdsInit();
	recordPort(port);
	setPortSocket(port);
}

void	Server::pollFdsInit()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		pollFdsReset(index);
}

void	Server::pollFdsReset(const int& index)
{
	if (_pollFds[index].fd >= 3)
		safeClose(_pollFds[index].fd);
	_pollFds[index].fd = UNSET;
	_pollFds[index].events = 0;
	_pollFds[index].revents = 0;
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
		_pollFds[iSocket].events = POLLIN;
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
	if (bind(_pollFds[iSocket].fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) < 0)
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
	try
	{
		booting();
		operating();
	}
	catch (std::exception& e){std::cout << e.what() << std::endl;}	
}


// server loop (listen signal & redirection)
void	Server::operating()
{
	_reqs = new Request[(POLLFD_LIMIT - _nbfdPort) / 2];
	
	std::signal(SIGINT, signal_handler);

	int signalIndex;
	while (1)
	{
		if (poll(_pollFds, POLLFD_LIMIT, POLL_TIMEOUT) < 0)
			std::cout << "poll return < 0: should never append" << std::endl;

		if ((signalIndex = pollIndexSignal()) == SIGNAL_NOT_IN_POLLFDS 
			|| _SIGINT == true)
			throw ServerOperatingException::EndServer();
		
		switch (indexOrigin(signalIndex))
		{
			case FROM_PORT:
			{
				//std::cout<< "signalIndex: " << signalIndex << std::endl;
				if (setRequest(signalIndex) == false)
					std::cout << "Request Fail" << std::endl;
				break;
			}
			case FROM_CLIENT:
			{
				if (_indexInfo[signalIndex].reqMade == false)
				{
					_reqs[_indexInfo[signalIndex].reqIndex]._initRequest();
					setReqMade(signalIndex);
				}
				else
				{
					std::cout << "ClosingConnection" << std::endl;
					closeConnection(signalIndex);
					std::cout << "connection closed" << std::endl;
				}
				break;
			}
			case FROM_CGI:
			{
				std::cout << "Parsing CGI AND RESPONDING" << std::endl;
				Response	respond(_reqs[_indexInfo[signalIndex].reqIndex]);
				break;
			}
			case SIGNAL_NOT_POLLIN:
			{
				std::cout << "Closed client request qui n'abouti pas" << std::endl;
			}
		}
	}
}

/*Return the first index of pollFds that have revents = POLLOUT
  Set server socket revents to 0 value
  Return -1 if no fd found
*/
int	Server::pollIndexSignal()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		if (_pollFds[index].revents == POLLIN)
		{
			_pollFds[index].revents = 0;
			return index;
		}
		else if (_pollFds[index].revents != 0 && index >= _nbfdPort)
			closePOLLHUPReq(index);
			return SIGNAL_NOT_POLLIN;
	}
	return SIGNAL_NOT_IN_POLLFDS;
}



//return the POLLOUT signal origin
int	Server::indexOrigin(const int& signalIndex) const
{

	if (signalIndex == SIGNAL_NOT_POLLIN)
		return signalIndex;
	if (signalIndex < _nbfdPort)
		return FROM_PORT;
	else if (_indexInfo[signalIndex].clientIndex == signalIndex)
		return FROM_CLIENT;
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
bool	Server::setRequest(const int& signalIndex)
{
	int clientFd = acceptClient(signalIndex);

	if (clientFd >= 3) //client connected
	{
		if (pollFdsAvailable() == true) //Enough place in pollFds
		{

			std::cout << "clientFd: " << clientFd << std::endl;

			int CGIPipe[2];
			if (pipe(CGIPipe) != -1) //pipe status ok
			{

				int	clientIndex = setPollFds(clientFd);
				int	CGIReadIndex = setPollFds(CGIPipe[PIPE_READ]);
				
				setIndexInfo(clientIndex, CGIReadIndex, signalIndex);
				
				_reqs[_indexInfo[signalIndex].reqIndex].setClient(clientFd);
				_reqs[_indexInfo[signalIndex].reqIndex].setServerId(signalIndex);
				_reqs[_indexInfo[signalIndex].reqIndex].setCGIFd(CGIPipe);

				std::cout << "clientIndex: " << clientIndex << std::endl;

				return true;
			}
		}
		std::string	HTMLBusy = DefaultHTML::getHtmlError(500);
		send(clientFd, &HTMLBusy, HTMLBusy.length(), 0);
		close(clientFd);
	}
	//Client not connected Impossible to answer
	return false;
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
		if (_pollFds[index].fd == UNSET)
		{
			nbFdRequire--;
			if (nbFdRequire == 0)
				return true;
		}
	}
	return false;
}

//set the next disponible fd of _pollFds (fd at UNSET) and return the index
int	Server::setPollFds(const int& fd)
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		if (_pollFds[index].fd == UNSET)
		{
			_pollFds[index].fd = fd;
			_pollFds[index].events = POLLIN;
			_pollFds[index].revents = 0;
			return index;
		}
	}
	return -1;
}

int	Server::reqAvailIndex() const
{
	for (int index = 0; index < (POLLFD_LIMIT - _nbfdPort) / 2; index++)
	{
		if (_reqs[index].getClient() == -1)
			return index;
	}
	return -1;
}



//******************************************************************************


//****************************INDEXINFO*****************************************

//Reset all indexInfo
void	Server::indexInfoInit()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		resetIndexInfo(index);
}

//reset _indexInfo[index]
void	Server::resetIndexInfo(const int& index)
{
	_indexInfo[index].serverNum = UNSET;
	_indexInfo[index].CGIReadIndex = UNSET;
	_indexInfo[index].clientIndex = UNSET;
	_indexInfo[index].reqIndex = UNSET;
	_indexInfo[index].reqMade = false;
}

//Set a new client cgi in _indexInfo
void	Server::setIndexInfo(const int& clientIndex, const int& CGIReadIndex, const int& serverNum)
{
	int	reqIndex = reqAvailIndex();
	
	_indexInfo[clientIndex].clientIndex = clientIndex;
	_indexInfo[clientIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[clientIndex].serverNum = serverNum;
	_indexInfo[clientIndex].reqIndex = reqIndex;
	_indexInfo[CGIReadIndex].clientIndex = clientIndex;
	_indexInfo[CGIReadIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[CGIReadIndex].serverNum = serverNum;
	_indexInfo[CGIReadIndex].reqIndex = reqIndex;
}

void	Server::setReqMade(const int& clientIndex)
{
	_indexInfo[clientIndex].reqMade = true;
	_indexInfo[_indexInfo[clientIndex].CGIReadIndex].reqMade = true;
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

	_pollFds[clientIndex].fd = UNSET;
	pollFdsReset(clientIndex);
	pollFdsReset(CGIReadIndex);

	_reqs[_indexInfo[signalIndex].reqIndex].resetRequest();

/* 	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		std::cout << std::dec << "index: " << index
				<< " fd:" << _pollFds[index].fd
				<< std::hex << " events:" << _pollFds[index].events << std::dec
				<< std::hex << " revents:" << _pollFds[index].revents << std::dec
				<< std::endl;
	} */
}

//Close _pollFds[index] if >= 3 and reset it to UNSET
void	Server::safeClose(int& fdSource)
{
	if (fdSource >= 3)
	{
		close(fdSource);
		fdSource = UNSET;
	}
}

//Close all _pollFds != UNSET
void	Server::closePollFds()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		safeClose(_pollFds[index].fd);
}

void	Server::closePOLLHUPReq(const int& clientIndex)
{
	safeClose(_pollFds[clientIndex].fd);
	closeConnection(clientIndex);
}

void	Server::signal_handler(int signal)
{
	(void)signal;
	_SIGINT = true;
}

bool Server::_SIGINT = false;