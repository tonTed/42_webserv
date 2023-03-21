#include "Server.hpp"

Server::Server(){}
Server::~Server()
{
	delete this->_pollFds;
	//TODO delete all indexInfo_t* dans _indexInfo

}

//TEMP DEFINE
#define CONFIG_SERVER_NB 2
#define CONFIG_MAX_CLIENT 10
#define CONFIG_POLLTIMEOUT -1
int CONFIG_SERVER_NBPORT[2] = {1, 2};
#define CONFIG_SERVER_MAXHEADERSIZE 300
int CONFIG_SERVER_PORTS_BACKLOG[3] = {5, 5, 5}; //changer en double dimension mais sera surement fix
uint16_t CONFIG_SERVER_PORTS[3] = {8080, 8081, 8082};

void	Server::configToServer(const ConfigServer& config)
{
	this->nbServer = CONFIG_SERVER_NB;
	this->maxClient = CONFIG_MAX_CLIENT;
	this->_pollTimeOut = CONFIG_POLLTIMEOUT;
	this->_nbFdServer = 0;
	//PORT COPY
	for (int serverNo = 0; serverNo < nbServer; serverNo++)
	{
		for (int iPort = 0; iPort < CONFIG_SERVER_NBPORT[serverNo]; iPort++)
		{
			this->_indexInfo.insert(std::pair<int, indexInfo_t*>(_nbFdServer, new indexInfo_t));
			indexInfoIt(_nbFdServer)->second->serverNo = serverNo;
			indexInfoIt(_nbFdServer)->second->isServer = true;
			indexInfoIt(_nbFdServer)->second->maxHeaderSize = CONFIG_SERVER_MAXHEADERSIZE;
			indexInfoIt(_nbFdServer)->second->portBacklog = CONFIG_SERVER_PORTS_BACKLOG[iPort];
			indexInfoIt(_nbFdServer)->second->port = CONFIG_SERVER_PORTS[iPort];
			this->_nbFdServer++;
		}
	}

	this->pollFdSize = this->_nbFdServer + this->maxClient;
}

void	Server::ServerBooting()
{
	int opt = 1;
	sockaddr_in addr;

	for (int iSocket = 0; iSocket < this->_nbFdServer; iSocket++)
	{
		//SET SERVER SOCKET (FD)
		if (this->_indexInfo.find(iSocket)->second->fd == socket(AF_INET, SOCK_STREAM, 0) < 0)
			throw ServerException::FctSocketFail();

		//OPTION ON SERVER SOCKET (NEED MORE TEST / REMOVE IF PROBLEM)
		if (setsockopt(indexInfoIt(_nbFdServer)->second->fd,
				SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			throw ServerException::FctSetsockoptFail();

		//BINDING PORT AND SOCKET
		setAddrServer(addr, indexInfoIt(_nbFdServer)->second->port);
		if (bind(indexInfoIt(_nbFdServer)->second->fd,
				reinterpret_cast<const sockaddr*>(&addr), sizeof(addr) < 0))
			throw ServerException::FctBindFail();

		//LISTENING
		if (listen(indexInfoIt(_nbFdServer)->second->fd,
				indexInfoIt(_nbFdServer)->second->portBacklog) < 0)
			throw ServerException::FctBindFail();

		//ADDING SOCKET TO pollFds
		this->pollFdSetFd(this->_pollFds, this->_indexInfo.find(iSocket)->second->fd, iSocket);
	}
}
	
void	Server::routine()
{
	try
	{
		ServerBooting();
		ServerPollLoop();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl
					<< "Server stop!" << std::endl;
	}
}

void	Server::ServerPollLoop()
{
	int	activeClient = 0;
	int signalIndex;
	//TODO set a signal to change value that will stop the server properly
	while (1)
	{
		if (poll(_pollFds, _nbFdServer, _pollTimeOut) < 0)
			std::cout << "POLL ERROR HANDLER" << std::endl;
		if (signalIndex = pollIndexSignal() >= 0)
		{
			if (indexInfoIt(signalIndex)->second->serverNo >= 0)
			{
				//REQUEST FROM SERVER
				addNewClient(indexInfoIt(signalIndex));
				//TODO gestion retour fail
			}
			else
			{
				//ANSWER FROM CGI TO CLIENT
			}
			
		}

	}
}

//set pollfd et add new indexInfo
int	Server::addNewClient(const indexInfo_it indexInfoServer)
{
	sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	int clientFd = accept(indexInfoServer->second->fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);
	//todo controller la sortie de accept si == -1 (exception ou return)
	int pollIndex = pollFdAdd(_pollFds, clientFd);
	_indexInfo.insert(std::pair<unsigned int, indexInfo_t*>(pollIndex, new indexInfo_t));
	//todo return if fail
}

/*Return the first index of pollFds that have revents = POLLIN
	Return -1 if no fd found
	//TODO faire des vérifications de signaux si peut avoir d'autres. Si oui, changer la fonction revents != 0
*/
int	Server::pollIndexSignal() const
{
	for (int index = 0; index < pollFdSize; index++)
	{
		if (_pollFds[index].revents & POLLIN)
			return index;
	}
	return -1;
}

indexInfo_it	Server::indexInfoIt(const unsigned int pollIndex)
{
	if (_indexInfo.find(pollIndex) == _indexInfo.end())
		std::cout << "throw an exception" << std::endl;
		//TODO faire une exception pour indexInfoIt (pas suposer arriver)
	return _indexInfo.find(pollIndex);
}


void	Server::setAddrServer(sockaddr_in& addr, uint16_t port)
{
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
}



