#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/socket.h> //socket
# include <poll.h> //pollfd
# include <netdb.h> //struct sockaddr_in
# include <arpa/inet.h> //htons

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