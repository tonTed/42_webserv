#include "Server2.hpp"

Server2::Server2()
{
	_reqs.reserve(POLLFD_LIMIT / 2);
}

Server2::~Server2(){closePollFds();}

//*****************************BOOTING SERVER***********************************

//Setup le server avant utilisation
void	Server2::booting()
{
	uint16_t	port[POLLFD_LIMIT];

	initIndexInfo();
	recordPort(port);
	setPortSocket(port);
}

//Initialisation à -1 des variables de l'array _indexInfo
void	Server2::initIndexInfo()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		resetIndexInfo(index);
}

//Initialisation à -1 des variables de _indexInfo[index]
void	Server2::resetIndexInfo(const int& index)
{
	_indexInfo[index].serverNum = -1;
	_indexInfo[index].CGIReadIndex = -1;
	_indexInfo[index].ClientIndex = -1;
}

//Initialise les valeurs de port des _indexInfo pour les fd de server
//Leurs assigne le numero de server
//Compte le nombre de fd de port
int	Server2::recordPort(uint16_t port[POLLFD_LIMIT])
{
	ConfigServer&	config = *ConfigServer::getInstance();
	std::vector<ServerData>	serversData = config.getServerData();

	_nbfdPort = 0;
	int nbServer = -1;

	for (std::vector<ServerData>::iterator itServer = serversData.begin();
			itServer != serversData.end(); itServer++)
	{
		nbServer++;
		for (std::vector<int>::iterator itPort = itServer->_serverPorts.begin();
			itPort != itServer->_serverPorts.end(); itPort++)
		{
			port[_nbfdPort] = static_cast<uint16_t>(*itPort);
			_indexInfo[_nbfdPort].serverNum = nbServer;
			_nbfdPort++;
		}
	}
}

