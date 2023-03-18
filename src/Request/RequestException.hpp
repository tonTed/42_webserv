//
// Created by Teddy BLANCO on 2023-03-17.
//

#ifndef REQUESTEXCEPTION_HPP
#define REQUESTEXCEPTION_HPP

#include <exception>

class RequestException {

public:
	class ReadError : public std::exception {
		const char * what() const throw() {
			return ERR_SOCKET_READ;
		}
	};

	class MaxSize : public std::exception {
		const char * what() const throw() {
			return ERR_REQUEST_SIZE;
		}
	};
};


#endif
