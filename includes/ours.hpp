//
// Created by Teddy BLANCO on 2023-03-16.
//

#ifndef OURS_HPP
# define OURS_HPP

//Server limits
# define MAX_REQUEST_SIZE 4096


//Exception messages
# define ERR_REQUEST_SIZE "Request size is too big"

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
	UNKNOWN
};

#endif
