#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/socket.h> //socket
# include <poll.h> //pollfd
# include <netdb.h> //struct sockaddr_in
# include <arpa/inet.h> //htons
# include <map>
# include <unistd.h> //close

# include "PollQueue.hpp"
# include "../ConfigServer/ConfigServer.hpp"
# include "../../includes/martin.hpp"
# include "ServerException.hpp"

//Structure pour obtenir de l'information sur un index de _pollFds
typedef struct indexInfo_s
{
	int					serverNo; 		//0 >= serverNo <0 = client
	bool				isServer;		//true if its a socket from socket/bind/listen
	uint16_t			port;			//Structure pour bind et accept
	int					portBacklog;	//Pour la function listen, le nombre de backlog par port (config?)
	int					fd;				//le fd qui sera inserer dans pollFds
}						indexInfo_t;

//TYPEDEF ITERATOR INDEXINFO
typedef std::map<unsigned int, indexInfo_t*>::iterator indexInfo_it;


class Server: public PollQueue
{
private:
	pollfd*									_pollFds; 		//pour poll
	std::map<unsigned int, indexInfo_t*>	_indexInfo;		//information sur chaque index de pollFds
	int										_pollTimeOut;	//Sait pas encore si fourni par config
	int										_nbFdServer;	//Nombre total de fd pour les servers (si index au dessus = client)

public:
						Server();
	virtual				~Server();

	void				routine(const ConfigServer& config);

	void				configToServer(const ConfigServer& config);

	//BOOTING SERVER FCT---------------------------------------

	void				ServerBooting();
	void				bootingSocket(const int& iSocket);
	void				bootingSetSockOpt(const int& iSocket);
	void				bootingBind(const int& iSocket);
	void				setAddrServer(sockaddr_in& addr, uint16_t port);
	void				bootingListen(const int& iSocket);

	//POLL LOOP------------------------------------------------

	void				ServerPollLoop();
	int					pollIndexSignal();

	int					addNewClient(const int& signalIndex);
	void				closeClient(const int& signalIndex);

	indexInfo_it		indexInfoIt(const int& pollIndex);
	void				indexInfoInsert(const int& pollIndex);
	void				AddFdIndexInfo();

};

#endif