#include "Server.hpp"

Server::Server():_reqs(NULL) {}

Server::~Server()
{
	closePollFds();
	if (_reqs != NULL)
		delete [] _reqs;
}

//*****************************BOOTING SERVER***********************************

//Setup le server avant utilisation
void	Server::booting()
{
	uint16_t	port[POLLFD_LIMIT];

	recordPort(port);
	setPortSocket(port);
}


//Initialise les valeurs de port
//Compte le nombre de fd de port
int	Server::recordPort(uint16_t port[POLLFD_LIMIT])
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
			reinterpret_cast<const sockaddr*>(&addr), sizeof(addr) < 0))
		throw ServerBootingException::FctBindFail();
}

//set addr for server socket purposes (bind )
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

// main server loop (listen signal & redirection)
void	Server::operating()
{
	_reqs = new Request[POLLFD_LIMIT - _nbfdPort];

	int signalIndex;
	while (1)
	{
		//TODO NEED TEST FOR ERRNO SIGNAL SIGINT
		//poll signal launcher
		if (poll(_pollFds, _nbfdPort, POLL_TIMEOUT) < 0 && errno != SIGINT)
		{
			if (errno != SIGINT)
				throw ServerLoopingException::FctPollFail();
			else
				throw ServerBootingException::FctSocketFail();
			//TODO replace above exception for closing program	
		}

		//find the index of the signal find by poll in pollfds
		if ((signalIndex = pollIndexSignal()) >= 0)
		{
			if (signalIndex < _nbfdPort)
			{
				if (setRequest(_reqs[reqIndex(signalIndex)], signalIndex) != -1)
					_reqs[reqIndex(signalIndex)].initRequest();
				break;
			}
			else
			{
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
	std::cout << SERR_POLLINDEXSIGNAL << std::endl;
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
	int clientFd = acceptClient(signalIndex);

	if (clientFd >= 3) //client connected
	{
			int clientIndex = setPollFds(clientFd);
			if (clientIndex != -1)
				return clientIndex;
			else
			{
				//busy response to clientfd
				close(clientFd);
			}
	}
	//Client not connected Impossible to answer
	return -1;
}


int	Server::acceptClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	return (accept(_pollFds[signalIndex].fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen));
}

int	Server::setPollFds(const int& fd)
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
	return -1;
}

int	Server::reqIndex(const int& signalIndex) const
{
	return (signalIndex - _nbfdPort);
}

//****************************CLOSE CONNECTION**********************************

void	Server::closeConnection(const int& signalIndex)
{
	safeClose(_pollFds[signalIndex].fd);
	_reqs[reqIndex(signalIndex)].resetRequest();
}

void	Server::safeClose(int& fdSource)
{
	if (fdSource >= 3)
	{
		close(fdSource);
		fdSource = 0;
	}
}

void	Server::closePollFds()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
		safeClose(_pollFds[index].fd);
}