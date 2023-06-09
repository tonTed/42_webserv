//
// Created by Teddy BLANCO on 2023-03-16.
//

#ifndef WEBSERV_HPP
# define WEBSERV_HPP


# include <iostream>
# include <fstream>
# include <sstream>
# include <algorithm>
# include <map>
# include <vector>
# include <string>
# include <stdexcept>

# include <stdlib.h>
# include <ctype.h>
# include <poll.h>			//pollfd
# include <sys/socket.h>	//socket
# include <poll.h>			//pollfd
# include <netdb.h>			//struct sockaddr_in
# include <arpa/inet.h>		//htons
# include <unistd.h>		//close
# include <errno.h>			//errno for poll
# include <csignal>
# include <cstdio>
#include <dirent.h>			//DIR

typedef std::string string;

enum eRequestType
{
	GET,
	POST,
	PUT,
	DELETE,
	HEAD,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH,
	UNKNOWN
};

enum eOriginFD
{
	FROM_PORT,
	FROM_CGI,
	FROM_CLIENT,
	SIGNAL_NOT_IN_POLLFDS = -1,
	SIGNAL_NOT_POLLIN = -2
};

enum ePipeDir
{
	PIPE_READ,
	PIPE_WRITE,
};

# define EXIT_SUCCESS 0
# define EXIT_FAILURE 1

# include "server_limits.hpp"
# include "exception_messages.hpp"
# include "colors.hpp"

# include "../src/ConfigServer/ConfigServer.hpp"
# include "../src/ListDir/ListDir.hpp"

// TODO: remove this
# include "hafid.hpp"
# include "martin.hpp"
# include "ours.hpp"

#endif
