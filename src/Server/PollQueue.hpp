#ifndef POLLQUEUE_HPP
# define POLLQUEUE_HPP

# include <poll.h> //pollfd
# include <stdexcept>

class PollQueue
{
protected:
	int			pollFdSize;
	int			nbServer;
				
				PollQueue();
public:
	virtual		~PollQueue();

	//init
	pollfd*		pollFdInit(const int* serverFds, const int& nbServer, const int& maxClient);
	
	//ClientFd
	const bool	pollFdAddClient(pollfd* pollFds, const int& clientFd);
	void		pollFdRemoveClient(pollfd* pollFds, const int& clientFd);
	
	// Set/reset
	void		pollFdSetFd(pollfd* pollFds, const int& setToFd, const int& index);
	void		pollFdResetFd(pollfd* pollFds, const int& index);

	//Exception de pollFdRemoveClient
	class	FdNotFoundException: public std::exception
	{
		public:
			virtual const char* what() const throw();
	};
};

#endif