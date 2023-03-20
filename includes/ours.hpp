//
// Created by Teddy BLANCO on 2023-03-16.
//

#ifndef OURS_HPP
# define OURS_HPP

# define CRLF "\r\n"
# define CR '\r'

# define HTTP_VERSION "HTTP/1.1"

//Server limits
# define MAX_REQUEST_SIZE 4096


//Exception messages
# define ERR_REQUEST_SIZE "Request size is too big"
# define ERR_SOCKET_READ "Error reading socket"
# define ERR_START_LINE_NO_CRLF "Invalid start line: no CRLF"
# define ERR_START_LINE_INVALID_METHOD "Invalid start line: invalid method"
# define ERR_START_LINE_INVALID_VERSION "Invalid start line: invalid version"
# define ERR_START_LINE_INVALID_LINE "Invalid start line: invalid line"
# define ERR_HEADER_DUPLICATE "Invalid header: duplicate header"
# define ERR_HEADER_MISSING_HEADER "Invalid header: missing header"
# define ERR_HEADER_INVALID_VALUE "Invalid header: invalid value"

/**
 * @brief	Enum of all the possible request types.
 * 			Used in the StartLine struct.
 *
 * @attention	Add new request types here,
 * 				needs to add them in the _setMethod() method in Request.cpp
 *
 * */
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

#endif
