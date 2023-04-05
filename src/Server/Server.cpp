#include "Server.hpp"

Server::Server()
{
	std::cout << "Server booting" << std::endl;
	_pollFds = NULL;
	configToServer(); //SET CONFIGURATIONS VARIABLES
}

Server::~Server()
{
	std::cout << "Server stop!" << std::endl;
		
	//Close all remaining fd (all server and not close client "on error")
	if (_pollFds != NULL)
	{
		for (uint index = 0; index < pollFdSize; index++)
		{
			if (_pollFds[index].fd >= 3)
				close(_pollFds[index].fd);
		}
		delete _pollFds;
	}
	// delete all indexInfo_t* dans _indexInfo
	for (indexInfo_it it = _indexInfo.begin(); it != _indexInfo.end(); it++)
		delete it->second;
}

//PONT ENTRE CONFIG ET SERVER
//INITIATION DE CERTAINES VARIABLE
void	Server::configToServer()
{
	ConfigServer&	config = *ConfigServer::getInstance();
	std::vector<ServerData>	serversData = config.getServerData();

	maxClient = CONFIG_MAX_CLIENT;
	_pollTimeOut = CONFIG_POLLTIMEOUT;
	_nbfdPort = 0;
	nbServer = -1;

	for (std::vector<ServerData>::iterator itServer = serversData.begin();
			itServer != serversData.end(); itServer++)
	{
		nbServer++;
		for (std::vector<int>::iterator itPort = itServer->_serverPorts.begin();
			itPort != itServer->_serverPorts.end(); itPort++)
		{
			indexInfoInsert(_nbfdPort);
			indexInfoIt(_nbfdPort)->second->serverNo = nbServer;
			indexInfoIt(_nbfdPort)->second->origin = FROM_PORT;
			indexInfoIt(_nbfdPort)->second->portBacklog = LISTEN_BACKLOG;
			indexInfoIt(_nbfdPort)->second->port =  static_cast<uint16_t>(*itPort);
			indexInfoIt(_nbfdPort)->second->indexCGI =  -1;
			indexInfoIt(_nbfdPort)->second->indexClient = -1;
			
			_nbfdPort++;
		}
	}
	pollFdSize = _nbfdPort + (maxClient * 2);
}

//main server process
void	Server::routine()
{
	while (1)
	{
		try
		{
			ServerBooting();
			std::cout << "Server on" << std::endl;
			ServerPollLoop();
		}
		catch (const ServerLoopingException::FctAcceptFail& e) {std::cout << e.what() << std::endl;}
		catch (const ServerLoopingException::FctPollFail& e) {std::cout << e.what() << std::endl;}
		catch (const ServerLoopingException::FindFail& e) {std::cout << e.what() << std::endl;}
		catch (const ServerLoopingException::InsertFail& e) {std::cout << e.what() << std::endl;}
		catch (const std::exception& e) {std::cout << e.what() << std::endl << "Server stop" << std::endl; break;}
	}
}



//***********************BOOTING SERVER******************************
//Creation des servers socket (listen socket)
void	Server::ServerBooting()
{
	//for all server socket
	for (int iSocket = 0; iSocket < _nbfdPort; iSocket++)
	{
		indexInfo_t* port_ii = new indexInfo_t;
		bootingSocket(iSocket);		//SET SERVER SOCKET (FD)
		indexInfoIt(iSocket)->second->fdPort = _pollFds[iSocket].fd; //pass ref to indexInfo
		bootingSetSockOpt(iSocket);	//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
		bootingBind(iSocket);		//BINDING PORT AND SOCKET
		bootingListen(iSocket);		//LISTENING
		pollFdSetFd(_pollFds, _pollFds[iSocket].fd, iSocket); //ADDING SOCKET TO pollFds
	}
}

