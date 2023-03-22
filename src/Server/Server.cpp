#include "Server.hpp"

Server::Server():_pollFds(NULL)	{}

Server::~Server()
{
	std::cout << "Server stop!" << std::endl;
		
	//Close all remaining fd (all server and not close client "on error")
	if (_pollFds != NULL)
	{
		for (int index = 0; index < pollFdSize; index++)
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

//TODO ADD IN -> In main, need to have loop while(1) that restart server on exception
//main server process
void	Server::routine(const ConfigServer& config)
{
	//SET CONFIGURATIONS VARIABLES
	configToServer(config);
	try
	{
		ServerBooting();
		std::cout << "Server on" << std::endl;
		ServerPollLoop();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}


//TODO DELETE -> After filled good config ref, delete temp define
//**************TEMP DEFINE****************
#define CONFIG_SERVER_NB 2
#define CONFIG_MAX_CLIENT 10
#define CONFIG_POLLTIMEOUT -1
int CONFIG_SERVER_NBPORT[2] = {1, 2};
int CONFIG_SERVER_PORTS_BACKLOG[3] = {5, 5, 5}; //changer en double dimension mais sera surement fix
uint16_t CONFIG_SERVER_PORTS[3] = {8080, 8081, 8082};
// ******************************************


//TODO CHANGE -> Replace all define for real config ref
//PONT ENTRE CONFIG ET SERVER
//INITIATION DE CERTAINES VARIABLE
void	Server::configToServer(const ConfigServer& config)
{
	nbServer = CONFIG_SERVER_NB;
	maxClient = CONFIG_MAX_CLIENT;
	_pollTimeOut = CONFIG_POLLTIMEOUT;
	_nbFdServer = 0;
	//for each server: New indexInfo + PORT COPY + isServer = true + serverNo
	for (int serverNo = 0; serverNo < nbServer; serverNo++)
	{
		for (int iPort = 0; iPort < CONFIG_SERVER_NBPORT[serverNo]; iPort++)
		{
			indexInfoInsert(_nbFdServer);
			indexInfoIt(_nbFdServer)->second->serverNo = serverNo;
			indexInfoIt(_nbFdServer)->second->isServer = true;
			indexInfoIt(_nbFdServer)->second->portBacklog = CONFIG_SERVER_PORTS_BACKLOG[iPort];
			indexInfoIt(_nbFdServer)->second->port = CONFIG_SERVER_PORTS[iPort];
			_nbFdServer++;
		}
	}
	pollFdSize = _nbFdServer + maxClient;
}

//***********************BOOTING SERVER******************************
//Creation des servers socket (listen socket)
void	Server::ServerBooting()
{
	//for all server socket
	for (int iSocket = 0; iSocket < _nbFdServer; iSocket++)
	{
		bootingSocket(iSocket);		//SET SERVER SOCKET (FD)
		bootingSetSockOpt(iSocket);	//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
		bootingBind(iSocket);		//BINDING PORT AND SOCKET
		bootingListen(iSocket);		//LISTENING
		pollFdSetFd(_pollFds, indexInfoIt(iSocket)->second->fd, iSocket); //ADDING SOCKET TO pollFds
	}
}

//SET SERVER SOCKET (FD)
void	Server::bootingSocket(const int& iSocket)
{
	if (indexInfoIt(iSocket)->second->fd = socket(AF_INET, SOCK_STREAM, 0) < 0)
		throw ServerException::FctSocketFail();
}

//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
void	Server::bootingSetSockOpt(const int& iSocket)
{
	int opt = 1;
	
	if (setsockopt(indexInfoIt(iSocket)->second->fd,
			SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
		throw ServerException::FctSetsockoptFail();
}

//BINDING PORT AND SOCKET
void	Server::bootingBind(const int& iSocket)
{
	sockaddr_in addr;

	setAddrServer(addr, indexInfoIt(iSocket)->second->port);
	if (bind(indexInfoIt(iSocket)->second->fd,
			reinterpret_cast<const sockaddr*>(&addr), sizeof(addr) < 0))
		throw ServerException::FctBindFail();
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
	if (listen(indexInfoIt(iSocket)->second->fd,
			indexInfoIt(iSocket)->second->portBacklog) < 0)
		throw ServerException::FctBindFail();
}
//**************************************************************************



// listen and redirection loop
void	Server::ServerPollLoop()
{
	int	activeClient = 0;
	int signalIndex;
	while (1)
	{
		//poll signal launcher
		if (poll(_pollFds, _nbFdServer, _pollTimeOut) < 0)
			throw ServerException::FctPollFail();
		
		//find the index of the signal find by poll in pollfds
		if (signalIndex = pollIndexSignal() >= 0)
		{
			if (indexInfoIt(signalIndex)->second->isServer == true)	//REQUEST TO SERVER
			{
				if (addNewClient(signalIndex) == 0)
					std::cout << "TEMP MESSAGE -> REQUESTING TO SERVER" << std::endl;
					//TODO ADD HERE -> REQUEST(serverNo, fd) & REMOVE -> TEMP MESSAGE
			}
			else 													//RESPONSE TO CLIENT
			{
				//TODO ADD HERE -> RESPONSE(fd) & REMOVE -> TEMP MESSAGE
				std::cout << "TEMP MESSAGE -> RESPONDING TO CLIENT" << std::endl;
				closeClient(signalIndex);
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
	for (int index = 0; index < pollFdSize; index++)
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
	
	int clientFd = accept(indexInfoServer->second->fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);
	if (clientFd < 0)
		throw ServerException::FctAcceptFail();
	
	if (int pollIndex = pollFdAdd(_pollFds, clientFd) < 0)
	{
		indexInfoInsert(pollIndex);
		return 0;
	}
	//TODO ADD HERE -> RESPONSE busy page or max client reached
	close(clientFd);
	return -1;
}

//Close client socket (fd) + delete infoIndex_t* + remove the map element for pollIndex
void	Server::closeClient(const int& signalIndex)
{
	indexInfo_it indexInfoClient = indexInfoIt(signalIndex);
	
	close(indexInfoClient->second->fd);
	delete indexInfoClient->second;
	_indexInfo.erase(indexInfoClient);
	pollFdResetFd(_pollFds, signalIndex);
}

//return the iterator on _indexinfo (map) from index of pollFds
indexInfo_it	Server::indexInfoIt(const int& pollIndex)
{
	if (_indexInfo.find(pollIndex) == _indexInfo.end())
		throw ServerException::FindFail();
	return _indexInfo.find(pollIndex);
}

void	Server::indexInfoInsert(const int& pollIndex)
{
	_indexInfo.insert(std::pair<unsigned int, indexInfo_t*>(pollIndex, new indexInfo_t));
	if (_indexInfo.find(pollIndex) == _indexInfo.end())
		throw ServerException::InsertFail();
}




