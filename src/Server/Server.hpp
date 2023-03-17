#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>

# include "PollQueue.hpp"
# include "../ConfigServer/ConfigServer.hpp"


class Server: public PollQueue
{
private:
	pollfd*			_pollFds;
	uint16_t**		_ports;
	unsigned int*	_nbPortServer;
	unsigned int	_pollTimeOut;
	unsigned_int*	_serverFds;

public:
					Server();
	virtual			~Server();

	//pont entre config et server
	void			configToServer(const ConfigServer& config);


	int				setServer();


};



#endif