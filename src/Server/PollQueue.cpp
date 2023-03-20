#include "PollQueue.hpp"

PollQueue::PollQueue(){}
PollQueue::~PollQueue(){}

/*Initiation de pollFds avec le size requis
   set fd, events et revents a 0
*/
pollfd*		PollQueue::pollFdInit()
{
	pollfd*	pollFds = new pollfd[pollFdSize];
	
	for (unsigned int i = 0; i < pollFdSize; i++)
		this->pollFdResetFd(pollFds, i);

	return pollFds;
}

/* Pour ajouter un fd client a l'array pollFd
   Recherche le premier fd = 0 et l'insere a cet endroit
   Renvoie index si aucun erreur / -1 si non inséré
*/
int	PollQueue::pollFdAdd(pollfd* pollFds, const unsigned int& fd)
{
	unsigned int i = -1;
	
	while (++i < this->pollFdSize && pollFds[i].fd != 0){}
	if (i < this->pollFdSize)
	{
		this->pollFdSetFd(pollFds, fd, i);
		return i;
	}
	return -1;
}

/* Set a 0 le pollFd ayant etant fd = clientFd
   Throw une exception si le fd n'a pas ete trouver
*/ 
void	PollQueue::pollFdRemove(pollfd* pollFds, int fd)
{
	unsigned int i = -1;

	while (++i < this->pollFdSize && pollFds[i].fd != fd){}
	if (i < this->pollFdSize)
		this->pollFdResetFd(pollFds, i);
	else
		throw PollQueue::FdNotFoundException();
}

/* Ajoute a un index de pollfd* un fd donner a un index donner et 
   set events a POLLIN
*/
void	PollQueue::pollFdSetFd(pollfd* pollFds, const unsigned int& setToFd, const unsigned int& index)
{
	pollFds[index].fd = setToFd;
	pollFds[index].events = POLLIN;
	pollFds[index].revents = 0;
}

/* Set a 0 un index donner de pollfd*
*/
void	PollQueue::pollFdResetFd(pollfd* pollFds, const unsigned int& index)
{
	pollFds[index].fd = 0;
	pollFds[index].events = 0;
	pollFds[index].revents = 0;
}

//Getters--------------

unsigned int	PollQueue::getNbServer() const {return this->nbServer;}
unsigned int	PollQueue::getPollFdSize() const {return this->pollFdSize;}

//setters

void	PollQueue::setNbServer(const unsigned int& nbServer) {this->nbServer = nbServer;}
void	PollQueue::setPollFdSize(const unsigned int& pollFdSize) {this->pollFdSize = pollFdSize;}

//Exception---------------

const char*	PollQueue::FdNotFoundException::what() const throw()
{
	return ("Error: PollQueue::pollFdRemoveClient: clientFd not found in pollFds*");
}