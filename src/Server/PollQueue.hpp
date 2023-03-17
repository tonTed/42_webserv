#ifndef POLLQUEUE_HPP
# define POLLQUEUE_HPP

# include <poll.h> //pollfd
# include <stdexcept>

class PollQueue
{
protected:
	unsigned int	pollFdSize;
	unsigned int	nbServer;
	

public:
					PollQueue();
	virtual			~PollQueue();

	//init
	pollfd*			pollFdInit(const unsigned int* serverFds);

	//ClientFd
	const bool		pollFdAddClient(pollfd* pollFds, const unsigned int& clientFd);
	void			pollFdRemoveClient(pollfd* pollFds, const unsigned int& clientFd);
	
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