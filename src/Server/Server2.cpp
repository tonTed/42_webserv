#include "Server2.hpp"

Server2::Server2(){}

Server2::~Server2(){}


void	Server2::resetIndexInfo()
{
	for (int index = 0; index < POLLFD_LIMIT; index++)
	{
		_indexInfo[index]->serverNo = -1;
		_indexInfo[index]->CGIReadIndex = -1;
		_indexInfo[index]->ClientIndex = -1;
	}
}


//PONT ENTRE CONFIG ET SERVER
//INITIATION DE CERTAINES VARIABLE
int	Server2::RecordPort()
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
			_indexInfo[_nbfdPort]->serverNo = nbServer;
			_nbfdPort++;
		}
	}
}

