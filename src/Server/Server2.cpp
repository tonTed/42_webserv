#include "Server2.hpp"

Server2::Server2(){}

Server2::~Server2(){}



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
			_indexInfo[_nbfdPort]->CGIReadIndex = -1;
			_indexInfo[_nbfdPort]->ClientIndex = -1;
			_nbfdPort++;
		}
	}
}

