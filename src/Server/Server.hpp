#ifndef SERVER_HPP
# define SERVER_HPP

# include "../../includes/webserv.hpp"
# include "../Request/Request.hpp"
# include "../Response/Response.hpp"
# include "../Response/DefaultHtml.hpp"
# include "ServerException.hpp"


//SERVER ERROR MESSAGES (SERR = SERVER ERROR) -> should never happen
# define SERR_POLLINDEXSIGNAL "Server error: poll trigger but index signal not found"

# define POLLFD_LIMIT 50
# define LISTEN_BACKLOG 50
# define POLL_TIMEOUT -1
# define UNSET -1

typedef struct	indexInfo_s
{
	int	serverNum;
	int	CGIReadIndex;
	int	clientIndex;
}				indexInfo_t;

class Request;

class Server
{
private:
	pollfd			_pollFds[POLLFD_LIMIT]; 		//pour poll
	int				_nbfdPort;	//Nombre total de fd pour les servers (si index au dessus = client)
	indexInfo_s		_indexInfo[POLLFD_LIMIT];
	Request*		_reqs;
	int				_activeFds;

public:
					Server();
	virtual			~Server();

//*****************************SET/GET/TERS*************************************
	void			setNbFdPort(const int& nbPort);
	int				getNbFdPort() const;
	//II -> IndexInfo
	void			setIIServerNum(const int& index, const int& serverNum);
	int				getIIServerNum(const int& index) const;
	void			setIICGIReadIndex(const int& index, const int& CGIReadIndex);
	int				getIICGIReadIndex(const int& index) const;
	void			setIIClientIndex(const int& index, const int& ClientIndex);
	int				getIIClientIndex(const int& index) const;
	//PF -> pollFds
	void			setPFFd(const int& index, const int& fd);
	int				getPFFd(const int& index) const;
	void			setPFEvents(const int& index, const int& events);
	int				getPFEvents(const int& index) const;
	void			setPFRevents(const int& index, const int& revents);
	int				getPFRevents(const int& index) const;
	

//*****************************BOOTING SERVER***********************************

	void			booting();
	void			pollFdsInit();
	void			recordPort(uint16_t port[POLLFD_LIMIT]);
	void			setPortSocket(const uint16_t port[POLLFD_LIMIT]);
	void			bootSocket(const int& iSocket);
	void			bootSetSockOpt(const int& iSocket);
	void			bootBind(const int& iSocket, const uint16_t port[POLLFD_LIMIT]);
	void			setAddrServer(sockaddr_in& addr, uint16_t port);
	void			bootListen(const int& iSocket);

//*****************************OPERATING SERVER*********************************

	void			serverRoutine();
	void			operating();
	int				pollIndexSignal();
	int				indexOrigin(const int& signalIndex) const;
	
//*****************************REQUEST******************************************

	bool			setRequest(const int& signalIndex);
	int				acceptClient(const int& signalIndex);
	bool			pollFdsAvailable() const;
	int				setPollFds(const int& fd);
	int				reqIndex(const int& signalIndex) const;

//****************************INDEXINFO*****************************************

	void			indexInfoInit();
	void			resetIndexInfo(const int& index);
	void			setIndexInfo(const int& clientIndex, const int& CGIReadIndex, const int& serverNum);

//****************************CLOSE CONNECTION**********************************
	void			closeConnection(const int& signalIndex);
	void			safeClose(int& fdSource);
	void			closePollFds();
};


#endif