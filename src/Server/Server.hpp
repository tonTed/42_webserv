#ifndef SERVER_HPP
# define SERVER_HPP

# include "PollQueue.hpp"
# include "../ConfigServer/ConfigServer.hpp"


class Server: public PollQueue
{
private:
	pollfd*			_pollFds;

					Server();
					Server(const Server& rhs);
	Server&			operator=(const Server& rhs);
public:
					Server(const ConfigServer& config);
	virtual			~Server();

	int				setServer(const ConfigServer& config);
	


};



#endif