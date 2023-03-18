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
	this->pollFdSize = nbServer + CONFIG_MAX_CLIENT;
	this->_pollTimeOut = CONFIG_POLLTIMEOUT;
	this->_nbFdServer = 0;
	//PORT COPY
	for (int serverNo = 0; serverNo < nbServer; serverNo++)
	{
		for (int iPort2 = 0; iPort < CONFIG_SERVER_NBPORT[serverNo]; iPort++)
		{
			this->_indexInfo.insert(std::pair<int, indexInfo_t*>(_nbFdServer, new indexInfo_t));
			this->_indexInfo.find(this->_nbFdServer)->second->serverNo = serverNo;
			this->_indexInfo.find(this->_nbFdServer)->second->maxHeaderSize = CONFIG_SERVER_MAXHEADERSIZE;
			this->_indexInfo.find(this->_nbFdServer)->second->portBacklog = CONFIG_SERVER_PORTS_BACKLOG[iPort];
			this->_indexInfo.find(this->_nbFdServer)->second->addr.sin_port = htons(CONFIG_SERVER_PORTS[iPort]);
			this->_indexInfo.find(this->_nbFdServer)->second->addr.sin_family = AF_INET; 
			this->_indexInfo.find(this->_nbFdServer)->second->addr.sin_addr.s_addr = htonl(INADDR_ANY); 
			this->_nbFdServer++;
		}
	}
}

void	Server::setUpServer()
{
	int opt = 1;
	
	for (int iSocket = 0; iSocket < this->_nbFdServer; iSocket++)
	{
		if (this->_indexInfo.find(iSocket)->second->fd == socket(AF_INET, SOCK_STREAM, 0) < 0)
			throw Server::FctSocketException();
		if (setsockopt(this->_indexInfo.find(_nbFdServer)->second->fd,
				SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
			throw Server::FctSetsockoptException();
		if (bind(this->_indexInfo.find(_nbFdServer)->second->fd,
				reinterpret_cast<const sockaddr*>(&(this->_indexInfo.find(_nbFdServer)->second->addr)),
				sizeof(&(this->_indexInfo.find(_nbFdServer)->second->addr))) < 0)
			throw Server::FctBindException();
		if (listen(this->_indexInfo.find(_nbFdServer)->second->fd,
				this->_indexInfo.find(_nbFdServer)->second->portBacklog) < 0)
			throw Server::FctBindException();
	}
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