//SET SERVER SOCKET (FD)
void	Server::bootingSocket(const int& iSocket)
{
	if ((_pollFds[iSocket].fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		throw ServerBootingException::FctSocketFail();
}

//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
void	Server::bootingSetSockOpt(const int& iSocket)
{
	int opt = 1;
	
	if (setsockopt(_pollFds[iSocket].fd,
			SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		throw ServerBootingException::FctSetsockoptFail();
}

//BINDING PORT AND SOCKET
void	Server::bootingBind(const int& iSocket)
{
	sockaddr_in addr;

	setAddrServer(addr, indexInfoIt(iSocket)->second->port);
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

//LISTENING
void	Server::bootingListen(const int& iSocket)
{
	if (listen(_pollFds[iSocket].fd,
			indexInfoIt(iSocket)->second->portBacklog) < 0)
		throw ServerBootingException::FctBindFail();
}
//**************************************************************************



// listen and redirection loop
void	Server::ServerPollLoop()
{
	int signalIndex;
	while (1)
	{
		//poll signal launcher
		if (poll(_pollFds, _nbfdPort, _pollTimeOut) < 0)
			throw ServerLoopingException::FctPollFail();
		
		//find the index of the signal find by poll in pollfds
		if ((signalIndex = pollIndexSignal()) >= 0)
		{
			switch (indexInfoIt(signalIndex)->second->origin)
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
int	Server::pollIndexSignal()
{
	for (unsigned int index = 0; index < pollFdSize; index++)
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


//set pollfd et add new indexInfo
int	Server::addNewClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	indexInfo_it indexInfoServer = indexInfoIt(signalIndex);
	
	int clientFd = accept(indexInfoServer->second->fdPort,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
		throw ServerLoopingException::FctAcceptFail();
	
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

int	Server::setClientCGI(const int& pollIndex)
{
	indexInfoInsert(pollIndex);
	indexInfo_it indexInfoClient = indexInfoIt(pollIndex);


	//pipe
	int CGIPipe[2];
	//if (pipe(CGIPipe) != 0)
		//protection
	
	int pollIndexCGI = pollFdAdd(_pollFds, CGIPipe[PIPE_READ]);

	

	indexInfoClient->second->indexClient = pollIndex;

}int	Server2::acceptClient(const int& signalIndex)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	int clientFd = accept(_pollFds[signalIndex].fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
		throw ServerLoopingException::FctAcceptFail();
	return clientFd;
}

/* //Close client socket (fd) + delete infoIndex_t* + remove the map element for pollIndex
void	Server::closeClient(const int& signalIndex)
{
	indexInfo_it indexInfoClient = indexInfoIt(signalIndex);
	indexInfo_it indexInfoCGI = indexInfoIt(indexInfoClient->second->indexCGI);

	//Close fd client & CGI
	_safePollFdsClose(signalIndex);
	_safePollFdsClose(indexInfoClient->second->indexCGI);

	//Check for CGI WRITE is not close -> closing it
	if (indexInfoCGI->second->fdCGI[PIPE_WRITE] >= 3)
		close(indexInfoCGI->second->fdCGI[PIPE_WRITE]);

	//Reset pollfd index client
	pollFdResetFd(_pollFds, signalIndex);
	pollFdResetFd(_pollFds, indexInfoClient->second->indexCGI);

	//Remove _indexInfo index	
	delete indexInfoClient->second;
	_indexInfo.erase(indexInfoClient);

	delete indexInfoCGI->second;
	_indexInfo.erase(indexInfoCGI);
	

} */

//return the iterator on _indexinfo (map) from index of pollFds
indexInfo_it	Server::indexInfoIt(const int& pollIndex)
{
	if (_indexInfo.find(pollIndex) == _indexInfo.end())
		throw ServerLoopingException::FindFail();
	return _indexInfo.find(pollIndex);
}

void	Server::indexInfoInsert(const int& pollIndex)
{
	_indexInfo.insert(std::pair<unsigned int, indexInfo_t*>(pollIndex, new indexInfo_t));
	if (_indexInfo.find(pollIndex) == _indexInfo.end())
		throw ServerLoopingException::InsertFail();
}


void	Server::_safePollFdsClose(const int& pollIndex)
{
	if (_pollFds[pollIndex].fd >= 3)
	{
		close(_pollFds[pollIndex].fd);
		_pollFds[pollIndex].fd = 0;
	}
}

