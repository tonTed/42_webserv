#ifndef SERVEREXCEPTION
# define SERVEREXCEPTION

# include <stdexcept>

//EXCEPTION ERR MESSAGES (SERVEX = SERVER EXCEPTION) -> should stop server
# define SERVEX_SOCKET_FAIL "Error: ServerBooting: socket function fail"
# define SERVEX_SETSOCKOPT_FAIL "Error: ServerBooting: setsockopt function fail"
# define SERVEX_BIND_FAIL "Error: ServerBooting: bind function fail"
# define SERVEX_LISTEN_FAIL "Error: ServerBooting: listen function fail"
# define SERVEX_ACCEPT_FAIL "Error: addNewClient: accept function fail"
# define SERVEX_SIGINT "SIGINT stopping server"

class ServerBootingException
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

};

class ServerOperatingException
{
public:
	class	EndServer: public std::exception
	{
		public:
			virtual const char* what() const throw() {return (SERVEX_SIGINT);}
	};
};

#endif