//Pour chaque fd de port, socket/setsockopt/bind/listen
void	Server2::setPortSocket(const uint16_t port[POLLFD_LIMIT])
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
void	Server2::bootSocket(const int& iSocket)
{
	if ((_pollFds[iSocket].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerBootingException::FctSocketFail();
}

//SO_REUSEADDR permet la reutilisation du socket
// opt = temp de deconnexion du socket et fd
void	Server2::bootSetSockOpt(const int& iSocket)
{
	int opt = 1;
	
	if (setsockopt(_pollFds[iSocket].fd,
			SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
		throw ServerBootingException::FctSetsockoptFail();
}

//BINDING SOCKET WITH PORT
void	Server2::bootBind(const int& iSocket, const uint16_t port[POLLFD_LIMIT])
{
	sockaddr_in addr;

	setAddrServer(addr, port[iSocket]);
	if (bind(_pollFds[iSocket].fd,
			reinterpret_cast<const sockaddr*>(&addr), sizeof(addr) < 0))
		throw ServerBootingException::FctBindFail();
}

//set addr for server socket purposes (bind )
void	Server2::setAddrServer(sockaddr_in& addr, uint16_t port)
{
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
}

//Listen function protected by exception
void	Server2::bootListen(const int& iSocket)
{
	if (listen(_pollFds[iSocket].fd, LISTEN_BACKLOG) < 0)
		throw ServerBootingException::FctBindFail();
}

//******************************************************************************



//*****************************LOOPING SERVER***********************************

// main server loop (listen signal & redirection)
void	Server2::operating()
{
	int signalIndex;
	while (1)
	{
		//poll signal launcher
		if (poll(_pollFds, _nbfdPort, POLL_TIMEOUT) < 0)
			throw ServerLoopingException::FctPollFail();
		
		//find the index of the signal find by poll in pollfds
		if ((signalIndex = pollIndexSignal()) >= 0)
		{
			switch (indexOrigin(signalIndex))
			{
			case FROM_PORT:
				requestInfo_t reqInfo;
				if (setRequest(reqInfo, signalIndex) == 0)
					//teddyrequest(reqInfo.serverNum, reqInfo.clientFd, reqInfo.CGIfdWrite, reqInfo.CGIfdRead)
				//TODO dire a teddy qu'il doit fermer write dans parent et read dans fork
				//TODO Demander s'il n'aimerais pas mieux recevoir la struct reqInfo
				break;
			
			case FROM_CGI:
				/*TeddyCGIParsing(_indexInfo[signalIndex].serverNum,
							_pollFds[_indexInfo[signalIndex].ClientIndex].fd,
							_pollFds[_indexInfo[signalIndex].CGIReadIndex].fd)
				*/
				break;

			case FROM_CGI_PARSING:
				//Client been responded in CGIParsing
				//Here we only handelling closing socket
				closeConnection(signalIndex);
				break;
			}
		}
	}
}

/*Return the first index of pollFds that have revents = POLLIN
  Set server socket revents to 0
  Return -1 if no fd found
*/
int	Server2::pollIndexSignal()
{
	for (unsigned int index = 0; index < POLLFD_LIMIT; index++)
	{
		if (_pollFds[index].revents & POLLIN)
		{
			_pollFds[index].revents = 0;
			return index;
		}
	}
	std::cout << SERR_POLLINDEXSIGNAL << std::endl;
	return -1;
}

//With the index and _indexInfo, find the origin of signal
int	Server2::indexOrigin(const int& signalIndex)
{
	if (_indexInfo[signalIndex].CGIReadIndex == signalIndex)
		return FROM_CGI;
	else if (_indexInfo[signalIndex].ClientIndex == signalIndex)
		return FROM_CGI_PARSING;
	else
		return FROM_PORT;
}


//******************************************************************************


//*****************************REQUEST******************************************

/*Set reqInfo for request
	- accept client (protected by condition)
	- pipe for CGI (protected by condition)
	- set pollFds with clientFd & CGIReadFd
	- set indexInfo
*/
int	Server2::setRequest(requestInfo_t reqInfo, const int& signalIndex)
{
	reqInfo.clientFd = acceptClient(signalIndex);
	reqInfo.serverNum = _indexInfo[signalIndex].serverNum;

	if (reqInfo.clientFd >= 3) //client connected
	{
		if (pollFdsAvailable() == true) //Enough place in pollFds
		{
			int CGIPipe[2];
			if (pipe(CGIPipe) != -1) //pipe status ok
			{
				reqInfo.CGIfdRead = CGIPipe[PIPE_READ];
				reqInfo.CGIfdWrite = CGIPipe[PIPE_WRITE];

				int clientIndex = setPollFds(reqInfo.clientFd);
				int CGIReadIndex = setPollFds(reqInfo.CGIfdRead);

				setIndexInfo(clientIndex, CGIReadIndex, reqInfo.serverNum);

				return 0;
			}
		}
		//busy response to clientfd
		close(reqInfo.clientFd);
	}
	//Client not connected Impossible to answer
	return -1;
}


int	Server2::acceptClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	return (accept(_pollFds[signalIndex].fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen));
}


bool Server2::pollFdsAvailable() const
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


void	Server2::setIndexInfo(const int& clientIndex, const int& CGIReadIndex, const int& serverNum)
{
	_indexInfo[clientIndex].ClientIndex = clientIndex;
	_indexInfo[clientIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[clientIndex].serverNum = serverNum;
	_indexInfo[CGIReadIndex].ClientIndex = clientIndex;
	_indexInfo[CGIReadIndex].CGIReadIndex = CGIReadIndex;
	_indexInfo[CGIReadIndex].serverNum = serverNum;
}


int	Server2::setPollFds(const int& fd)
{
	//check for next fd disponible ou poll full throw excep[tion]
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
}


//****************************CLOSE CONNECTION**********************************

void	Server2::closeConnection(const int& signalIndex)
{
	int ClientIndex = _indexInfo[signalIndex].ClientIndex;
	int CGIReadIndex = _indexInfo[signalIndex].CGIReadIndex;
	
	safeClose(_pollFds[ClientIndex].fd);
	safeClose(_pollFds[CGIReadIndex].fd);
	resetIndexInfo(ClientIndex);
	resetIndexInfo(CGIReadIndex);
}

void	Server2::safeClose(int& fdSource)
{
	if (fdSource >= 3)
	{
		close(fdSource);
		fdSource = 0;
	}
}


void	Server2::closePollFds()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		safeClose(_pollFds[index].fd);
}