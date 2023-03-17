#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/socket.h> //socket
# include <poll.h> //pollfd
# include <netdb.h> //struct sockaddr_in
# include <arpa/inet.h> //htons
# include <map>

# include "PollQueue.hpp"
# include "../ConfigServer/ConfigServer.hpp"

typedef struct indexInfo_s
{
	unsigned int		serverNo; 		//0 = client >0 = serverNo
	struct sockaddr_in	addr;			//Structure pour bind et accept
	unsigned int		maxHeaderSize;	//Pour le buffer de lecture sur fd (via config)
	unsigned int		fd;				//le fd qui sera inserer dans pollFds
}						indexInfo_t;

class Server: public PollQueue
{
private:
	pollfd*									_pollFds; 		//pour poll
	std::map<unsigned int, indexInfo_t*>	_indexInfo;	//information sur chaque index de pollFds
	unsigned int*							_nbPortServer;	//nombre de port par server (via config)
	unsigned int							_pollTimeOut;	//Sait pas encore si fourni par config
	unsigned int							_nbFdServer;	//Nombre total de fd pour les servers (si index au dessus = client)

public:
					Server();
	virtual			~Server();

	//pont entre config et server
	void			configToServer(const ConfigServer& config);


	void			setUpServer();
	void			AddFdIndexInfo(

	

};



#endif