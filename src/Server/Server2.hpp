#ifndef SERVER2_HPP
# define SERVER2_HPP

# include "../../includes/webserv.hpp"
# include "../Request/Request.hpp"
# include "ServerException.hpp"


//SERVER ERROR MESSAGES (SERR = SERVER ERROR) -> should never happen
# define SERR_POLLINDEXSIGNAL "Server error: poll trigger but index signal not found"

# define POLLFD_LIMIT 50
# define LISTEN_BACKLOG 50
# define POLL_TIMEOUT -1

class Request;

class Server2
{
private:
	pollfd			_pollFds[POLLFD_LIMIT]; 		//pour poll
	int				_nbfdPort;	//Nombre total de fd pour les servers (si index au dessus = client)
	Request*		_reqs;

public:
					Server2();
	virtual			~Server2();

//*****************************BOOTING SERVER***********************************

	void			booting();
	int				recordPort(uint16_t& port);
	void			setPortSocket(const uint16_t& port);
	void			bootSocket(const int& iSocket);
	void			bootSetSockOpt(const int& iSocket);
	void			bootBind(const int& iSocket, const uint16_t port[POLLFD_LIMIT]);
	void			setAddrServer(sockaddr_in& addr, uint16_t port);
	void			bootListen(const int& iSocket);

//*****************************OPERATING SERVER*********************************

	void			operating();
	int				pollIndexSignal();
	
//*****************************REQUEST******************************************

	int				setRequest(Request& req, const int& signalIndex);
	int				acceptClient(const int& signalIndex);
	int				setPollFds(const int& fd);
	int				reqIndex(const int& signalIndex) const;

//****************************CLOSE CONNECTION**********************************
	void			closeConnection(const int& signalIndex);
	void			safeClose(int& fdSource);

	void			closePollFds();
};


#endif