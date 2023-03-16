#include "PollQueue.hpp"

PollQueue::PollQueue(){}
PollQueue::~PollQueue(){}

/*Initiation de pollFds avec le size requis
   Pour nbServer: Ajoute les serverFds a pollfd.fd et set events a POLLIN
   Pour maxClient, set fd, events et revents a 0
*/
pollfd*		PollQueue::pollFdInit(const int* serverFds, const int& nbServer, const int& maxClient)
{
	pollfd*	pollFds = new pollfd[nbServer + maxClient];
	
	this->pollFdSize = nbServer + maxClient;
	this->nbServer = nbServer;
	for (int i = 0; i < nbServer; i++)
		this->pollFdSetFd(pollFds, serverFds[i], i);

	for (int i = nbServer; i < nbServer + maxClient; i++)
		this->pollFdResetFd(pollFds, i);

	return pollFds;
}

/* Pour ajouter un fd client a l'array pollFd
   Recherche le premier fd = 0 et l'insere a cet endroit
   Throw une exception si l'ajout a echouer (pas de fd a 0)
*/
const bool	PollQueue::pollFdAddClient(pollfd* pollFds, const int& clientFd)
{
	int i = 0;
	
	while (++i < this->pollFdSize && pollFds[i].fd != 0){}
	if (i < this->pollFdSize)
	{
		this->pollFdSetFd(pollFds, clientFd, i);
		return true;
	}
	return false;
}

/* Set a 0 le pollFd ayant etant fd = clientFd
   Throw une exception si le fd n'a pas ete trouver
*/ 
void	PollQueue::pollFdRemoveClient(pollfd* pollFds, const int& clientFd)
{
	int i = 0;

	while (++i < this->pollFdSize && pollFds[i].fd != clientFd){}
	if (i < this->pollFdSize)
		this->pollFdResetFd(pollFds, i);
	else
		throw PollQueue::FdNotFoundException();
}

/* Ajoute a un index de pollfd* un fd donner a un index donner et 
	set events a POLLIN
*/
void	PollQueue::pollFdSetFd(pollfd* pollFds, const int& setToFd, const int& index)
{
	pollFds[index].fd = setToFd;
	pollFds[index].events = POLLIN;
	pollFds[index].revents = 0;
}

/* Set a 0 un index donner de pollfd*
*/
void	PollQueue::pollFdResetFd(pollfd* pollFds, const int& index)
{
	pollFds[index].fd = 0;
	pollFds[index].events = 0;
	pollFds[index].revents = 0;
}

const char*	PollQueue::FdNotFoundException::what() const throw()
{
	return ("Error: PollQueue::pollFdRemoveClient: clientFd not found in pollFds*");
}