#ifndef SERVER_HPP
# define SERVER_HPP

# include "../../includes/webserv.hpp"
# include "ServerException.hpp"
# include "PollQueue.hpp"


#define CONFIG_POLLTIMEOUT -1
#define CONFIG_MAX_CLIENT 100
#define LISTEN_BACKLOG 100

//SERVER ERROR MESSAGES (SERR = SERVER ERROR) -> should never happen
# define SERR_POLLINDEXSIGNAL "Server error: poll trigger but index signal not found"

//Structure pour obtenir de l'information sur un index de _pollFds
typedef struct indexInfo_s
{
	int					serverNo; 		//0 >= serverNo <0 = client
	int					origin;			//0->fromPort 1->fromCGI 2->fromCGIParsing
	uint16_t			port;			//Structure pour bind et accept
	int					portBacklog;	//Pour la function listen, le nombre de backlog par port (config?)
	int&				fdPort;			//fd creer par socket
	int&				fdCGIRead;		//pipe for fdCGI
	int					fdCGIWrite;		//
	int					indexCGI;		//index de pollFds du CGI linked
	int					indexClient;	//index de pollFds du client linked
	int&				fdClient;		//fd creer par accept
}						indexInfo_t;

//TYPEDEF ITERATOR INDEXINFO
typedef std::map<unsigned int, indexInfo_t*>::iterator indexInfo_it;

class Server: public PollQueue
{
private:
	pollfd*									_pollFds; 		//pour poll
	std::map<unsigned int, indexInfo_t*>	_indexInfo;		//information sur chaque index de pollFds
	int										_pollTimeOut;	//Sait pas encore si fourni par config
	int										_nbfdPort;	//Nombre total de fd pour les servers (si index au dessus = client)

	void									_safePollFdsClose(const int& pollIndex);

public:
						Server();
	virtual				~Server();

	void				configToServer();

	void				routine();

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
	int					setClientCGI(const int& pollIndex);
	void				closeClient(const int& signalIndex);

	indexInfo_it		indexInfoIt(const int& pollIndex);
	void				indexInfoInsert(const int& pollIndex);
	void				AddFdIndexInfo();

};

#endif