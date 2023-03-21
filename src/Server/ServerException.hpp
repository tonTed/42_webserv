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
			virtual const char* what() const throw() {return (SERR_SOCKET_FAIL);}
	};

	class	FctSetsockoptFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERR_SETSOCKOPT_FAIL);}
	};

	class	FctBindFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERR_BIND_FAIL);}
	};

	class	FctListenFail: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERR_LISTEN_FAIL);}
	};

};

#endif