//
// Created by Teddy BLANCO on 2023-03-16.
//

#ifndef OURS_HPP
# define OURS_HPP

# define CRLF "\r\n"
# define CR '\r'
# define OWS " \t"
# define SP ' '
# define HTAB '\t'
# define VCHAR "\x21-\x7E"

# define HTTP_VERSION "HTTP/1.1"

//Server limits
# define MAX_REQUEST_SIZE 4096000

#include "../src/Log/Log.h"


/**
 * @brief	Enum of all the possible request types.
 * 			Used in the StartLine struct.
 *
 * @attention	Add new request types here,
 * 				needs to add them in the _setMethod() method in Request.cpp
 *
 * */


#endif
