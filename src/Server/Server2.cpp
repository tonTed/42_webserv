#include "Server2.hpp"

Server2::Server2(){}

Server2::~Server2(){}

//*****************************BOOTING SERVER***********************************

void	Server2::booting()
{
	uint16_t	port[POLLFD_LIMIT];

	resetIndexInfo();
	recordPort(port);
	setPortSocket(port);
}

void	Server2::resetIndexInfo()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		_indexInfo[index].serverNo = -1;
		_indexInfo[index].CGIReadIndex = -1;
		_indexInfo[index].ClientIndex = -1;
	}
}

//PONT ENTRE CONFIG ET SERVER
//INITIATION DE CERTAINES VARIABLE
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
			_indexInfo[_nbfdPort].serverNo = nbServer;
			_nbfdPort++;
		}
	}
}

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

//SET SERVER SOCKET (FD)
void	Server2::bootSocket(const int& iSocket)
{
	if ((_pollFds[iSocket].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerBootingException::FctSocketFail();
}

//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
void	Server2::bootSetSockOpt(const int& iSocket)
{
	int opt = 1;
	
	if (setsockopt(_pollFds[iSocket].fd,
			SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		throw ServerBootingException::FctSetsockoptFail();
}

//BINDING PORT AND SOCKET
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

//LISTENING
void	Server2::bootListen(const int& iSocket)
{
	if (listen(_pollFds[iSocket].fd, LISTEN_BACKLOG) < 0)
		throw ServerBootingException::FctBindFail();
}

//******************************************************************************



//*****************************LOOPING SERVER***********************************

// listen and redirection loop
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
				if (addNewClient(signalIndex) == 0)
					std::cout << "TEMP MESSAGE -> REQUESTING TO SERVER" << std::endl;
					//TODO ADD HERE -> REQUEST(serverNo, fd) & REMOVE -> TEMP MESSAGE
				break;
			
			case FROM_CGI:
				/* code */
				break;

			case FROM_CGI_PARSING:
				//TODO ADD HERE -> RESPONSE(fd) & REMOVE -> TEMP MESSAGE
				std::cout << "TEMP MESSAGE -> RESPONDING TO CLIENT" << std::endl;
				closeClient(signalIndex);
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

int	Server2::indexOrigin(const int& signalIndex)
{
	if (_indexInfo[signalIndex].CGIReadIndex == signalIndex)
		return FROM_CGI;
	else if (_indexInfo[signalIndex].ClientIndex == signalIndex)
		return FROM_CGI_PARSING;
	else
		return FROM_PORT;
}

int	Server2::setClient(const int& signalIndex)
{
	int clientFd = acceptClient(signalIndex);

	//set next pollfdindex
	//CGI
}

int	Server2::acceptClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	int clientFd = accept(_pollFds[signalIndex].fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
		throw ServerLoopingException::FctAcceptFail();
	return clientFd;
}

int	Server2::setPollFds(const int& fd)
{
	//check for next fd disponible ou poll full throw excep[tion]
	while 
}


	//Add client
	if (int pollIndex = pollFdAdd(_pollFds, clientFd) < 0)
	{
		indexInfoInsert(pollIndex);
		//Add CGIfd
		return 0;
	}
	
	//TODO ADD HERE -> RESPONSE busy page or max client reached
	close(clientFd);
	return -1;
}