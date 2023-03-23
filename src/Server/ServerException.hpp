#ifndef SERVEREXCEPTION
# define SERVEREXCEPTION

# include <stdexcept>

//EXCEPTION ERR MESSAGES (SERVEX = SERVER EXCEPTION) -> should stop server
# define SERVEX_SOCKET_FAIL "Error: ServerBooting: socket function fail"
# define SERVEX_SETSOCKOPT_FAIL "Error: ServerBooting: setsockopt function fail"
# define SERVEX_BIND_FAIL "Error: ServerBooting: bind function fail"
# define SERVEX_LISTEN_FAIL "Error: ServerBooting: listen function fail"
# define SERVEX_ACCEPT_FAIL "Error: addNewClient: accept function fail"
# define SERVEX_POLL_FAIL "Error: addNewClient: poll function fail"
# define SERVEX_INSERT_FAIL "Error: indexInfoIt: map.insert(): map.insert fail in _indexInfo"
# define SERVEX_FIND_FAIL "Error: indexInfoIt: map.find(): index not found in _indexInfo"

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