#ifndef SERVEREXCEPTION
# define SERVEREXCEPTION

# include <stdexcept>
# include "../../includes/martin.hpp"

class ServerException
{
public:
	class	FctSocketFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_SOCKET_FAIL);}
	};

	class	FctSetsockoptFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_SETSOCKOPT_FAIL);}
	};

	class	FctBindFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_BIND_FAIL);}
	};

	class	FctListenFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_LISTEN_FAIL);}
	};

	class	FctAcceptFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_ACCEPT_FAIL);}
	};

	class	FctPollFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_POLL_FAIL);}
	};
	
	class	InsertFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_FIND_FAIL);}
	};

	class	FindFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_FIND_FAIL);}
	};
};

#endif