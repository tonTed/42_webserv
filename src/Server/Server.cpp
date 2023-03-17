#include "Server.hpp"

Server::Server(){}
virtual			Server::~Server()
{
	delete this->_pollFds;
	delete this->_nbPortServer;
	for (int i = 0; i < nbServer; i++)
	delete this->_ports;
}

void			Server::configToServer(const ConfigServer& config)
{
	//this->nbServer = CONFIG_NB_SERVER;
	//this->pollFdSize = CONFIG_NB_SERVER + CONFIG_MAX_CLIENT;
	//this->_nbPortServer = new unsigned int[CONFIG_NB_SERVER];
	
	//PORT COPY
	//this->_ports = new uint16_t*[CONFIG_NB_SERVER];
	//for (int i = 0; i < nbServer; i++)
	//{
		//this->_nbPortServer[i] = CONFIG_NB_PORT_IN_SERVER[i];
		//this->_ports[i] = new uint16_t[this->_nbPortServer[i]];
		//for (int i2 = 0; i2 < this->_nbPortServer[i]; i2++)
		//	this->_ports[i][i2] = CONFIG_PORT_OF_SERVER[i][i2];
	//}

	this->_pollTimeOut = 5000;
}

int				Server::setServer()
{

}