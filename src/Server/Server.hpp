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
	int					serverNo; 		//0 >= serverNo <0 = client
	bool				isServer;		//true if its a socket from socket/bind/listen
	uint16_t			port;			//Structure pour bind et accept
	unsigned int		maxHeaderSize;	//Pour le buffer de lecture sur fd (via config)
	unsigned int		portBacklog;	//Pour la function listen, le nombre de backlog par port (config?)
	unsigned int		fd;				//le fd qui sera inserer dans pollFds
}						indexInfo_t;

typedef std::map<unsigned int, indexInfo_t*>::iterator indexInfo_it;

class Server: public PollQueue
{
private:
	pollfd*									_pollFds; 		//pour poll
	std::map<unsigned int, indexInfo_t*>	_indexInfo;	//information sur chaque index de pollFds
	unsigned int							_pollTimeOut;	//Sait pas encore si fourni par config
	unsigned int							_nbFdServer;	//Nombre total de fd pour les servers (si index au dessus = client)

public:
						Server();
	virtual				~Server();

	//pont entre config et server
	void				configToServer(const ConfigServer& config);


	void				setUpServer();
	void				routine();

	int					addNewClient(const indexInfo_it indexInfoServer);

	const int			pollIndexSignal() const;
	indexInfo_it		indexInfoIt(const unsigned int pollIndex);

	const struct sockaddr*	addrServer(uint16_t port);

	void				AddFdIndexInfo();

	//todo faire fonction ajout et remove de indexInfo

	//Exceptions

	class	FctSocketException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};

	class	FctSetsockoptException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};

	class	FctBindException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};

	class	FctListenException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};

};



#endif