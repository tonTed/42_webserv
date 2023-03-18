#ifndef POLLQUEUE_HPP
# define POLLQUEUE_HPP

# include <poll.h> //pollfd
# include <stdexcept>

class PollQueue
{
protected:
	unsigned int	pollFdSize;
	unsigned int	nbServer;
	unsigned int	maxClient;
	unsigned int	nbServerFd;

public:
					PollQueue();
	virtual			~PollQueue();

	//init
	pollfd*			pollFdInit();

	//ClientFd
	const int		pollFdAdd(pollfd* pollFds, const unsigned int& fd);
	void			pollFdRemove(pollfd* pollFds, const unsigned int& fd);
	
	// Set/reset
	void			pollFdSetFd(pollfd* pollFds, const unsigned int& setToFd, const unsigned int& index);
	void			pollFdResetFd(pollfd* pollFds, const unsigned int& index);

	//getter
	const unsigned int	getNbServer() const;
	const unsigned int	getPollFdSize() const;

	//setter
	void				setNbServer(const unsigned int& nbServer);
	void				setPollFdSize(const unsigned int& pollFdSize);

	//Exception de pollFdRemoveClient
	class			FdNotFoundException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};
};

#endif