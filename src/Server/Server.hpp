#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>

# include "PollQueue.hpp"
# include "../ConfigServer/ConfigServer.hpp"

typedef struct serverData_t
{
	std::vector<uint16_t>	ports;
	
}


class Server: public PollQueue
{
private:
	pollfd*			pollFds;
	std::vector<std::vector<uint16_t>> _port;
	unsigned int	pollTimeOut;

					Server();
					Server(const Server& rhs);
	Server&			operator=(const Server& rhs);
public:
					Server();
					Server(const ConfigServer& config);
	virtual			~Server();

	int				setServer(const ConfigServer& config);
	void			configToServer(const ConfigServer& config);


};



#endif