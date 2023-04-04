#ifndef SERVER2_HPP
# define SERVER2_HPP

# include "../../includes/webserv.hpp"
# include "ServerException.hpp"


# define POLLFD_LIMIT 500

typedef struct	indexLink_s
{
	int	serverNo;
	int	CGIReadIndex;
	int	ClientIndex;
}				indexLink_t;

class Server2
{
private:
	pollfd*			_pollFds[POLLFD_LIMIT]; 		//pour poll
	indexLink_t*	_indexInfo[POLLFD_LIMIT];		//information sur chaque index de pollFds
	uint16_t		port[POLLFD_LIMIT];
	int				_nbfdPort;	//Nombre total de fd pour les servers (si index au dessus = client)

public:
					Server2();
	virtual			~Server2();

	void			resetIndexInfo();
	int				RecordPort();


};


#endif