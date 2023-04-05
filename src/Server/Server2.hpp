#ifndef SERVER2_HPP
# define SERVER2_HPP

# include "../../includes/webserv.hpp"
# include "ServerException.hpp"

//SERVER ERROR MESSAGES (SERR = SERVER ERROR) -> should never happen
# define SERR_POLLINDEXSIGNAL "Server error: poll trigger but index signal not found"

# define POLLFD_LIMIT 50
# define LISTEN_BACKLOG 50
# define POLL_TIMEOUT -1

typedef struct	indexLink_s
{
	int	serverNum;
	int	CGIReadIndex;
	int	ClientIndex;
}				indexLink_t;

typedef struct	requestInfo_s
{
	int			clientFd;
	int			CGIfdRead;
	int			CGIfdWrite;
	int			serverNum;
}				requestInfo_t;

class Server2
{
private:
	pollfd			_pollFds[POLLFD_LIMIT]; 		//pour poll
	indexLink_t		_indexInfo[POLLFD_LIMIT];		//information sur chaque index de pollFds
	int				_nbfdPort;	//Nombre total de fd pour les servers (si index au dessus = client)

public:
					Server2();
	virtual			~Server2();

//*****************************BOOTING SERVER***********************************

	void			booting();
	void			initIndexInfo();
	void			resetIndexInfo(const int& index);
	int				recordPort(uint16_t port[POLLFD_LIMIT]);
	void			setPortSocket(const uint16_t port[POLLFD_LIMIT]);
	void			bootSocket(const int& iSocket);
	void			bootSetSockOpt(const int& iSocket);
	void			bootBind(const int& iSocket, const uint16_t port[POLLFD_LIMIT]);
	void			setAddrServer(sockaddr_in& addr, uint16_t port);
	void			bootListen(const int& iSocket);

//*****************************OPERATING SERVER*********************************

	void			operating();
	int				pollIndexSignal();
	int				indexOrigin(const int& signalIndex);
	
//*****************************REQUEST******************************************

	int				setRequest(requestInfo_t reqInfo, const int& signalIndex);
	int				acceptClient(const int& signalIndex);
	bool			pollFdsAvailable() const;
	int				setPollFds(const int& fd);
	void			setIndexInfo(const int& clientIndex, const int& CGIReadIndex, const int& serverNum);

//****************************CLOSE CONNECTION**********************************
	void			closeConnection(const int& signalIndex);
	void			safeClose(int& fdSource);

	void			closePollFds();
};


#endif