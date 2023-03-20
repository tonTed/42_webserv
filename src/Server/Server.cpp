#include "Server.hpp"

Server::Server(){}
Server::~Server()
{
	delete this->_pollFds;
	//todo delete all indexInfo_t* dans _indexInfo

}

void	Server::configToServer(const ConfigServer& config)
{
	this->nbServer = CONFIG_SERVER_NB;
	this->maxClient = CONFIG_MAX_CLIENT;
	this->_pollTimeOut = -1;
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
			indexInfoIt(_nbFdServer)->second->port = CONFIG_SERVER_PORTS;
			this->_nbFdServer++;
		}
	}

	this->pollFdSize = this->_nbFdServer + this->maxClient;
}

void	Server::setUpServer()
{
	int opt = 1;

	for (int iSocket = 0; iSocket < this->_nbFdServer; iSocket++)
	{
		if (this->_indexInfo.find(iSocket)->second->fd == socket(AF_INET, SOCK_STREAM, 0) < 0)
			throw Server::FctSocketException();
		if (setsockopt(indexInfoIt(_nbFdServer)->second->fd,
				SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			throw Server::FctSetsockoptException();
		const sockaddr* addr = addrServer(indexInfoIt(_nbFdServer)->second->port);
		if (bind(indexInfoIt(_nbFdServer)->second->fd, addr, sizeof(addr) < 0))
			throw Server::FctBindException();
		if (listen(indexInfoIt(_nbFdServer)->second->fd,
				indexInfoIt(_nbFdServer)->second->portBacklog) < 0)
			throw Server::FctBindException();
		this->pollFdSetFd(this->_pollFds, this->_indexInfo.find(iSocket)->second->fd, iSocket);
	}
}
	
void	Server::routine()
{
	int	activeClient = 0;
	int signalIndex;
	//todo set a signal to change value that will stop the server properly
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
				//todo gestion retour fail
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
	struct sockaddr_in addr;
	socklen_t addrLen = sizeof(addr);
	
	int clientFd = accept(indexInfoServer->second->fd,
			reinterpret_cast<sockaddr*>(&addr), &addrLen);

	int pollIndex = pollFdAdd(_pollFds, clientFd);
	_indexInfo.insert(std::pair<unsigned int, indexInfo_t*>(pollIndex, new indexInfo_t));
//todo return if fail
}

const int	Server::pollIndexSignal() const
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
		//todo faire une exception pour indexInfoIt (pas suposer arriver)
	return _indexInfo.find(pollIndex);
}


const struct sockaddr*	Server::addrServer(uint16_t port)
{
	struct sockaddr_in addr; 
	addr.sin_family = AF_INET; 
	addr.sin_port = htons(port); 
	addr.sin_addr.s_addr = htonl(INADDR_ANY); 
	return reinterpret_cast<const struct sockaddr*>(&addr);
}


//Exception---------------

const char*	Server::FctSocketException::what() const throw()
{
	return ("Error: Server::SetUpServer: socket function fail*");
}

const char*	Server::FctSetsockoptException::what() const throw()
{
	return ("Error: Server::SetUpServer: setsockopt function fail*");
}

const char*	Server::FctBindException::what() const throw()
{
	return ("Error: Server::SetUpServer: bind function fail*");
}

const char*	Server::FctListenException::what() const throw()
{
	return ("Error: Server::SetUpServer: listen function fail*");
}
