#include "Server.hpp"

Server::Server(){}
Server::~Server()
{
	delete this->_pollFds;
	delete this->_nbPortServer;

}

void	Server::configToServer(const ConfigServer& config)
{
	//this->nbServer = CONFIG_NB_SERVER;
	//this->pollFdSize = nbServer + CONFIG_MAX_CLIENT;
	//this->_nbPortServer = new unsigned int[nbServer];
	//this->_nbFdServer = 0
	//PORT COPY
	//for (int i = 0; i < nbServer; i++)
	//{
		//this->_nbPortServer[i] = CONFIG_NB_PORT_IN_SERVER[i];
		//this->_ports[i] = new uint16_t[this->_nbPortServer[i]];
		//for (int i2 = 0; i2 < this->_nbPortServer[i]; i2++)
		//	this->_ports[i][i2] = CONFIG_PORT_OF_SERVER[i][i2];
	//}

	this->_pollTimeOut = 5000;
}


//TODO Initialiser _nbFdServer & _indexInfo dans configToServer et non dans setUpServer


void	Server::setUpServer()
{

	int opt = 1;
	this->_nbFdServer = 0;
	
	for (int iServer = 0; iServer < nbServer; iServer++)
	{
		for (int iPort = 0; iPort < _nbPortServer[iServer]; iPort++)
		{
			this->_indexInfo.insert(std::pair<int, indexInfo_t*>(_nbFdServer, new indexInfo_t));
			
			if (this->_indexInfo.find(_nbFdServer)->second->fd == socket(AF_INET, SOCK_STREAM, 0) < 0)
				std::cout << "THROWING PERSONALIZE EXCEPTION" << std::endl;




			this->_nbFdServer++;
		}
	}
}
	
	if (serverSocket == socket(AF_INET, SOCK_STREAM, 0) < 0)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	// Forcefully attaching socket to the port 80
	if (setsockopt(serverSocket, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	
	if (bind(serverSocket, this->addr(), sizeof(addr)) < 0)
	{
		perror("bindResult");
		exit(EXIT_FAILURE);
	}
	if (listen(serverSocket, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	this->fds.push_back(this->newPollFD(serverSocket));

